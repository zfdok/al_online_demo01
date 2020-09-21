/*----------------------------------------------------------
*                 sht20操作相关
*
*---------------------------------------------------------*/
#include "config.h"
extern float currentTemp;
extern float currentHumi;
extern bool tempAndHumi_Ready;
extern int postMsgId;
void sht20getTempAndHumi()
{
  float _currentTemp = sht20.temperature();
  float _currentHumi = sht20.humidity();

  if (_currentTemp < 100)
  {
    currentTemp = _currentTemp;
    currentHumi = _currentHumi;
    tempAndHumi_Ready = true;
    postMsgId++;
  }
}