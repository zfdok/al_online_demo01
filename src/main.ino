#include "config.h"
int rollback = 0;

void setup()
{
  hardware_init(); //硬件初始化
  delay(2000);
  if (rollback)
  {
    /*************如果rollback置1, 会恢复出厂设置,数据全清***********/
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
    if (oledState == OLED_OFF)
    {
      send_Msg_var_GSM_while_OLED_off();
      delay(1000);
      go_sleep();
    }
  }
}

void loop()
{
}

void send_Msg_var_GSM_while_OLED_off()
{
  setupModem();          //SIM800L物理开机
  modemToGPRS();         //modem连接GPRS
  getLBSLocation();      //获取定位信息
  sht20getTempAndHumi(); //获取温湿度数据
  onenet_connect();
  if (client.connected())
  {
    client.subscribe("$sys/370098/esp_device001/cmd/request/#"); //订阅命令下发主题
    sendTempAndHumi();
  }
}