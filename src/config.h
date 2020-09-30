#if !defined(CONFIG_H)
#define CONFIG_H
#include "Arduino.h"
#include "Wire.h"
#include "uFire_SHT20.h"
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800
//引入TinyGSM库. 在引入之前要定义好TINY_GSM_MODEM_SIM800,让它知道我们用的模块型号
#include <TinyGsmClient.h>
#include "PubSubClient.h"
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

/*-------------------------------显示/按键相关定义-------------------------------------*/
#define OLED_OFF 0
int oledState = OLED_OFF;

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
RTC_DATA_ATTR time_t sleeptime;                      //休眠时间
RTC_DATA_ATTR time_t reduce_sleeptime;               //缩减休眠时间
RTC_DATA_ATTR time_t lastNTP_timestamp;              //上次对时的时间戳
RTC_DATA_ATTR int postMsgId = 0;                     //记录已经post了多少条
RTC_DATA_ATTR float locationE, locationN, locationA; //地理位置,经度纬度
RTC_DATA_ATTR int timeNow_Y, timeNow_M, timeNow_D, timeNow_h, timeNow_m, timeNow_s;
RTC_DATA_ATTR int timeLastNTP_Y, timeLastNTP_M, timeLastNTP_D, timeLastNTP_h, timeLastNTP_m, timeLastNTP_s;
RTC_DATA_ATTR int measureing_flag; //是否在测量中

/*-------------------------------初始化相关init.ino-------------------------------------*/
void hardware_init();
void software_init();

/*-------------------------------温湿度采集相关al_sht20.ino-----------------------------*/
uFire_SHT20 sht20;
void sht20getTempAndHumi();

/*-------------------------------SIM800相关network.ino---------------------*/
// 创建一个关联到SerialAT的SIM800L模型
TinyGsm modem(SerialAT);
// 创建一个GSM型的网络客户端
TinyGsmClient gsmclient(modem);
PubSubClient client(gsmclient);

void setupModem();
void modemToGPRS();
void getLBSLocation();
// /*-------------------------------ali_mqtt服务相关ali_mqtt.ino---------------------*/
// void ali_mqtt_connect();
// void ali_callback(char *topic, byte *payload, unsigned int length);
// void ali_sendTemp_Humi_LBS();
/*-------------------------------onenet_mqtts服务相关onenet_mqtts.ino---------------------*/

/*-------------------------------云平台相关定义-------------------------------------*/
const char *mqtt_server = "183.230.40.96"; //onenet 的 IP地址
const int port = 1883;                     //端口号
#define mqtt_devid "esp_device001"         //设备ID
#define mqtt_pubid "370098"                //产品ID
//鉴权信息
#define mqtt_password "version=2018-10-31&res=products%2F370098%2Fdevices%2Fesp_device001&et=4092512761&method=md5&sign=MUV%2BKFLzv81a4Bw6BDrChQ%3D%3D"

char msgJson[256]; //要发送的json格式的数据
char dataTemplate[] = "{\"id\":123,\"dp\":{\"temp\":[{\"v\":%.2f}],\"humi\":[{\"v\":%.2f}],\"location\":[{\"v\":{\"lon\":%.2f,\"lat\":%.2f}}]}}";

void onenet_connect();
void sendTempAndHumi();
// void onenet_mqtts_connect();
// void onenet_mqtts_sendTemp_Humi_LBS();
/*-------------------------------休眠服务相关al_sleep.ino---------------------*/
void go_sleep();

/*--------------------------------eeprom相关函数--------------------*/
void get_eeprom_firstBootFlag();
void eeprom_config_init();
void eeprom_config_set_sleeptime(time_t time1);
/*********************************SPIFFS相关函数 al_FFS.ino**********/
void alFFS_init();

#endif // CONFIG_H