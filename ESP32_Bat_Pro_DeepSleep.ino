/*
Simple Deep Sleep with Timer Wake Up
=====================================
ESP32 offers a deep sleep mode for effective power saving as power is an important factor for IoT
applications. In this mode CPUs, most of the RAM, and all the digital peripherals which are clocked
from APB_CLK are powered off. The only parts of the chip which can still be powered on are:
RTC controller, RTC peripherals and RTC memories.

This code displays the most basic deep sleep with a timer to wake it up and how to store data in
RTC memory to use it over reboots

The example disables the LoBat interrupt generated by the LC709203F

This code is in the Public Domain.

daniel at ezsbc.com
*/

#include "LC709203F.h"            // From https://github.com/EzSBC/ESP32_Bat_Pro

#define uS_TO_S_FACTOR 1000000    // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  15         // Time ESP32 will go to sleep (in seconds) 

RTC_DATA_ATTR int bootCount = 0;  //Remove low power RAM access for lowest power consumption

LC709203F gg;                     // gas gauge

/*
Method to print the reason by which ESP32 has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void setup(){
  Serial.begin(115200);
  gg.begin() ;  // Initialize the LC709203F
  gg.setAlarmVoltage( 0 );      // Zero voltage to prevent trigering the interrupt
  gg.setAlarmRSOC( 0 ) ;        // Zero capacity to prevent trigering the interrupt
  delay(300); //Take some time to open up the Serial Monitor

  //Increment boot number and print it every reboot
  ++bootCount;  // Don't use RTC_DATA_ATTR access for minimum power consumption in deep sleep
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every TIME_TO_SLEEP seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  /*
  Now that we have setup a wake cause and if needed setup the peripherals state in deep sleep, we can now start going to
  deep sleep.   In the case that no wake up sources were provided but deep sleep was started, it will sleep forever unless hardware
  reset occurs.
  */
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop(){
  //This is not going to be called
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
