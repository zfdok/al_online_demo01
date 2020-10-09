#include "config.h"

/*-------------------------------连接平台-------------------------------*/
void onenet_connect()
{
  //连接OneNet并上传数据
  Serial.println("connecting to OneNet IOT...");
  client.setServer(mqtt_server, port);                   //设置客户端连接的服务器,连接Onenet服务器, 使用6002端口
  client.connect(mqtt_devid, mqtt_pubid, mqtt_password); //客户端连接到指定的产品的指定设备.同时输入鉴权信息
}
/*-------------------------------向云平台发送温湿度数据-------------------------------*/
void sendTempAndHumi()
{
  if (client.connected())
  {
    snprintf(msgJson, 256, dataTemplate, currentTemp, currentHumi, locationE, locationN); //将模拟温湿度数据套入dataTemplate模板中, 生成的字符串传给msgJson
    Serial.print("public the data:");
    Serial.println(msgJson);
    char publicTopic[75] ; 
    char topicTemplate[] = "$sys/%s/%s/dp/post/json"; //信息模板
    snprintf(publicTopic,75,topicTemplate,mqtt_pubid,mqtt_devid);
    Serial.println("publicTopic");
    Serial.println(publicTopic);
    client.publish(publicTopic, (uint8_t *)msgJson, strlen(msgJson));
    //发送数据到主题
  }
}
