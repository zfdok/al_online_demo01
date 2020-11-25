/*----------------------------------------------------------
                 硬件初始化操作相关
*---------------------------------------------------------*/
#include "config.h"

void hardware_init()
{
  Wire.begin();
  SerialMon.begin(115200); //初始化调试串口
  Serial.println("wakeup");
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX); //初始化AT串口
  sht20.begin();
  EEPROM.begin(4096);
  SPIFFS.begin();
  display.init();
  display.flipScreenVertically();
  key_init();
}
/*----------------------------------------------------------
                 软件初始化操作相关
*---------------------------------------------------------*/
void software_init()
{
  tempAndHumi_Ready = false;
  Serial.printf("workingState:%d\r\n", workingState);
  Serial.printf("oledState:%d\r\n", oledState);
  loopStartTime = millis();
  screen_loopEnabled = true;
  show_tip_screen_last = 2500;
  show_BLE_screen_last = 8000;
  show_rec_stop_screen_last = 2000;
  screen_On_last_span = 10000;
  screen_Off_to_sleep_span = 3000;
  screen_On_Start = millis();
  screen_On_now = millis();
  Serial.print("esp_sleep_get_wakeup_cause()");
  Serial.println(esp_sleep_get_wakeup_cause());
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) //如果是系统复位唤醒的, 则停止工作, 亮屏
  {
    workingState = NOT_WORKING;
    oledState = OLED_ON;
  } 
  keyState = NOKEYDOWN;
  screenState = MAIN_TEMP_SCREEN;
  bleState = BLE_OFF;
  lockState = UNLOCKED;
  qualifiedState = QUALITIFY_RIGHT;
  wakeup_init_time();
}