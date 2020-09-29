#include "config.h"
int rollback = 0;
void setup()
{
  hardware_init(); //硬件初始化
  delay(2000);
  if (rollback)
  {
    /*************clean the eeprom and reset to first start***********/
    Serial.println("clean EEPROM");
    EEPROM.write(1, 0);
    EEPROM.commit();
    Serial.println("OK");
    ESP.deepSleep(300000000);
  }
  else
  {
    get_eeprom_firstBootFlag(); //获取EEPROM第1位,判断是否是初次开机
    alFFS_init();               //初始化FFS
    eeprom_config_init();       //初始化EEPROM
    software_init();            //软件初始化
  }
}

void loop()
{
  if (measureing_flag == 1)
  {
    setupModem();          //SIM800L物理开机
    modemToGPRS();         //modem连接GPRS
    getLBSLocation();      //获取定位信息
    ali_mqtt_connect();    //连接云平台
    sht20getTempAndHumi(); //获取温湿度数据
    mqttPublish_ntpTimeRequest();
    sendTemp_Humi_LBS();
    delay(100);
    //进入睡眠
    go_sleep();
  }
}