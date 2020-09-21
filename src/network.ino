#include "config.h"
/*-------------------------------APN相关定义-------------------------------------*/
const char apn[] = "CMNET"; // Your APN
const char gprsUser[] = ""; // User
const char gprsPass[] = ""; // Password
const char simPIN[] = "";   // SIM card PIN code, if any

/*-------------------------------初始化SIM800L-------------------------------------*/
void setupModem()
{
  pinMode(MODEM_POWER_ON, OUTPUT); //电源引脚
  pinMode(MODEM_PWRKEY, OUTPUT);   //开关机键引脚

  // 先打开SIM800L的电源
  digitalWrite(MODEM_POWER_ON, HIGH);
  //根据手册要求拉下PWRKEY 1秒钟以上 可以开机
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, HIGH);
  SerialMon.println("Initializing modem...");
  modem.init(); //开机后modem初始化一下
}
/*-------------------------------SIM800L连接GPRS-------------------------------------*/
void modemToGPRS()
{
  //连接网络
  SerialMon.print("Waiting for network...");
  while (!modem.waitForNetwork(240000L))
  {
    SerialMon.print(".");
    delay(500);
  }
  SerialMon.println(" OK");
  //连接GPRS接入点
  SerialMon.print(F("Connecting to APN: "));
  SerialMon.print(apn);
  while (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    SerialMon.print(".");
    delay(10000);
  }
  SerialMon.println(" OK");
}
/*-------------------------------获取位置信息-------------------------------------*/
void getLBSLocation()
{
  Serial.println("getting LBS...");
  String locationStr, locationStrX, locationStrY;
  locationStr = modem.getGsmLocation();
  if (locationStr.length() > 15)
  {
    int finddou;
    finddou = locationStr.indexOf(',');
    locationStrX = locationStr.substring(0, finddou);
    locationStrY = locationStr.substring(finddou + 1, locationStr.length());
    Serial.println(locationStr);
    if (locationStrX.toFloat() > 1)
    {
      locationE = locationStrX.toFloat();
      locationN = locationStrY.toFloat();
    }
  }
}



