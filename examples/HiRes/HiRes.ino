#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h>
//#include <WiFi101.h>
#include <WiFiUdp.h>

const char *ssid     = "<SSID>";
const char *password = "<PASSWORD>";

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionally you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient my_time_client(ntpUDP, "europe.pool.ntp.org", 28800, 20000);

inline void sync_time(){
    my_time_client.begin();
    //smaller timeout will give you more accuracy
    //but also larger possibility to fail
    my_time_client.setTimeout(800);
    Serial.println("syncing...");

    while(my_time_client.update()!=1){
        delay(2000);
        my_time_client.forceUpdate();
    }

    Serial.print("success: ");
    Serial.println(my_time_client.getFormattedTime());
}

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  sync_time();
}

String s_last_time="s_last_time";

void loop(){
    String s_time=my_time_client.getFormattedTime();
    if(s_time!=s_last_time){
        Serial.print("a second passed ");
        Serial.print(s_time);Serial.print(" ");
        Serial.print(my_time_client.get_millis(),3);
        Serial.println("ms");
        s_last_time=s_time;

        //please do not update too frequently
        int8_t re=my_time_client.update();
        if(re==0){
            Serial.println("0: sync but failed");
            delay(500);
        }else if(re==1){
            Serial.println("1: sync and suc");
        }else if(re==2){
            ;//Serial.println("2: not time to sync");
        }else if(re==3){
            Serial.println("3: last failed was just happen");
        }else{
            Serial.print("return value error: ");
            Serial.println(re);
        }
    }

    delay(1);
}
