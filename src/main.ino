#include "config.h"
void setup()
{
  hardware_init(); //硬件初始化
  software_init(); //软件初始化
  delay(5000);
  /*************clean the eeprom and reset to first start***********
  // EEPROM.write(1,0);
  // EEPROM.commit();
  // Serial.println("OK");
  *****************************************************************/
  eeprom_config_init();
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

void loop()
{
}