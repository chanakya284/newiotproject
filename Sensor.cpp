#include "Sensor.h"
#include <aJSON.h>

/*
String firstValue;
String secondValue;
String thirdValue;
String fourthValue;
String fifthValue;
String sixthValue;
String seventhValue;
String eightValue;
String ninthValue;
String tenthValue;
*/

    String gstrModel;
 String gstrSerialNum;
String  gstrMfgDt;
String  gstrCps;
String  gstrExposureRate;
String  gstrUnit;
String  gstrHV;
 String gstrLV;
 String gstrDetStatus;
 String gstrLastCalibDate;
 String galarmStatus;


void Sensor::measure(){
  temperature = 25;
  humidity = 50;
  pressure = 1000;
}

char* Sensor::toJSON() {
  Serial.println("Enter serial data now");

 String colibriData = Serial.readString();
 Serial.println(colibriData);
 delay(5000);
   char a[700];
               colibriData.toCharArray(a,700);
                aJsonObject* jsonObject = aJson.parse(a);

                 aJsonObject* strModel = aJson.getObjectItem(jsonObject, "strModel");
                 gstrModel=strModel->valuestring;

                 aJsonObject* strSerialNum = aJson.getObjectItem(jsonObject, "strSerialNum");
                 gstrSerialNum=strSerialNum->valuestring;

                  aJsonObject* strMfgDt = aJson.getObjectItem(jsonObject, "strMfgDt");
                 gstrMfgDt=strMfgDt->valuestring;

                    aJsonObject* strCps = aJson.getObjectItem(jsonObject, "strCps");
                gstrCps=strCps->valuestring;
                 
                  
                   aJsonObject* strExposureRate = aJson.getObjectItem(jsonObject, "strExposureRate");
                 gstrExposureRate=strExposureRate->valuestring;
                 
                    aJsonObject* strUnit = aJson.getObjectItem(jsonObject, "strUnit");
                 gstrUnit=strUnit->valuestring;

                   aJsonObject* strHV = aJson.getObjectItem(jsonObject, "strHV");
                 gstrHV=strHV->valuestring;

                   aJsonObject* strLV = aJson.getObjectItem(jsonObject, "strLV");
                 gstrLV=strLV->valuestring;

                   aJsonObject* strDetStatus = aJson.getObjectItem(jsonObject, "strDetStatus");
                 gstrDetStatus=strDetStatus->valuestring;

                   aJsonObject* strLastCalibDate = aJson.getObjectItem(jsonObject, "strLastCalibDate");
                 gstrLastCalibDate=strLastCalibDate->valuestring;

                   aJsonObject* alarmStatus = aJson.getObjectItem(jsonObject, "alarmStatus");
                 galarmStatus=alarmStatus->valuestring;

               Serial.println(gstrModel);
                Serial.println(gstrMfgDt);
                Serial.println(gstrCps);
                Serial.println(gstrExposureRate);
                Serial.println(gstrUnit);
                Serial.println(gstrHV);
                Serial.println(gstrLV);
                Serial.println(gstrDetStatus);
                Serial.println(gstrLastCalibDate);
                   Serial.println(galarmStatus);

   

                 
                 
             
  StaticJsonBuffer<JSON_OBJECT_SIZE(20)> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();


   root["strModel"]=gstrModel;
    root["strSerialNum"]=gstrSerialNum;
     root["strMfgDt"]=gstrMfgDt;
      root["strCps"]=gstrCps;
            root["strExposureRate"]=gstrExposureRate;
             root["strUnit"]=  gstrUnit;
             root["strHV"]=  gstrHV;
             root["strLV"]=  gstrLV;
              root["strDetStatus"]=  gstrDetStatus;
             root["strLastCalibDate"]= gstrLastCalibDate;
           root["alarmStatus"]=  galarmStatus;
                               
 
 

  



 

 

  root.printTo(buffer, sizeof(buffer));
 Serial.println(buffer);
  return buffer;
}



char* Sensor::getISODateTime() {
  sprintf(isoTime, "%4d-%02d-%02dT%02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
  return isoTime;
}


