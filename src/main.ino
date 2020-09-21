#include "config.h"

void setup()
{
  hardware_init();       //硬件初始化
  software_init();       //软件初始化
  setupModem();          //SIM800L物理开机
  modemToGPRS();         //modem连接GPRS
  getLBSLocation();      //获取定位信息
  ali_mqtt_connect();    //连接云平台
  sht20getTempAndHumi(); //获取温湿度数据
  sendTemp_Humi_LBS();
  mqttPublish_ntpTimeRequest();
  delay(100);
  //进入睡眠
  go_sleep();
}

void loop()
{
}