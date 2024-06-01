#include "dwin.h"
#include "esphome/core/log.h"
#include <stdio.h>

namespace esphome {
namespace dwin {

static const char *TAG = "dwin";

#define CMD_HEAD1           0x5A
#define CMD_HEAD2           0xA5
#define CMD_WRITE           0x82
#define CMD_READ            0x83

#define MIN_ASCII           32
#define MAX_ASCII           255

#define CMD_READ_TIMEOUT    50
#define READ_TIMEOUT        100

void DWIN::setup() {
}

void DWIN::loop() {
  //while (this->available()) {
  //  uint8_t c;
  //  this->read_byte(&c);
  //  this->handle_char_(c);
  //}
}

void DWIN::init(Stream* port, bool isSoft){
    this->_dwinSerial = port;
    this->_isSoft = isSoft;
}

void DWIN::echoEnabled(bool echoEnabled){
    _echo = echoEnabled;
}

// Get Hardware Firmware Version of DWIN HMI
double DWIN::getHWVersion(){  //  HEX(5A A5 04 83 00 0F 01)
    byte sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x04, CMD_READ, 0x00, 0x0F, 0x01};
    _dwinSerial->write(sendBuffer, sizeof(sendBuffer)); 
    delay(10);
    return readCMDLastByte();
}

// Restart DWIN HMI
void DWIN::restartHMI(){  // HEX(5A A5 07 82 00 04 55 aa 5a a5 )
    byte sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x07, CMD_WRITE, 0x00, 0x04, 0x55, 0xaa, CMD_HEAD1, CMD_HEAD2};
    _dwinSerial->write(sendBuffer, sizeof(sendBuffer)); 
    delay(10);
    readDWIN();
}

// SET DWIN Brightness
void DWIN::setBrightness(byte brigtness){
    byte sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x04, CMD_WRITE, 0x00, 0x82, brigtness };
    _dwinSerial->write(sendBuffer, sizeof(sendBuffer));
    readDWIN();
}

// SET DWIN Brightness
byte DWIN::getBrightness(){
    byte sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x04, CMD_READ, 0x00, 0x31, 0x01 };
    _dwinSerial->write(sendBuffer, sizeof(sendBuffer));
    return readCMDLastByte();
}

// Chnage Page 
void DWIN::setPage(byte page){
    //5A A5 07 82 00 84 5a 01 00 02
    byte sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x07, CMD_WRITE, 0x00, 0x84, 0x5A, 0x01, 0x00, page};
    _dwinSerial->write(sendBuffer, sizeof(sendBuffer));
    readDWIN();
}

// Get Current Page ID
byte DWIN::getPage(){
    byte sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x04, CMD_READ, 0x00 , 0x14, 0x01};
    _dwinSerial->write(sendBuffer, sizeof(sendBuffer)); 
    return readCMDLastByte();
}

// Set Text on VP Address
void DWIN::setText(long address, String textData){

    int dataLen = textData.length();
    byte startCMD[] = {CMD_HEAD1, CMD_HEAD2, dataLen+3 , CMD_WRITE, 
    (address >> 8) & 0xFF, (address) & 0xFF};
    byte dataCMD[dataLen];textData.getBytes(dataCMD, dataLen+1);
    byte sendBuffer[6+dataLen];

    memcpy(sendBuffer, startCMD, sizeof(startCMD));
    memcpy(sendBuffer+6, dataCMD, sizeof(dataCMD));

    _dwinSerial->write(sendBuffer, sizeof(sendBuffer));
    readDWIN();
}

// Set Data on VP Address
void DWIN::setVP(long address, byte data){
    // 0x5A, 0xA5, 0x05, 0x82, 0x40, 0x20, 0x00, state
    byte sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x05 , CMD_WRITE, (address >> 8) & 0xFF, (address) & 0xFF, 0x00, data};
    _dwinSerial->write(sendBuffer, sizeof(sendBuffer));
    readDWIN();
}

// beep Buzzer for 1 Sec
void DWIN::beepHMI(){
    // 0x5A, 0xA5, 0x05, 0x82, 0x00, 0xA0, 0x00, 0x7D
    byte sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x05 , CMD_WRITE, 0x00, 0xA0, 0x00, 0x7D};
    _dwinSerial->write(sendBuffer, sizeof(sendBuffer));
    readDWIN();
}


// SET CallBack Event
void DWIN::hmiCallBack(hmiListner callBack){
    listnerCallback = callBack;
}

// Listen For incoming callback  event from HMI
void DWIN::listen(){
     handle();
}

String DWIN::readDWIN(){
      //* This has to only be enabled for Software serial
    #if defined(DWIN_SOFTSERIAL)
        if(_isSoft){
            ((SoftwareSerial *)_dwinSerial)->listen(); // Start software serial listen
        }  
    #endif

    String resp;
    unsigned long startTime = millis(); // Start time for Timeout

    while((millis() - startTime < READ_TIMEOUT)){
        if(_dwinSerial->available() > 0){
            int c = _dwinSerial->read();
            resp.concat(" "+String(c, HEX));
        }
    }
    if (_echo){
        Serial.println("->> "+resp);
    }
    return resp;
}

String DWIN::checkHex(byte currentNo){
    if (currentNo < 10){
        return "0"+String(currentNo, HEX);
    }
    return String(currentNo, HEX);
}

String DWIN::handle(){

    int lastByte;
    String response;
    String address;
    String messege;
    bool isSubstr = false;
    bool messegeEnd = true;
    bool isFirstByte = false;
    unsigned long startTime = millis(); 
  
    while((millis() - startTime < READ_TIMEOUT)){
        while(_dwinSerial->available() > 0){
            delay(10);
            int inhex = _dwinSerial->read();
            if (inhex == 90 || inhex == 165){
                isFirstByte = true;
                response.concat(checkHex(inhex)+" ");
                continue;
            }
            for(int i = 1 ; i <= inhex ;i++){
                int inByte = _dwinSerial->read();
                response.concat(checkHex(inByte)+" ");
                if (i <= 3){
                    if((i == 2) || (i == 3)){
                        address.concat(checkHex(inByte));
                    }
                    continue;
                }
                else{
                    if(messegeEnd){
                        if (isSubstr && inByte != MAX_ASCII && inByte > MIN_ASCII){
                            messege += char(inByte);
                        }
                        else{
                            if(inByte == MAX_ASCII){
                                messegeEnd = false;
                            }
                            isSubstr = true;
                        }
                    }
                }
                lastByte = inByte;
            }
        }
    }

    if (isFirstByte &&_echo){
        Serial.println("Address : " + address + " | Data : " + String(lastByte, HEX) + " | Messge : " + messege + " | Response " +response );
    }
    if (isFirstByte){
        listnerCallback(address, lastByte, messege, response);
    }
    return response;
}



byte DWIN::readCMDLastByte(){
      //* This has to only be enabled for Software serial
    #if defined(DWIN_SOFTSERIAL)
        if(_isSoft){
            ((SoftwareSerial *)_dwinSerial)->listen(); // Start software serial listen
        }  
    #endif

    byte lastByte = -1;
    unsigned long startTime = millis(); // Start time for Timeout
    while((millis() - startTime < CMD_READ_TIMEOUT)){
        while(_dwinSerial->available() > 0){
            lastByte = _dwinSerial->read();
        }
    }
    return lastByte;
}


void DWIN::flushSerial(){
  Serial.flush();
  _dwinSerial->flush();
}

}  // namespace dwin
}  // namespace esphome
