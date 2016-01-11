//************ HRP2 Battery monitor *****************
//LAAS CNRS - Thomas FLAYOLS - 11 jan 2016
//measure voltage and current of each of the 2 batteries of HRP2 via INA226
//send measurements to HRP2 computer via USB-UART

void setup() 
{
  pinMode(PB1, OUTPUT);
}

void loop() 
{
  digitalWrite(PB1, HIGH); 
  delay(1000);
  digitalWrite(PB1, LOW);    
  delay(100); 
}
