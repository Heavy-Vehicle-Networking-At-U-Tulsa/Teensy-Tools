//Example used to find CAN ID's that manipulate an automotive instrument cluster
//Increments ID while sending multiple data combinations
//When movement is found in instrument cluster (ie. LED flash, Beep, etc),
//reference Teensy serial output and record ID
//Further analysis of that ID is needed to find what data bytes manipulate the cluster

//Originally reverse engineered by Jason Staggs at the Univerity of Tulsa
//    http://tucrrc.utulsa.edu/CANClock.html

//John Maag
//Hayden Allen
//June 2017
//University of Tulsa: Student CyberTruck Experience


#include <FlexCAN.h>
#include <kinetis_flexcan.h>
static CAN_message_t txmsg;

int id = 0x00;
int bufcount = 0;

void setup() {
  // put your setup code here, to run once:
 Can0.begin(500000);
 Serial.begin(9600);
 delay(2);
 Serial.println("Sending Incrementing ID's to visually find ID's active on Bus");
 delay(1000);
}

void loop() {

  //Sends four different messages from same ID alternating the bit values
  
  txmsg.id = id;
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
  delay(1000);

  txmsg.id = id;
  txmsg.len = 8;
  txmsg.buf[0] = 0xFF;
  txmsg.buf[1] = 0xFF;
  txmsg.buf[2] = 0xFF;
  txmsg.buf[3] = 0xFF;
  txmsg.buf[4] = 0xFF;
  txmsg.buf[5] = 0xFF;
  txmsg.buf[6] = 0xFF;
  txmsg.buf[7] = 0xFF;
  Can0.write(txmsg);
  delay(1000);

  txmsg.id = id;
  txmsg.len = 8;
  txmsg.buf[0] = 0x55;
  txmsg.buf[1] = 0x55;
  txmsg.buf[2] = 0x55;
  txmsg.buf[3] = 0x55;
  txmsg.buf[4] = 0x55;
  txmsg.buf[5] = 0x55;
  txmsg.buf[6] = 0x55;
  txmsg.buf[7] = 0x55;
  Can0.write(txmsg);
  delay(1000);

  txmsg.id = id;
  txmsg.len = 8;
  txmsg.buf[0] = 0xAA;
  txmsg.buf[1] = 0xAA;
  txmsg.buf[2] = 0xAA;
  txmsg.buf[3] = 0xAA;
  txmsg.buf[4] = 0xAA;
  txmsg.buf[5] = 0xAA;
  txmsg.buf[6] = 0xAA;
  txmsg.buf[7] = 0xAA;
  Can0.write(txmsg);
  delay(1000);
  Serial.println(id, HEX);


  id = id + 1;

//Used once an ID is found to find data bits
//Change data and loops as source of change is narrowed down 
/*
for(int i = 0x88; i<=0xFF; i++){
 // delay(10);
 while(bufcount<=7){
  txmsg.id = 0xC3F;
  txmsg.len = bufcount;
  txmsg.buf[0] = i;
  txmsg.buf[1] = i;
  txmsg.buf[2] = i;
  txmsg.buf[3] = i;
  txmsg.buf[4] = i;
  txmsg.buf[5] = i;
  txmsg.buf[6] = i;
  txmsg.buf[7] = i;
  Can0.write(txmsg);
  Serial.println(i, HEX);
  bufcount++;
 }
 bufcount = 0;
}
*/

//delay(80);
}
