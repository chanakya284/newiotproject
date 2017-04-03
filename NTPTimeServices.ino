#include <WiFiUdp.h>
void sendNTPpacket(IPAddress &address);
time_t getNtpTime();
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
const int timeZone = 5; //VR: I changed it from 0 to 5 to make it to our indian time zone and then manually added 30 mins in the formula below for accuracy
char isoTime[30];

char* GetISODateTime() {
  sprintf(isoTime, "%4d-%02d-%02dT%02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
  return isoTime;
}

const int NTP_PACKETSIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKETSIZE]; //buffer to hold incoming & outgoing packets


time_t getNtpTime()
{
  Udp.begin(localPort);
  
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKETSIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKETSIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];

      Udp.stop();
      
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR + (SECS_PER_HOUR/2) ; //VR:I had to add halfan hour more seconds because the offset is only int so 5hrs as int and 30mins manually 
    
    }
  }

  Udp.stop();
  
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKETSIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKETSIZE);
  Udp.endPacket();
}
