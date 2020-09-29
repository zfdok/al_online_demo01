#if !defined(CONFIG_H)
#define CONFIG_H
#include "Arduino.h"
#include "Wire.h"
#include "uFire_SHT20.h"
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800
//引入TinyGSM库. 在引入之前要定义好TINY_GSM_MODEM_SIM800,让它知道我们用的模块型号
#include <TinyGsmClient.h>
#include "PubSubClient.h"
#include "aliyun_mqtt.h"
#include "ArduinoJson.h"
#include "SPIFFS.h"
#include "EEPROM.h"

/*-------------------------------SIM800L 硬件定义----------------------------------*/
#define MODEM_RST 5       //SIM800L复位引脚接在GPIO5
#define MODEM_PWRKEY 4    //SIM800L开关机引脚接在GPIO4
#define MODEM_POWER_ON 23 //SIM800L电源引脚接在GPIO23
#define MODEM_TX 27       //SIM800L串口TX引脚接在GPIO27
#define MODEM_RX 26       //SIM800L串口RX引脚接在GPIO26

/*-------------------------------其他硬件定义-------------------------------------*/
#define SerialMon Serial //调试串口为UART0
#define SerialAT Serial1 //AT串口为UART1

/*-------------------------------公共变量,参数定义-------------------------------------*/
//温湿度采集相关
float currentTemp;
float currentHumi;
bool tempAndHumi_Ready;
bool timeNTPdone;
//判断是否第一次启动
bool firstBootFlag;

/*-------------------------------公共变量,参数定义-------------------------------------*/
String originSet = "{\"sleeptime\":20000000,\"end\":0}";

//以下参数需要休眠RTC记忆
RTC_DATA_ATTR time_t sleeptime;           //休眠时间
RTC_DATA_ATTR time_t reduce_sleeptime;    //缩减休眠时间
RTC_DATA_ATTR time_t lastNTP_timestamp;   //上次对时的时间戳
RTC_DATA_ATTR int postMsgId = 0;          //记录已经post了多少条
RTC_DATA_ATTR float locationE, locationN; //地理位置,经度纬度
RTC_DATA_ATTR tm *timeNow;                //当前时间
RTC_DATA_ATTR int measureing_flag;        //是否在测量中

/*-------------------------------初始化相关init.ino-------------------------------------*/
void hardware_init();
void software_init();

/*-------------------------------温湿度采集相关al_sht20.ino-----------------------------*/
uFire_SHT20 sht20;
void sht20getTempAndHumi();

/*-------------------------------SIM800相关network.ino---------------------*/
TinyGsm modem(SerialAT);
TinyGsmClient gsmclient(modem);

void setupModem();
void modemToGPRS();
void getLBSLocation();
/*-------------------------------mqtt服务相关ali_mqtt.ino---------------------*/
PubSubClient mqttClient(gsmclient);

void ali_mqtt_connect();
void callback(char *topic, byte *payload, unsigned int length);
void sendTemp_Humi_LBS();
void mqttPublish_ntpTimeRequest();

/*-------------------------------休眠服务相关al_sleep.ino---------------------*/
void go_sleep();

/*--------------------------------eeprom相关函数--------------------*/
void get_eeprom_firstBootFlag();
void eeprom_config_init();
void eeprom_config_set_sleeptime(time_t time1);
/*********************************SPIFFS相关函数 al_FFS.ino**********/
void alFFS_init();

#endif // CONFIG_H