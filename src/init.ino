/*----------------------------------------------------------
                 硬件初始化操作相关
*---------------------------------------------------------*/
#include "config.h"

void hardware_init()
{
  Wire.begin();
  SerialMon.begin(115200);                                //初始化调试串口
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX); //初始化AT串口
  sht20.begin();
  EEPROM.begin(4096);
}

void software_init()
{
  tempAndHumi_Ready = false;
  measureing_flag = 1;
  reduce_sleeptime = 0;
}