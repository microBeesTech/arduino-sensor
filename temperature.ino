#include <SPI.h>
#include <Ethernet.h>
#define MQTT_VERSION 3
#define MQTT_MAX_PACKET_SIZE 200
#include <PubSubClient.h>

//DON'T FORGET TO CHANGE YOUR GATEBEE IP 
char * gateBeeIP = "x.x.x.x";
IPAddress server(x,x,x,x);

byte mac[]  = {0xA8,0x17,0xFB,0xBB,0x46,0x55};
//DON'T FORGET TO CHANGE YOUR GATEBEE SSERIAL
char * gateSerial ="B8-27-EB-83-E2-00";

char * beeSerial="YOUR-UID";

EthernetClient ethClient;
PubSubClient client(gateBeeIP, 1883, 0, ethClient);

unsigned long lastMillis = 0;

void setup()
{
  Serial.begin(57600);
  /*COMMENT THIS LOOP IF YOU WANT TO SET MANUALLY YOUR IP AND PUT:
  Ethernet.begin(mac,ip);
  */
  while(Ethernet.begin(mac)==0){
  };
  Serial.println("CONNECTED TO LAN");
  client.setServer(server, 1883);
  analogReference(INTERNAL);
  pinMode(A0,INPUT); 
}
String buildJson(){
  float temp =  0;
  for (byte i = 0; i < 5; i++) { 
    temp += (analogRead(A0) / 9.31); 
  }
  temp /= 5; 
  String json= "{\"product_id\":20,";
  json+="\"conf\":{\"smartbee_serial\":\"";
  json+=beeSerial;
  json+="\",\"smartgate_serial\":\"";
  json+=gateSerial;
  json+="\"},";
  json+="\"data\":{\"param_type\":5,\"param_num\":1,\"payload\":[";
  json+=temp;
  json+="]}}";
  return json;
} 
long lastUpdate=0;
void loop(){
   client.loop();
   if (!client.connected()) {
    Serial.print("Trying to connect to: ");
    Serial.println(gateBeeIP);
    client.connect("temperature") ;
   }
   if (client.connected()  && millis()-lastUpdate>5000) {
      lastUpdate=millis();
      Serial.println("CONNECTED TO gateBee");
      String json = buildJson();
      char  jsonStr [200];
      json.toCharArray(jsonStr,200);
      boolean pubresult = client.publish("sensor_in",jsonStr);
      Serial.println(json);
      if (pubresult)
        Serial.println("successfully sent");
      else{
        Serial.println("unsuccessfully sent");
      }
   } else if(!client.connected()){
      Serial.println("ERROR DURING CONNECTION WITH gateBee");
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(1000);
   }
}
