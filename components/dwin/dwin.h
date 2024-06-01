#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
//#include "esphome/components/button/button.h"
#include "esphome/components/output/binary_output.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace dwin {

class DWIN : public Component,  public uart::UARTDevice {
 public:
 
  float get_setup_priority() const override { return setup_priority::LATE; }
  void setup() override;
  void loop() override;
  //void dump_config() override;

  //From Arduino DWIN_DGUS_HMI library --------------------------------------------------------
 		
  void echoEnabled(bool enabled);
  // Listen Touch Events & Messeges from HMI
  void listen();
  // Get Version
  double getHWVersion();
  // restart HMI
  void restartHMI();
  // set Perticulat Page
  void setPage(byte pageID);
  // get Current Page ID
  byte getPage();
  // set LCD Brightness
  void setBrightness(byte pConstrast);
  // set LCD Brightness
  byte getBrightness();
  // set Data on VP Address
  void setText(long address, String textData);
  // set Byte on VP Address
  void setVP(long address, byte data);
  // beep Buzzer for 1 sec
  void beepHMI();
  // Callback Function
  typedef void (*hmiListner) (String address, int lastByte, String messege, String response);

  // CallBack Method
  void hmiCallBack(hmiListner callBackFunction);
  //End From Arduino DWIN_DGUS_HMI library --------------------------------------------------------

 protected:
	

 private:

    //From Arduino DWIN_DGUS_HMI library --------------------------------------------------------
    #ifndef ESP32
    SoftwareSerial* localSWserial = nullptr; 
    #endif

    Stream* _dwinSerial;   // DWIN Serial interface
    bool _isSoft;          // Is serial interface software
    long _baud;              // DWIN HMI Baud rate
    bool _echo;            // Response Command Show
    bool _isConnected;     // Flag set on successful communication

    bool cbfunc_valid;
    hmiListner listnerCallback;

    void init(Stream* port, bool isSoft); 
    byte readCMDLastByte();
    String readDWIN();
    String handle();
    String checkHex(byte currentNo);
    void flushSerial();

    //End From Arduino DWIN_DGUS_HMI library --------------------------------------------------------

};

}  // namespace dwin
}  // namespace esphome
