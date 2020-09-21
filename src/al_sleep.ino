#include "config.h"

void go_sleep()
{
  esp_sleep_enable_timer_wakeup(20000000);
  Serial.println("now sleep");
  esp_deep_sleep_start();
}