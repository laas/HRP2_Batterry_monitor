/*
 * log_batteries.c
 * 
 * Copyright 2016 Thomas Flayols <tflayols@rausudake>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
//~ #include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
//~ #include <errno.h>
#include <sys/ioctl.h>
//~ #include <stdint.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
  
  struct timeval tv;
  struct timeval start_tv;
  int fd,n;
  FILE * logFile;
  unsigned char buf[64] = {0};
  double current_time;
  struct termios toptions;
  int time_stop = 0;
  /* open serial port */
  fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
  if (argc == 3)
  {
    sscanf(argv[2], "%d", &time_stop);
  }
  else
  {
      printf("Error, Usage is: log_batteries \"outputfile\" time \r\n  with time in seconds\r\n");
      return -1;
  }
  printf("Batteries states will be captured for %d seconds and written in \"%s\"\r\n",time_stop,argv[2]);
  //~ printf("Serial port opened as %i\n", fd);

  tcgetattr(fd, &toptions);
  toptions.c_cflag     |= (CLOCAL | CREAD);
  toptions.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
  toptions.c_oflag     &= ~OPOST;
  toptions.c_cc[VMIN]  = 0;
  toptions.c_cc[VTIME] = 10;
  tcsetattr(fd, TCSANOW, &toptions);
  
  /*open logFile*/
  logFile = fopen(argv[1], "w");
  /*write header*/
  fprintf(logFile,"Time\t Voltage_A\t Current_A\t Voltage_B\t Current_B\r\n");
 
  gettimeofday(&start_tv, NULL);
  while(1)
  {
      //clear terminal
      //~ printf("\x1b[2J");
      //~ printf("\x1b[0;0H");
      //write(fd, "r", 1);    //ASK FOR A MEASURE TO BE SEND IN RAW FORMAT
      //n = read(fd, buf, 10);//READ IT
      gettimeofday(&tv, NULL);
      current_time = (tv.tv_sec - start_tv.tv_sec) +  (tv.tv_usec - start_tv.tv_usec) / 1000000.0;
      if (n== 10||1) 
      {
        //~ #message format:
        //~ #|-00-|-01-|-02-|-03-|-04-|-05-|-06-|-07-|-08-|-09-|
        //~ #|cpt |IA_H|IA_L|VA_H|VA_L|IB_H|IB_L|VB_H|VB_L|crc |
        //~ #|8bit|16bits   |16bits   |16bits   |16bits   |8bit| 
      double V_bus_A   = (int16_t)((buf[3]<<8)|buf[4]) * 0.00125 * 2.0 / 0.9450400236;
      double V_bus_B   = (int16_t)((buf[7]<<8)|buf[8]) * 0.00125 * 2.0 / 0.9408849205;
      double V_shunt_A = (int16_t)((buf[1]<<8)|buf[2]) * 0.0000025 / 1.0261337886; 
      double V_shunt_B = (int16_t)((buf[5]<<8)|buf[6]) * 0.0000025 / 1.0189463657;
      double I_A = V_shunt_A / 0.002;
      double I_B = V_shunt_B / 0.002;
      fprintf(logFile,"%lf\t%lf\t%lf\t%lf\t%lf\n",current_time,V_bus_A, I_A, V_bus_B, I_B);
  
      //~ printf(" SENSOR A\t SENSOR B\r\n");
      //~ printf(" %lf A\t%lf A\r\n",I_A    , I_B    );
      //~ printf(" %lf V\t%lf V\r\n",V_bus_A, V_bus_B);
      //~ printf(" %lf\r\n",current_time);
	  }
	  else
	  {
		  printf("BAD RESPOND\r\n");
          usleep(1000);
	  }
      usleep(100);
      if (current_time > time_stop) break;
  }
  fclose(logFile);
  return 0;
}
