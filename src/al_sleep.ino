#include "config.h"

void go_sleep_a_while_with_ext0()
{
  esp_sleep_enable_ext0_wakeup(WEAKUPKEY1, LOW);
  sleep_update_time();
  if (workingState == WORKING)
    esp_sleep_enable_timer_wakeup(sleeptime - reduce_sleeptime);
  Serial.println("now sleep for " + (String)((sleeptime - reduce_sleeptime)/1000000)+"seconds!");
  esp_deep_sleep_start();
}