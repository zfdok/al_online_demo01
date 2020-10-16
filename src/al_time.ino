#include "config.h"

void sleep_update_time()
{
  sleep_start_time = system_get_time();
  waking_update_time();
  Serial.printf("sleep at : %d:%d:%d\r\n", rtc.now().hour(), rtc.now().minute(), rtc.now().second());
  EEPROM.writeULong(39, now_unixtime + (millis() - time_last_async_stamp) / 1000);
  EEPROM.commit();
}
void wakeup_init_time()
{
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  sleep_end_time = system_get_time();
  sleep_time_count = sleep_end_time - sleep_start_time;
  Serial.printf("sleep count: %ld second\r\n", sleep_time_count / 1000000);
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0 && workingState == WORKING)
  {
    reduce_sleeptime += sleep_time_count;
  }
}
void waking_update_time()
{
  rtc.adjust(DateTime(now_unixtime + (millis() - time_last_async_stamp) / 1000));
}
