#include "config.h"

void go_sleep()
{
  esp_sleep_enable_timer_wakeup(sleeptime-reduce_sleeptime);
  Serial.println("now sleep");
  esp_deep_sleep_start();
}