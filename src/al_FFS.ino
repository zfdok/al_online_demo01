#include "config.h"

void alFFS_init()
{
  if (firstBootFlag)
    Serial.printf("format FFS:%d", SPIFFS.format());
  else
    Serial.printf("FFS size:%d,used:%d\r\n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
}

void alFFS_addRec()
{
  char tempStr[15];
  char tempStrtemplate[] = "%d%02d%02d %02d:%02d";
  snprintf(tempStr, 15, tempStrtemplate, rtc.now().year(), rtc.now().month(), rtc.now().day(), rtc.now().hour(), rtc.now().minute());
  Serial.print("DATE:");
  Serial.println(tempStr);
  Serial.print("now the alFFS_thisRec_firstData_flag value is :");
  Serial.println(alFFS_thisRec_firstData_flag);
  if (alFFS_thisRec_firstData_flag)
  {
    alFFS_thisRec_firstData_flag = false;
    Serial.println("first rec, so create a file named:");
    char tempPathtemplate[] = "/R%d%02d%02d_%02d%02d.json";
    snprintf(nowREC_filepath, 21, tempPathtemplate, rtc.now().year(), rtc.now().month(), rtc.now().day(), rtc.now().hour(), rtc.now().minute());
    Serial.println(nowREC_filepath);
    Serial.println("now first write content to it");
    File f = SPIFFS.open(nowREC_filepath, FILE_WRITE);
    String strtemp = "{\"st\":\"" + (String)tempStr +
                     "\",\"data\": [{\"tm\":\"" + (String)tempStr +
                     "\",\"tmsp\":" + (String)(rtc.now().unixtime() - 8 * 60 * 60) +
                     ",\"tp\":" + (String)currentTemp +
                     ",\"h\":" + (String)currentHumi +
                     ",\"E\":" + (String)locationE +
                     ",\"N\":" + (String)locationN +
                     "}";
    f.println(strtemp);
    Serial.println("ADD:" + strtemp);
    f.close();
    alFFS_thisRec_firstData_flag = 0;
  }
  else
  {
    Serial.println("not the first rec, so i can just append some content in to the file:");
    Serial.println(nowREC_filepath);
    File f = SPIFFS.open(nowREC_filepath, FILE_APPEND);
    String strtemp = ",{\"tm\":\"" + (String)tempStr +
                     "\",\"tmsp\":" + (String)(rtc.now().unixtime() - 8 * 60 * 60) +
                     ",\"tp\":" + (String)currentTemp +
                     ",\"h\":" + (String)currentHumi +
                     ",\"E\":" + (String)locationE +
                     ",\"N\":" + (String)locationN +
                     "}";
    f.println(strtemp);
    Serial.println("ADD:" + strtemp);
    f.close();
  }
}

void alFFS_readRecing()
{
  File f = SPIFFS.open(nowREC_filepath, FILE_READ);
  // String strtemp;
  // strtemp = f.readString();
  Serial.println("read out the file:");
  Serial.println(f.readString());
  Serial.println("Size of json_file :" + (String)(f.size()) + "B");
  Serial.println("Size of json_file :" + (String)(f.size() / 1024.0) + "KB");
  Serial.println("Size of json_file :" + (String)((f.size() / 1024.0) / 1024.0) + "MB");
}

void alFFS_endRec()
{
  char tempStr[15];
  char tempStrtemplate[] = "%d%02d%02d %02d:%02d";
  snprintf(tempStr, 15, tempStrtemplate, rtc.now().year(), rtc.now().month(), rtc.now().day(), rtc.now().hour(), rtc.now().minute());
  Serial.print("DATE:");
  Serial.println(tempStr);
  File f = SPIFFS.open(nowREC_filepath, FILE_APPEND);
  String strtemp = "],\"et\":\"" + (String)tempStr + "\"}";
  f.println(strtemp);
  Serial.println("ADD:" + strtemp);
  f.close();
  alFFS_readRecing();
}
