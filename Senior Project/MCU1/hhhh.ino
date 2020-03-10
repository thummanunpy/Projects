
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <TridentTD_LineNotify.h>
#include <time.h>
#include <SoftwareSerial.h>
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//เชื่อต่อ Wifi

    char ssid1[] = "AofECS"; 
    char pass1[] = "AAAaaa52";

// Config MQTT Server
    #define mqtt_server "tailor.cloudmqtt.com"
    #define mqtt_port  16253
    #define mqtt_user "ibpdvqkv"
    #define mqtt_password "rlFXvQvk-AB0"

// Config ToKen line  
    String LINE_TOKEN = "qHnz9dkx5bWWC73oHGiBkqki8dWz72mBypGvza0VBeb";
    
// กำหนดค่าเวลา
    char ntp_server1[20] = "pool.ntp.org";
    char ntp_server2[20] = "time.nist.gov";
    char ntp_server3[20] = "time.uni.net.th";
    int timezone = 7 * 3600;
    int dst = 0;
    int time1=0;
    int time2=34;
    int time3;

//กำ ouput ให้หลอดไฟ

    String a;
    int i;
    int j=0;
  
  
//กำหนด client
    WiFiClient espClient;
    PubSubClient client(espClient);
    SoftwareSerial chat(D2,D3); // RX, TX
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
void setup() {
// กำหนด Line Token
      LINE.setToken(LINE_TOKEN);
//config time
      configTime(timezone, dst, ntp_server1, ntp_server2, ntp_server3);
// Set output 
     pinMode(D2, INPUT);
     pinMode(D3, OUTPUT);
     pinMode(D4, OUTPUT);
     pinMode(D5, OUTPUT);
     pinMode(D6, OUTPUT);
     Serial.begin(115200);
     chat.begin(4800);
// เชื่อต่อไวไฟ

      WiFi.begin(ssid1, pass1);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        }
       Serial.println("");
       Serial.println("WiFi connected");
       Serial.println("IP address: ");
       Serial.println(WiFi.localIP());
       client.setServer(mqtt_server, mqtt_port);
       client.setCallback(callback);
       }

       
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
       
void loop() {
//เชื่อต่อ MQTT
   if (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
        Serial.println("connected");
        } else 
        {
           Serial.print("failed, rc=");
           Serial.print(client.state());
           Serial.println(" try again in 5 seconds");
           delay(5000);
           j=0;
           return;
              }       
   } else {
    client.subscribe("/led");
    if(j==0){
    LINE.notify("อุปกรณ์ตัวที่1ของท่านเชื่อต่ออินเตอร์เน็ตแล้ว");
    j=1;
    }
          }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//กำค่าให้เวลาเปิดปิด
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    if(time1>0){
    if(p_tm->tm_hour == 6 && p_tm->tm_min == 0 && p_tm->tm_sec == 0 )
     {
       time1++;
       LINE.notify("ต้นพืชของท้านมีอายุ");
       LINE.notify("สามารถเก็บเกี่ยวได้");
     }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//รับคำมาจาก Arduino เพื่อส่งคำให้ Line
      a=chat.readString();
      if(a== "Sendplant")
    { 
        LINE.notify("กำหนดหลุมปลูกแล้ว");
    }
      else if(a=="Start"){
        LINE.notify("เริ่มต้นการปลูกต้นพืช");
      }
      else if(a=="Start2"){
        LINE.notify("เริ่มต้นการเก็บต้นพืช");
      }
      else if(a=="Start3"){
        LINE.notify("เริ่มต้นการถ่ายต้นพืช");
      }
      else if(a=="Finish"){
        digitalWrite(D4,LOW);
        LINE.notify("การดำเนินการทำงานเสร็จสิ้น");
      }
       else if(a=="takephoto"){
        client.publish("/led", "takeon"); 
      }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        client.loop();
      }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------


void callback(char* topic, byte* payload, unsigned int length) {
      Serial.print("Message arrived [");
      Serial.print(topic);
      Serial.print("] "); 
      String msg = "";
      int i = 0;
//เลือกคำทีส่งมา
 while (i < length) msg += (char)payload[i++];

      Serial.println(msg);
//เปิดไฟ
      if(msg == "on")
      {
      digitalWrite(D6,HIGH);
      }
//ปิดไฟ
      else if(msg == "off")
      {
     digitalWrite(D6,LOW);
      }
//เปิดปั้ม
      else if(msg == "pumpon")
      {
     digitalWrite(D5,HIGH);
      }
//ปิดปั้ม
     else if(msg =="pumpoff")
      {
     digitalWrite(D5,LOW);
      }

  }


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
