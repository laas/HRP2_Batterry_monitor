from IPython import embed
import serial
import time

def binaryInt16_to_float(wd):
    '''Convert a signed int to a number'''
    wd_L=ord(wd[1])
    wd_H=ord(wd[0])
    binary16 = (wd_H << 8) + wd_L
    if(binary16 &    0x8000):
        binary16 = -0x10000 + binary16
    return  binary16;
ser = serial.Serial('/dev/ttyACM0')

ser.timeout = 0.1
#message format:
#|-00-|-01-|-02-|-03-|-04-|-05-|-06-|-07-|-08-|-09-|
#|cpt |IA_H|IA_L|VA_H|VA_L|IB_H|IB_L|VB_H|VB_L|crc |
#|8bit|16bits   |16bits   |16bits   |16bits   |8bit|


def read_batteries_V_I(ser):
    ser.write('r')             #ask for a measure to be send in RAW format
    ser.write('r')#force overflow
    message = ser.read(10)     #read it 
    ser.flushInput() 

    if not(len(message) == 10):
        return -1
    else:
        V_bus_A =   (binaryInt16_to_float(message[3:3+2]) * 0.00125 * 0.5) 
        V_bus_B =   (binaryInt16_to_float(message[7:7+2]) * 0.00125 * 0.5) 
        V_shunt_A = (binaryInt16_to_float(message[1:1+2]) * 0.0000025) 
        V_shunt_B = (binaryInt16_to_float(message[5:5+2]) * 0.0000025)
        #Check CRC:
        crc =(0xaa ^
              ord(message[0]) ^ 
              ord(message[1]) ^ 
              ord(message[2]) ^ 
              ord(message[3]) ^ 
              ord(message[4]) ^ 
              ord(message[5]) ^ 
              ord(message[6]) ^ 
              ord(message[7]) ^ 
              ord(message[8])  )
        if (not crc == ord(message[9])):
            return -1
    return (V_bus_A,V_bus_B,V_shunt_A,V_shunt_B)

while(True):
    t1=time.time()
    V_bus_A,V_bus_B,V_shunt_A,V_shunt_B = read_batteries_V_I(ser)
    print str((time.time()-t1)*1000 ) + " ms"
    print( str(V_bus_A) + "V\t"
          + str(V_bus_B) + "V\t"
          + str(V_shunt_A) + "V\t" 
          + str(V_shunt_B) + "V")




