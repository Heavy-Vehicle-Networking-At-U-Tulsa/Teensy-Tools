//Displays Time On instrument Cluster using Spedometer and Tachometer for Hours and Minutes, respectively
//Coninuously Reads time from Teensy's Time library and outputs that time via CAN messages
//These CAN messages manipulate an Instrument Cluster to display Time
//Instrument Clusters from different Makes and Models of vehicles may use different CAN messages

//Originally created by Jason Staggs at the Univerity of Tulsa
//    http://tucrrc.utulsa.edu/CANClock.html

//Built using a Teensy 3.2 with additional oscillator for time and TimeLib
//    https://www.pjrc.com/teensy/td_libs_Time.html
//Functions for Timesync found in Arduino Examples
//    http://playground.arduino.cc/Code/Time    

//John Maag
//July 2017
//University of Tulsa: Student CyberTruck Experience


#include <TimeLib.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>

static CAN_message_t txmsg;
byte sign = 0;
int led = 13;
int id = 0x153;
int hr = 0;
int mn = 0;

void setup() {
  // put your setup code here, to run once:
  setSyncProvider(getTeensy3Time);
  Serial.begin(9600);
  Can0.begin(500000);
  
  delay(2);
  Serial.println("Sending Time Traffic on Bus");
  delay(1000);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }

}
void loop() {

  //Syncs with RealTime Clock to keep accurate Time when Available
  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t); // set the RTC
      setTime(t);
   }
    
  }
 
  hr = hour();
  mn = minute();

  //Display Time on Serial Monitor
  /*
  Serial.print(hr);
  Serial.print(":");
  Serial.print(mn);
  Serial.println();
  */

  //adjust hour to corresponding value of CAN message
  hr = (.78)*hr*10;
  hr = hr + (mn*.16);

  //CAN message to manipulate Speedometer
  txmsg.id = 0x153;
  txmsg.len = 8;
  txmsg.buf[0] = 0;
  txmsg.buf[1] = 0;
  txmsg.buf[2] = hr;
  txmsg.buf[3] = 0;
  txmsg.buf[4] = 0;
  txmsg.buf[5] = 0;
  txmsg.buf[6] = 0;
  txmsg.buf[7] = 0;
  Can0.write(txmsg);

  //adjust minute to corresponding value of CAN message
  mn = mn*2.5;
  
  //CAN message to manipulate Tachometer
  txmsg.id = 0x316;
  txmsg.len = 8;
  txmsg.buf[0] = 0;
  txmsg.buf[1] = 0;
  txmsg.buf[2] = 0;
  txmsg.buf[3] = mn;
  txmsg.buf[4] = 0;
  txmsg.buf[5] = 0;
  txmsg.buf[6] = 0;
  txmsg.buf[7] = 0;
  Can0.write(txmsg);


}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}
