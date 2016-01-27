#include <HardWire.h>
#include <Wire.h>
#include <WireBase.h>
#include "INA226.h"

//************ HRP2 Battery monitor *****************
//LAAS CNRS - Thomas FLAYOLS - 11 jan 2016
//measure voltage and current of each of the 2 batteries of HRP2 via INA226
//send measurements to HRP2 computer via USB-UART

#define INA_A 0x40
#define INA_B 0x4F


INA226 inaA;
INA226 inaB;

float I_A = 0.0;
float I_B = 0.0;
float V_A = 0.0;
float V_B = 0.0;

int16_t REG_V_shunt_A = 0;
int16_t REG_V_shunt_B = 0;
int16_t REG_V_bus_A   = 0;
int16_t REG_V_bus_B   = 0;

char led = 1;
char message[10] ={0};

char c;
void setup() 
{
  
  Serial.begin(115200);
  Serial.println("START");
  pinMode(PB1, OUTPUT); //LED
    
  inaA.begin(INA_A);
  inaB.begin(INA_B);
  // Configure INA226
//inaA.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
//inaB.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
//
//inaA.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_140US, INA226_SHUNT_CONV_TIME_140US, INA226_MODE_SHUNT_BUS_CONT);
//inaB.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_140US, INA226_SHUNT_CONV_TIME_140US, INA226_MODE_SHUNT_BUS_CONT);
//
inaA.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_332US, INA226_SHUNT_CONV_TIME_332US, INA226_MODE_SHUNT_BUS_CONT);
inaB.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_332US, INA226_SHUNT_CONV_TIME_332US, INA226_MODE_SHUNT_BUS_CONT);

  // Calibrate INA226. Rshunt = 0.002 ohm, Max excepted current = 40A
  inaA.calibrate(0.002, 40);
  inaB.calibrate(0.002, 40);
}

void loop() 
{
  while(1)
  {
    led=!led;
    digitalWrite(PB1, led); 
    if (Serial.available() )
    {
      c=Serial.read();
      switch(c)
      {
    //****************************************************
        case('r'): //RAW DATA HEX
          REG_V_shunt_A = inaA.readRegister16(INA226_REG_SHUNTVOLTAGE);
          REG_V_shunt_B = inaB.readRegister16(INA226_REG_SHUNTVOLTAGE);
          REG_V_bus_A   = inaA.readRegister16(INA226_REG_BUSVOLTAGE);
          REG_V_bus_B   = inaB.readRegister16(INA226_REG_BUSVOLTAGE);
          // message format:
          //  |-00-|-01-|-02-|-03-|-04-|-05-|-06-|-07-|-08-|-09-|
          //  |cpt |IA_H|IA_L|VA_H|VA_L|IB_H|IB_L|VB_H|VB_L|crc |
          //  |8bit|16bits   |16bits   |16bits   |16bits   |8bit|
          message[0]++;
          message[1]= 0xff & (REG_V_shunt_A>>8);
          message[2]= 0xff & (REG_V_shunt_A);
          message[3]= 0xff & (REG_V_bus_A  >>8);
          message[4]= 0xff & (REG_V_bus_A  );
          message[5]= 0xff & (REG_V_shunt_B>>8);
          message[6]= 0xff & (REG_V_shunt_B);
          message[7]= 0xff & (REG_V_bus_B  >>8);
          message[8]= 0xff & (REG_V_bus_B  );
          message[9]= 0xaa ^ message[0] 
                           ^ message[1] 
                           ^ message[2] 
                           ^ message[3] 
                           ^ message[4] 
                           ^ message[5] 
                           ^ message[6] 
                           ^ message[7] 
                           ^ message[8] ;
          Serial.write(message,10);
          break;
    //****************************************************
        case('v'): //Verbose: Human readable data (for debug):
          I_A = inaA.readShuntCurrent();
          I_B = inaB.readShuntCurrent();
          V_A = inaA.readBusVoltage();
          V_B = inaB.readBusVoltage();
          Serial.print("\r\n");
          Serial.print(V_A, 5);
          Serial.print(" V\t");
          Serial.print(V_B, 5);
          Serial.print(" V\t\t");
          Serial.print(I_A, 5);
          Serial.print(" A\t");
          Serial.print(I_B, 5);
          Serial.print(" A\t");
          break;
    //****************************************************
        case('h'):
          Serial.println("*********************");
          Serial.println("Usage: ");
          Serial.println(" 'r': send RAW data from ina226 last measurement (10 bytes respond:)");
          Serial.println("    |-00-|-01-|-02-|-03-|-04-|-05-|-06-|-07-|-08-|-09-|");
          Serial.println("    |cpt |IA_H|IA_L|VA_H|VA_L|IB_H|IB_L|VB_H|VB_L|crc |");
          Serial.println("    |8bit|16bits   |16bits   |16bits   |16bits   |8bit|");
          Serial.println(" 'v': send human readable data from ina226 last measurement");
          Serial.println(" 'h': display this menu");
          break;
      }
    }
  }
}

