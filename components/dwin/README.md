# demo component to show how to include various other components

A configured uart is required.

Example:
```yaml

esphome:
  name: $devicename
  platform: "ESP32"
  board: nodemcu-32s
  on_boot:
    lambda: |-
      demo->restartHMI();
      demo->beepHMI();

uart:
  id: uart_a
  rx_pin: GPIO16
  tx_pin: GPIO17
  baud_rate: 115200
  debug:
    direction: BOTH

dwin:
  - id: demo
    uart_id: uart_a

interval:
  - interval: 5s
    then : 
      lambda: |-
        //demo->write_vp_command_(4,10);
        
        //demo->setVP(0x1002,2);
        //demo->setVP(0x1004,3);
        //demo->setVP(0x1006,4);
        demo->setColor(0x2003,0x041F); //041F = Blue
        demo->setVP(0x1000,1);
  - interval: 10s
    then : 
      lambda: |-
        //demo->beepHMI();
```

