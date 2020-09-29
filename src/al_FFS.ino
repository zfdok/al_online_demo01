#include "config.h"

void alFFS_init()
{
  if (firstBootFlag)
    Serial.printf("format FFS:%d", SPIFFS.format());
  else
    Serial.printf("FFS size:%d,used:%d\r\n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
}