// change next line to use with another board/shield
#include <WiFi.h>
//#include <ESP8266WiFi.h>
//#include <WiFi.h>
//#include <WiFi101.h>
#include <WiFiUdp.h>
#include "NTPClient.h"

WiFiUDP ntpUDP;
NTPClient my_time_client(ntpUDP);

void connect_wifi(char * ssid,char * password){
    Serial.print("wifi connecting...");
    WiFi.begin(ssid,password);
    while(WiFi.status()!= WL_CONNECTED){
        delay(1000);
        Serial.print(".");
    }
    
    Serial.println("\nwifi connected!");
    Serial.print("ip: ");
    Serial.println(WiFi.localIP());
    Serial.print("netmask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("channel: ");
    Serial.println(WiFi.channel());
    Serial.print("auto-reconnect: ");
    Serial.println(WiFi.getAutoReconnect());
}

inline void sync_time(){
    my_time_client.begin();
    my_time_client.setTimeOffset(28800);
    my_time_client.setUpdateInterval(20000);
    //my_time_client.setPoolServerName("0.ubuntu.pool.ntp.org");
    my_time_client.setPoolServerName("192.168.1.200");
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
    Serial.begin(230400);
    Serial.println("serial inited");

    connect_wifi((char *)"ssid",(char *)"pwd");
    sync_time();

    Serial.println("ready!");
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

    //float ms=my_time_client.get_millis();
    
    delay(1);
}