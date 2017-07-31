// UDP communication class
#include <string.h>

#define _UDP_COMM_C_
#include "UdpComm.h"

// default port number
#define DEFAULT_LOCAL_PORT  0xC000
#define DEFAULT_REMOTE_PORT 0xC001

// invalic IP address
static const IPAddress NULL_ADDR(0,0,0,0);

// constructor
UdpComm_t::UdpComm_t()
{
    this->localPort = DEFAULT_LOCAL_PORT;
    this->remotePort = DEFAULT_REMOTE_PORT;
    
    onReceive = NULL;
    remoteIP = NULL_ADDR;
}

// set port numbers
void UdpComm_t::setPort(int localPort, int remotePort)
{
    this->localPort = localPort;
    this->remotePort = remotePort;
}

// begin AP mode
void UdpComm_t::beginAP(char* ssid, char* password)
{
    mode = UDP_COMM_MODE_AP;
    
    if(ssid == NULL){
        sprintf(this->mySSID, "gpduino-%06x", ESP.getChipId());
    }else{
        strncpy(mySSID, ssid, sizeof(mySSID)-1);
        mySSID[sizeof(mySSID)-1] = '\0';
    }
    
    strncpy(myPassword, password, sizeof(myPassword)-1);
    mySSID[sizeof(myPassword)-1] = '\0';
    
    // setup AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(mySSID, myPassword);
    localIP = WiFi.softAPIP();
    Serial.println();
    Serial.print("AP SSID: ");Serial.println(mySSID);
    Serial.print("AP IP address: ");Serial.println(localIP);
    remoteIP = NULL_ADDR;

    // begin UDP
    udp.begin(localPort);
}

// begin STA mode
void UdpComm_t::beginSTA(char* ssid, char* password, char* hostName)
{
    mode = UDP_COMM_MODE_STA;
    isConnected = false;
    
    strncpy(hisSSID, ssid, sizeof(hisSSID)-1);
    hisSSID[sizeof(hisSSID)-1] = '\0';
    
    strncpy(hisPassword, password, sizeof(hisPassword)-1);
    hisPassword[sizeof(hisPassword)-1] = '\0';
    
    if(hostName == NULL){
        sprintf(this->hostName, "esp8266-%06x", ESP.getChipId());
    }else{
        strncpy(this->hostName, hostName, sizeof(this->hostName)-1);
        this->hostName[sizeof(this->hostName)-1] = '\0';
    }
    //Serial.print("HostName: ");  Serial.println(hostName);

    // setup STA
    WiFi.mode(WIFI_STA);
    WiFi.begin(hisSSID, hisPassword);
}

// cyclic procedure. It must be called from main loop()
void UdpComm_t::loop()
{
    if(mode == UDP_COMM_MODE_AP)
    {
        loopAP();
    }else{
        loopSTA();
    }
    // onReceive(buff);
}

// cyclic procedure (AP mode)
void UdpComm_t::loopAP()
{
    // when a packet has beeen received
    int packetSize = udp.parsePacket();
    if (packetSize) {
        if(packetSize > (UDP_COMM_BUFF_SIZE-1)){
            packetSize = UDP_COMM_BUFF_SIZE-1;
        }
        int len = udp.read(buff, packetSize);
        if (len > 0) buff[len] = '\0';
        udp.flush();

        Serial.print("loopAP:");Serial.println(buff);
        
        // get IP address of remote host
        remoteIP = udp.remoteIP();
        
        // callback
        if(onReceive != NULL) onReceive(buff);
        
        //Serial.print(remoteIP);
        //Serial.print(" / ");
        //Serial.println(packetBuffer);
    }
}

// cyclic procedure (STA mode)
void UdpComm_t::loopSTA()
{
    // wait for connecting to AP
    if (WiFi.status() != WL_CONNECTED) {
      if(isConnected){
        isConnected = false;
        // stop UDP
        udp.stop();
      }
      //delay(500);
      Serial.print(".");
      return;
    }
    
    // when connected to AP
    if(!isConnected){
      isConnected = true;
      
      // get my own IP address
      localIP = WiFi.localIP();
      Serial.println();
      Serial.print("Connected to "); Serial.println(hisSSID);
      Serial.print("STA IP address: "); Serial.println(localIP);
      remoteIP = NULL_ADDR;
      
      // begin UDP
      udp.begin(localPort);
      if ( mdns.begin ( hostName, localIP ) ) {
        Serial.println ( "MDNS responder started" );
      }else{
        Serial.println("Error setting up MDNS responder!");
      }
    }
    
    // update MDNS
    mdns.update();
    
    // do as same as AP mode
    loopAP();

    //localIP = WiFi.localIP();
    //Serial.println();
    //Serial.print("AP SSID: ");Serial.println(mySSID);
    //Serial.print("AP IP address: ");Serial.println(localIP);
}

// send data
void UdpComm_t::send(char* data)
{
    if(remoteIP != NULL_ADDR){
        udp.beginPacket(remoteIP, remotePort);
        udp.write(data);
        udp.endPacket();
        Serial.println(data);
    }
}

// is connected to AP? (STA mode)
bool UdpComm_t::isReady()
{
    if(mode == UDP_COMM_MODE_AP)
    {
        return true;
    }else{
        return isConnected;
    }
}
