#ifndef    _UDP_COMM_H_
#define    _UDP_COMM_H_

#ifdef _UDP_COMM_C_
#define EXTERN
#else
#define EXTERN extern
#endif

#include <ESP8266WiFi.h> // WiFi
#include <ESP8266mDNS.h> // mDNS
#include <WiFiUDP.h>     // UDP

// receive buffer size
#define UDP_COMM_BUFF_SIZE 64

// mode of WiFi
#define UDP_COMM_MODE_AP    0
#define UDP_COMM_MODE_STA   1

// UDP communication class
class UdpComm_t
{
public:
    UdpComm_t();
    // set port numbers
    void setPort(int localPort, int remotePort);
    // begin AP mode
    void beginAP(char* ssid, char* password);
    // begin STA mode
    void beginSTA(char* ssid, char* password, char* hostName);
    // cyclic procedure. It must be called from main loop()
    void loop();
    // send data
    void send(char* data);
    // is ready for communication?
    bool isReady();
    
    // callback. It is called when data have been received
    void (*onReceive)(char* data);
    
    // my SSID, password (AP mode)
    char mySSID[33];
    char myPassword[64];
    // AP's SSID and password (STA mode)
    char hisSSID[33];
    char hisPassword[64];
    // my own IP address and port number
    IPAddress localIP;
    int localPort;
    // remote host's IP address and port number
    IPAddress remoteIP;
    int remotePort;
    // my own host name
    char hostName[32];
    
private:
    // cyclic procedure (AP mode)
    void loopAP();
    // cyclic procedure (STA mode)
    void loopSTA();
    
    // AP mode or STA mode
    int mode;
    // receive data buffer
    char buff[UDP_COMM_BUFF_SIZE];
    // UDP object
    WiFiUDP udp;
    // mDNS object
    MDNSResponder mdns;
    // is connected to AP? (STA mode)
    bool isConnected;
};

// Global object
EXTERN UdpComm_t UdpComm;

#endif
