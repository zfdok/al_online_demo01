#include "config.h"
int rollback = 0;

//第二核创建任务代码
void codeForTask1(void *parameter)
{
  while (1) //这是核1 的loop
  {
    delay(10);
    button.tick(); //扫描按键
  }
}

void setup()
{
  hardware_init(); //硬件初始化
  software_init(); //软件初始化
  if (oledState == OLED_ON)
    showWelcome();
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
    //3. 将任务句柄和任务代码传与某个核连接起来, 开始执行
    xTaskCreatePinnedToCore(codeForTask1, //任务函数名
                            "task1",      //用户起的任务名
                            1000,         //栈区分配大小
                            NULL,         //创建任务时传入的参数
                            1,            //优先级
                            &task1,       //用于传回创建任务的句柄,
                            0);           //用哪个核执行
    get_eeprom_firstBootFlag();           //获取EEPROM第1位,判断是否是初次开机
    alFFS_init();                         //初始化FFS
    eeprom_config_init();                 //初始化EEPROM
    if (oledState == OLED_OFF)
    {
      if (workingState == WORKING && (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER))
      {
        send_Msg_var_GSM_while_OLED_off();
        delay(1000);
        go_sleep_a_while_with_ext0();
      }
    }
  }
}

void loop()
{
  waking_update_time();
  if (oledState == OLED_ON)
  {
    sht20getTempAndHumi();
    screen_loop(); //展示和滚屏
    key_loop();
    screen_show(); //OLED最终显示
    send_Msg_var_GSM_while_OLED_on();
  }
  oled_on_off_switch();
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
    char subscribeTopic[75];
    char topicTemplate[] = "$sys/%s/%s/cmd/request/#"; //信息模板
    snprintf(subscribeTopic, 75, topicTemplate, mqtt_pubid, mqtt_devid);
    client.subscribe(subscribeTopic); //订阅命令下发主题
    sendTempAndHumi();
  }
  alFFS_addRec();
  alFFS_readRecing();
  reduce_sleeptime = 0;
}

void send_Msg_var_GSM_while_OLED_on()
{
  if (workingState == WORKING)
  {
    now_rec_stamp = millis();
    if (now_rec_stamp - last_rec_stamp > sleeptime / 1000)
    {
      screen_loopEnabled = false;
      key_attach_null();
      //上传
      display.clear();
      display.setFont(Roboto_Condensed_12);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 5, "Initializing modem...");
      display.drawProgressBar(5, 50, 118, 8, 5);
      display.display();
      setupModem(); //SIM800L物理开机

      display.clear();
      display.drawString(64, 5, "Waiting for network...");
      display.drawProgressBar(5, 50, 118, 8, 40);
      display.display();
      modemToGPRS(); //modem连接GPRS

      display.clear();
      display.drawString(64, 5, "getting LBS...");
      display.drawProgressBar(5, 50, 118, 8, 70);
      display.display();
      getLBSLocation(); //获取定位信息

      display.clear();
      display.drawString(64, 5, "connecting to OneNet");
      display.drawProgressBar(5, 50, 118, 8, 90);
      display.display();
      sht20getTempAndHumi(); //获取温湿度数据
      onenet_connect();

      display.clear();
      display.drawString(64, 5, "uploading...");
      if (client.connected())
      {
        char subscribeTopic[75];
        char topicTemplate[] = "$sys/%s/%s/cmd/request/#"; //信息模板
        snprintf(subscribeTopic, 75, topicTemplate, mqtt_pubid, mqtt_devid);
        client.subscribe(subscribeTopic); //订阅命令下发主题
        sendTempAndHumi();
      }
      display.drawProgressBar(5, 50, 118, 8, 100);
      display.display();
      delay(200);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      key_init();
      last_rec_stamp = millis();
      screen_loopEnabled = true;
      screen_On_Start = millis();
      screen_On_now = millis();
      alFFS_addRec();
      alFFS_readRecing();
      reduce_sleeptime = 0;
    }
  }
}
