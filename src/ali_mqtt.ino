#include "config.h"

#define PRODUCT_KEY "a1AYa96sZMJ"                        //产品ID
#define DEVICE_NAME "EspTempAndHumi_D001"                //设备名
#define DEVICE_SECRET "a23249cb179feee41ca2f8f38525113d" //设备key
//鉴权信息
#define mqtt_password "version=2018-10-31&res=products%2F370098%2Fdevices%2Fesp_device001&et=4092512761&method=md5&sign=MUV%2BKFLzv81a4Bw6BDrChQ%3D%3D"

//设备下发命令的set主题
#define ALINK_TOPIC_PROP_SET "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
//设备上传数据的post主题
#define ALINK_TOPIC_PROP_POST "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
//设备post上传数据要用到一个json字符串, 这个是拼接postJson用到的一个字符串
#define ALINK_METHOD_PROP_POST "thing.event.property.post"
//这是post上传数据使用的模板
#define ALINK_BODY_FORMAT "{\"id\":\"%u\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"
//服务器时间同步主题
#define ALINK_TOPIC_NTP_REQ "/ext/ntp/" PRODUCT_KEY "/" DEVICE_NAME "/request"
#define ALINK_TOPIC_NTP_RSP "/ext/ntp/" PRODUCT_KEY "/" DEVICE_NAME "/response"

void ali_mqtt_connect()
{
  Serial.println("connecting to ALI IOT...");
  if (connectAliyunMQTT(mqttClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET))
  {
    Serial.println("MQTT connected!");
  };
  mqttClient.setCallback(callback); //绑定收到set主题时的回调(命令下发回调)
}

void callback(char *topic, byte *payload, unsigned int length)
{
  //如果收到的主题里包含字符串ALINK_TOPIC_PROP_SET(也就是收到"/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"主题)
  if (strstr(topic, ALINK_TOPIC_PROP_SET))
  {
    Serial.println("rev a topic:");
    Serial.println(topic);
    Serial.println("the payload is:");
    payload[length] = '\0'; //为payload添加一个结束附,防止Serial.println()读过了
    Serial.println((char *)payload);
    //接下来是收到的json字符串的解析
    DynamicJsonDocument doc(100);
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.println("parse json failed");
      return;
    }
    JsonObject setAlinkMsgObj = doc.as<JsonObject>();
    serializeJsonPretty(setAlinkMsgObj, Serial);
    Serial.println();
    //接下来就可以解析set里的命令了
  }
  //如果收到的主题里包含字符串ALINK_TOPIC_NTP_RSP(也就是收到"/ext/ntp/" PRODUCT_KEY "/" DEVICE_NAME "/response")
  if (strstr(topic, ALINK_TOPIC_NTP_RSP))
  {
    Serial.println("rev a topic:");
    Serial.println(topic);
    Serial.println("the payload is:");
    payload[length] = '\0'; //为payload添加一个结束附,防止Serial.println()读过了
    Serial.println((char *)payload);

    DynamicJsonDocument timDoc(100);
    DeserializationError error = deserializeJson(timDoc, payload);
    if (error)
    {
      Serial.println("parse timDoc json failed");
      return;
    }
    JsonObject setAlinkMsgObj = timDoc.as<JsonObject>();
    serializeJsonPretty(setAlinkMsgObj, Serial);
    String str = setAlinkMsgObj["serverRecvTime"];
    //这个时间字符串是以毫秒为单位的, 用toInt方法可能会溢出,所以需要裁减
    str = str.substring(0, 10);
    long stamp = str.toInt() + 8 * 60 * 60; //我们在东八区,所以给时间戳加了八个小时的偏移
    time_t timestmap = stamp;
    timeNow = localtime(&timestmap);
    Serial.printf("timeNow: %d-%d-%d %d:%d",
                  timeNow->tm_year + 1900, //由于格林威治时间从1900年开始,所以加1900
                  timeNow->tm_mon + 1,     //格林威治时间月份是0~11,所以＋1
                  timeNow->tm_mday,        //mday的意思是本月第几天
                  timeNow->tm_hour,
                  timeNow->tm_min);
    timeNTPdone = true;
  }
}

/*-------------------------------向云平台发送温湿度数据-------------------------------*/
void sendTemp_Humi_LBS()
{
  if (mqttClient.connected())
  {
    if (!tempAndHumi_Ready)
    {
      Serial.println("failed got TEMP&humi,return");
      return; //如果没有测好温湿度, 就不用上传了
    }
    //先拼接出json字符串
    char param[182];
    char jsonBuf[278];
    sprintf(param, "{\"CurrentHumidity\":%.2f,\"CurrentTemperature\":%.2f,\"GeoLocation\":{\"value\":{\"Longitude\":%.2f,\"Latitude\":%.2f,\"Altitude\":150,\"CoordinateSystem\":2}}}",
            currentHumi,
            currentTemp,
            locationE,
            locationN); //我们把要上传的数据写在param里
    postMsgId += 1;
    sprintf(jsonBuf, ALINK_BODY_FORMAT, postMsgId, ALINK_METHOD_PROP_POST, param);
    //再从mqtt客户端中发布post消息
    if (mqttClient.publish(ALINK_TOPIC_PROP_POST, jsonBuf))
    {
      Serial.print("Post message to cloud: ");
      Serial.println(jsonBuf);
    }
    else
    {
      Serial.println("Publish message to cloud failed!");
    }
    tempAndHumi_Ready = false;
  }
}

/*-------------------------------获取NTP信息-------------------------------------*/
void mqttPublish_ntpTimeRequest()
{
  if (mqttClient.connected())
  {
    //先拼接出json字符串
    char jsonBuf[178] = "{\"deviceSendTime\":\"1571724098000\"}";
    //再从mqtt客户端中发布post消息
    if (mqttClient.publish(ALINK_TOPIC_NTP_REQ, jsonBuf))
    {
      Serial.print("NTP message to cloud: ");
      Serial.println(jsonBuf);
      while (!timeNTPdone)
      {
        mqttClient.loop();
        //做跳出
      }
    }
    else
    {
      Serial.println("Publish NTP message to cloud failed!");
    }
  }
}
