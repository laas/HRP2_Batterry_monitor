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
float P_A = 0.0;
float P_B = 0.0;
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

inaA.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_140US, INA226_SHUNT_CONV_TIME_140US, INA226_MODE_SHUNT_BUS_CONT);
inaB.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_140US, INA226_SHUNT_CONV_TIME_140US, INA226_MODE_SHUNT_BUS_CONT);

  
  // Calibrate INA226. Rshunt = 0.002 ohm, Max excepted current = 20A
  inaA.calibrate(0.002, 20);
  inaB.calibrate(0.002, 20);
}

void loop() 
{
  digitalWrite(PB1, HIGH); 
  delay(1000);
  digitalWrite(PB1, LOW);    
  delay(10); 

  int t=micros();
  I_A = inaA.readShuntCurrent();
  I_B = inaB.readShuntCurrent();
  
  V_A = inaA.readBusVoltage();
  V_B = inaB.readBusVoltage();
  
  P_A = inaA.readBusPower();
  P_B = inaB.readBusPower();
  Serial.println(micros()-t);
  Serial.print("Bus voltage:   ");
  Serial.print(V_A, 5);
  Serial.print(" V\t");
  Serial.print(V_B, 5);
  Serial.println(" V");

  Serial.print("Bus power:     ");
  Serial.print(P_A, 5);
  Serial.print(" W\t");
  Serial.print(P_B, 5);
  Serial.println(" W");


//  Serial.print("Shunt voltage: ");
//  Serial.print(inaA.readShuntVoltage(), 5);
//  Serial.print(" V\t");
//  Serial.print(inaB.readShuntVoltage(), 5);
//  Serial.println(" V");

  Serial.print("Shunt current: ");
  Serial.print(I_A, 5);
  Serial.print(" A\t");
  Serial.print(I_B, 5);
  Serial.println(" A");
  Serial.println("");
    

  
  
}
