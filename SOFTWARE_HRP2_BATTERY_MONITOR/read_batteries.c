/*
 * read_batteries.c
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

int main(int argc, char *argv[])
{
  int fd,n;
  unsigned char buf[64] = {0};
  struct termios toptions;
  /* open serial port */
  fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
  printf("fd opened as %i\n", fd);
  
  tcgetattr(fd, &toptions);
  toptions.c_cflag     |= (CLOCAL | CREAD);
  toptions.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
  toptions.c_oflag     &= ~OPOST;
  toptions.c_cc[VMIN]  = 0;
  toptions.c_cc[VTIME] = 10;
  tcsetattr(fd, TCSANOW, &toptions);
  while(1)
  {
      //clear terminal
      printf("\x1b[2J");
      printf("\x1b[0;0H");
      write(fd, "r", 1);    //ASK FOR A MEASURE TO BE SEND IN RAW FORMAT
      n = read(fd, buf, 10);//READ IT
      if (n!= 10) break;
        //~ #message format:
        //~ #|-00-|-01-|-02-|-03-|-04-|-05-|-06-|-07-|-08-|-09-|
        //~ #|cpt |IA_H|IA_L|VA_H|VA_L|IB_H|IB_L|VB_H|VB_L|crc |
        //~ #|8bit|16bits   |16bits   |16bits   |16bits   |8bit| 
      double V_bus_A   = (int16_t)((buf[3]<<8)|buf[4]) * 0.00125 * 0.5;
      double V_bus_B   = (int16_t)((buf[7]<<8)|buf[8]) * 0.00125 * 0.5;
      double V_shunt_A = (int16_t)((buf[1]<<8)|buf[2]) * 0.0000025 ;
      double V_shunt_B = (int16_t)((buf[5]<<8)|buf[6]) * 0.0000025 ;
      
      printf("V_bus_A %lf\t",V_bus_A);
      printf("V_shunt_A %lf\t",V_shunt_A);
      printf("V_bus_B %lf\t",V_bus_B);
      printf("V_shunt_B %lf\t",V_shunt_B);
      printf("\r\n");
      usleep(100);
  }
  return 0;
}
