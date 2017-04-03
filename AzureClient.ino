#include <aJSON.h>

#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <NTPClient.h>

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>


#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include "Device.h"
#include "IoTHub.h"
//#include "EventHub.h"  // uncomment if you plan to publish to Azure Event Bus
#include "Sensor.h"

//#include "DhtSensor.h"
#include "DigitalPin.h"

void sendToIoTHub();

void getCurrentTime();
time_t getNtpTime();


const char* connectionString = "HostName=nucleonix-iot-hub.azure-devices.net;DeviceId=NodeMCUNUCLEONIX;SharedAccessKey=K5xNG+asPQHksoDtP6ju/1+p/62yqrPdBKpFnWN0ElE=";
const char* ssid = "IoTLab";
const char* pwd = "snistiot123";
//const char* ssid = "Varun";
//const char* pwd = "77777777";
const char* geo = "mlb-garage";
BoardType boardType = NodeMCU; // BoardType enumeration: NodeMCU, WeMos, SparkfunThing, Other (defaults to Other).
/* 
 http://hassansin.github.io/certificate-pinning-in-nodejs
 for information on generating fingerprint
 From Ubuntu subsystem on Windows 10
 echo -n | openssl s_client -connect IoTCampAU.azure-devices.net:443 | sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' > cert.pem
 openssl x509 -noout -in cert.pem -fingerprint
*/
const char* certificateFingerprint = "38:5C:47:B1:97:DA:34:57:BB:DD:E7:7C:B9:11:8F:8D:1D:92:EB:F1";

Device device(ssid, pwd);
IoT hub;

ESP8266WiFiMulti WiFiMulti;



Sensor sensor;  // Fake sample environmental data

//DhtSensor sensor(device, dht11);



DigitalPin led(BUILTIN_LED); 

IPAddress timeServer(62, 237, 86, 238); // Update these with values suitable for your network.

void initDeviceConfig() { // Example device configuration
  device.boardType = boardType;            // BoardType enumeration: NodeMCU, WeMos, SparkfunThing, Other (defaults to Other). This determines pin number of the onboard LED for publish status. Other means no LED status 
  device.deepSleepSeconds = 0;         // if greater than zero with call ESP8266 deep sleep (default is 0 disabled). GPIO16 needs to be tied to RST to wake from deepSleep. Causes a reset, execution restarts from beginning of sketch
  device.publishRateInSeconds = 20;     // limits publishing rate to specified seconds (default is 90 seconds).  Connectivity problems may result if number too small eg 2
  
  hub.sasExpiryPeriodInSeconds = 15 * 60; // Renew Sas Token every 15 minutes
  hub.certificateFingerprint = certificateFingerprint;
  hub.setConnectionString(connectionString);

 // sensor.geo = geo;
}
String gMode;
String gPartitionKey;
String gRowKey;
String gCalibrationAlertIntervalInMonths;
String gDevicePollingTimeInSeconds;
String gUpgradeAlertIntervalInYears;
int gpolltime = 0;


// String colibriData;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("");

  initDeviceConfig();
  device.connectWifi();
  getCurrentTime();
  
   if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        http.begin("http://nucleonixstorage.table.core.windows.net/inputtable(PartitionKey='chan',RowKey='varun')?sv=2016-05-31&ss=t&srt=sco&sp=rwdlacu&spr=https,http&sig=0%2f9UR%2fzf2bHqyOKKH1YfUhNBY86CxstCfP5TTKxmNPE%3d&se=2117-03-07T12:38:16Z&st=2017-03-07T04:38:16Z");
        http.addHeader("Accept", "application/json;odata=nometadata");
       
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
                                 // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
                char a[500];
               payload.toCharArray(a,500);
                aJsonObject* jsonObject = aJson.parse(a);

                 aJsonObject* DevicePollingTimeInSeconds = aJson.getObjectItem(jsonObject, "DevicePollingTimeInSeconds");
                 gDevicePollingTimeInSeconds=DevicePollingTimeInSeconds->valuestring;

                // Serial.println(gDevicePollingTimeInSeconds);
            
                gpolltime = gDevicePollingTimeInSeconds.toInt();


                
                   
             
            }

            else
           {
          gpolltime=60;
          Serial.println("Failed to get polling value and hence running using default polling value.");
          }
      }
else
{
   gpolltime=60;
   Serial.println("Failed to get polling value and hence running using default polling value.");
  }
     

}
else
{
   gpolltime=60;
  Serial.println("Failed to get polling value and hence running using default polling value.");
  }
Alarm.timerRepeat(gpolltime, sendToIoTHub);

} // end of setup





void loop() {
 // sensor.measure();
                     


  device.connectWifi();

    // Alarm.delay(1);
  led.off();
  delay(100);
   if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        http.begin("http://nucleonixstorage.table.core.windows.net/inputtable(PartitionKey='chan',RowKey='varun')?sv=2016-05-31&ss=t&srt=sco&sp=rwdlacu&spr=https,http&sig=0%2f9UR%2fzf2bHqyOKKH1YfUhNBY86CxstCfP5TTKxmNPE%3d&se=2117-03-07T12:38:16Z&st=2017-03-07T04:38:16Z");
        http.addHeader("Accept", "application/json;odata=nometadata");
       
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
                                 // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
                char a[500];
               payload.toCharArray(a,500);
                aJsonObject* jsonObject = aJson.parse(a);
                aJsonObject* Mode = aJson.getObjectItem(jsonObject, "MODE");
                gMode=Mode->valuestring;
                aJsonObject* PartitionKey = aJson.getObjectItem(jsonObject, "PartitionKey");
                 gPartitionKey=PartitionKey->valuestring;
                aJsonObject* RowKey = aJson.getObjectItem(jsonObject, "RowKey");
                 gRowKey=RowKey->valuestring;
                 aJsonObject* CalibrationAlertIntervalInMonths = aJson.getObjectItem(jsonObject, "CalibrationAlertIntervalInMonths");
                 gCalibrationAlertIntervalInMonths=CalibrationAlertIntervalInMonths->valuestring;
                                 aJsonObject* DevicePollingTimeInSeconds = aJson.getObjectItem(jsonObject, "DevicePollingTimeInSeconds");
                 gDevicePollingTimeInSeconds=DevicePollingTimeInSeconds->valuestring;
                                    aJsonObject*  UpgradeAlertIntervalInYears = aJson.getObjectItem(jsonObject, "UpgradeAlertIntervalInYears");
                 gUpgradeAlertIntervalInYears=UpgradeAlertIntervalInYears->valuestring;

                 Serial.println(gMode);
                 Serial.println(gPartitionKey);
                 Serial.println(gRowKey);
                 Serial.println(gCalibrationAlertIntervalInMonths);
                 Serial.println(gDevicePollingTimeInSeconds);
                 Serial.println( gUpgradeAlertIntervalInYears);


                
            }
        } 
        
        else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
   }


   

      
     if(gpolltime != gDevicePollingTimeInSeconds.toInt())
     {
          gpolltime = gDevicePollingTimeInSeconds.toInt();
           Alarm.timerRepeat(gDevicePollingTimeInSeconds.toInt(), sendToIoTHub);
          ESP.restart();
     }

     
   
   

    Alarm.delay(0);
  //checks if alarm needs to be triggered every 0 seconds or almost continuously
  led.on();

  if (device.deepSleepSeconds > 0) {
    WiFi.mode(WIFI_OFF);
    ESP.deepSleep(1000000 * device.deepSleepSeconds, WAKE_RF_DEFAULT); // GPIO16 needs to be tied to RST to wake from deepSleep. Execute restarts from beginning of sketch
  }
  else {
    delay(device.publishRateInSeconds * 1000);  // limit publishing rate
  }

}
void getCurrentTime(){
  int ntpRetryCount = 0;
  while (timeStatus() == timeNotSet && ++ntpRetryCount < 20) { // get NTP time
    Serial.println(WiFi.localIP());
    setSyncProvider(getNtpTime);
    setSyncInterval(60 * 60);  
  }
}

void sendToIoTHub()
{


     if(gMode.toInt() == 1)
      {

         Serial.println("In mode 1 and hence sending any data according to poll time");
          Serial.println(hub.send(sensor.toJSON()));
      }
  else if(gMode.toInt() == 2)
  {
    
   // if()
     Serial.println("In mode 2");

     Serial.println("Enter Serial data");
         String colibriData = Serial.readString();
        Serial.println(colibriData);
        delay(5000);
        char a[700];
               colibriData.toCharArray(a,700);
                aJsonObject* jsonObject = aJson.parse(a);
                String galarmStatus;
                   aJsonObject* alarmStatus = aJson.getObjectItem(jsonObject, "alarmStatus");
                 galarmStatus=alarmStatus->valuestring;

                 if(galarmStatus=="true")
                 {
                  Serial.println("Alarm status is true and hence sending data");
                  Serial.println(hub.send(sensor.toJSON()));
                  }

                  else
                  {
                  Serial.println("Alarm status is false and hence  not sending data");
                  }
                  


     
    }

    else if(gMode.toInt() == 3)
    {
       // Do nothing
       Serial.println("In mode 3 and hence not sending any data");
    }
    else
    {
       Serial.println("In default mode");

       Serial.println(hub.send(sensor.toJSON()));
      }

  // response 204 means successful send of data to Azure IoT Hub

}



