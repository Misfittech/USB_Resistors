
#include <Wire.h>
#include <wiring_private.h>
#include "commands.h"


#define I2C_ADDR_R1 (0x2E)
#define I2C_ADDR_R2 (0x2C)
#define I2C_ADDR_R3 (0x2F)
#define I2C_ADDR_R4 (0x2E) //Second I2C bus
#define I2C_ADDR_R5 (0x2C) //Seconds I2C bus

#define PIN_SCL (21) //PA23
#define PIN_SDA (20) //PA22

#define PIN_USB_PWR (38) //PA13 this pin is high when usb is connected


#define PIN_SCL2 (13) //PA17
#define PIN_SDA2 (11) //PA16



TwoWire Wire1(&sercom1, PIN_SDA2, PIN_SCL2);


extern "C" void SERCOM1_Handler(void) {
    Wire1.onService();
 }
  
#define SerialUSB Serial

bool sendCmdAD5272Bus0(uint8_t i2cAddr, uint8_t cmd, uint16_t value)
{
  uint8_t data[2];
  data[0]=cmd<<2;
  data[0] |= (value>>8) & 0x03;
  data[1] = (value & 0x0ff);

   Wire.beginTransmission(i2cAddr); // transmit to device
   Wire.write(data[0]);
   Wire.write(data[1]);
   if (0 == Wire.endTransmission())
   {
    return true;
   }
  return false;
}

bool readCmdAD5272Bus0(uint8_t i2cAddr, uint8_t cmd, uint16_t *ptrvalue)
{
  bool ret=true;
  uint8_t data[2];
  *ptrvalue=0;
  ret=sendCmdAD5272Bus0(i2cAddr, cmd, 0); 
  if (!ret)
  {
    return ret; 
  }
  
   Wire.beginTransmission(i2cAddr); // transmit to device
   Wire.requestFrom(i2cAddr,2);
   data[1]=Wire.read();
   data[0]=Wire.read();
   if(0 != Wire.endTransmission())
   {
    ret=false; 
   }
   *ptrvalue= (uint16_t)data[0] | ((uint16_t)(data[1] & 0x03))<<8; 
   return ret;
}


bool sendCmdAD5272Bus1(uint8_t i2cAddr, uint8_t cmd, uint16_t value)
{
  bool ret;
  uint8_t data[2];
  data[0]=cmd<<2;
  data[0] |= (value>>8) & 0x03;
  data[1] = (value & 0x0ff);

   Wire1.beginTransmission(i2cAddr); // transmit to device
   Wire1.write(data[0]);
   Wire1.write(data[1]);
   if (0 == Wire1.endTransmission())
   {
    return true;
   }
  return false;

}

bool readCmdAD5272Bus1(uint8_t i2cAddr, uint8_t cmd, uint16_t *ptrvalue)
{
  bool ret=true;
  uint8_t data[2];
  *ptrvalue=0;
  ret=sendCmdAD5272Bus1(i2cAddr, cmd, 0); 
  if (!ret)
  {
    return ret; 
  }
  
   Wire1.beginTransmission(i2cAddr); // transmit to device
   Wire1.requestFrom(i2cAddr,2);
   data[1]=Wire1.read();
   data[0]=Wire1.read();
   if(0 != Wire1.endTransmission())
   {
    ret=false; 
   }
   *ptrvalue= (uint16_t)data[0] | ((uint16_t)(data[1] & 0x03))<<8; 
   return ret;
}


void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(115200);
  pinMode(PIN_USB_PWR, INPUT);
  pinMode(PIN_SCL, INPUT);
  pinMode(PIN_SDA, INPUT);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(36, INPUT);
  pinMode(37, INPUT);


  Wire.begin();
  Wire1.begin();
  pinPeripheral(PIN_SCL2, PIO_SERCOM);
  pinPeripheral(PIN_SDA2, PIO_SERCOM);
 
  
   uint32_t to=5000;
  if (digitalRead(PIN_USB_PWR))
  {
    //wait for USB serial port to come alive
    while (!SerialUSB)
    {
      to--;
      if (to == 0)
      {
        break;
      }
      delay(500);
    };     //wait for serial
  } 
  SerialUSB.println("Power Up");


 
//
//  //enable writes to the RDAC
  sendCmdAD5272Bus0(I2C_ADDR_R1, 7, 0x02);
  sendCmdAD5272Bus0(I2C_ADDR_R2, 7, 0x02);
  sendCmdAD5272Bus0(I2C_ADDR_R3, 7, 0x02);
  sendCmdAD5272Bus1(I2C_ADDR_R4, 7, 0x02);
  sendCmdAD5272Bus1(I2C_ADDR_R5, 7, 0x02);
  SerialUSB.println("Done");

//  sendCmdAD5272Bus0(I2C_ADDR_R1, 1, 256);
//  SerialUSB.println("read");
//  SerialUSB.println(readAD5272Bus0(I2C_ADDR_R1));

  commandsInit();

}

void loop() {
  // put your main code here, to run repeatedly:
  commandsProcess();
}
