//Functioning alarm clock using an automotive intrument cluster
//Features include:
//Displays Time On instrument Cluster using Spedometer and Tachometer for Hours and Minutes, respectively
//Coninuously Reads time from Teensy's Time library and outputs that time via CAN messages
//These CAN messages manipulate an Instrument Cluster to display Time
//Instrument Clusters from different Makes and Models of vehicles may use different CAN messages
//Buttons are used to set Alarm Time, turn alarm on or off, and display alarm time
//When Alarm is turned on to current time, the instrument cluster will beep until alarm is turned off
//LCD screen is also used to diplay information for easy understanding of instrument cluster

//Originally inspired by Jason Staggs at the Univerity of Tulsa
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
#include <SoftwareSerial.h>
#include <kinetis_flexcan.h>

SoftwareSerial mySerial(0,1); // pin 1 = TX, pin 0 = RX (unused) 
static CAN_message_t txmsg;
byte sign = 0;
int id = 0x153;
int hr = 1;
int LCDhr; 
int LCDAhr;
int mn = 1;
int pm = 1;
int apm = 1;
int alarmHour = 0;
int alarmMin = 0;
int alarmOn = 0;
int alarmSet = 0;
long lastmillisHour = 0;
long lastmillisMin = 0;
long lastmillistimeadjust = 0;

void setup() {

  setSyncProvider(getTeensy3Time);
  Serial.begin(9600);
  mySerial.begin(9600); //monitor baud rate
  Can0.begin(500000);

  //pins on Teensy used for alarm buttons
  pinMode(10, INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);

  
  delay(2);
  Serial.println("Sending Time Traffic on bus");
  delay(1000);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  setSyncInterval(1);
  
  
  //my current hardware and software implementation
  //creates innacurate time over a period of time
  //(ie. after 24 hours time is off by 15 minutes)
  //after following TimeLib library's adjustments
  //time is still inaccurate. May be hardware issue.

  //used to attempt to make time accurate
  Teensy3Clock.compensate(-300000);
  delay(1000);
 

}
void loop() {
  
  
  //syncs accurate time
  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t); // set the RTC 
      setTime(t);
   }
    
  }
  
  //used to attempt to make time accurate
  if((millis()-lastmillistimeadjust) >= 120000){
    adjustTime(-1);
    lastmillistimeadjust = millis();
  }

  //Checks if alarm is turned on or off using an external switch
  if(digitalRead(10) == LOW){
    alarmOn = 1;
  }
  else{
    alarmOn = 0;
  }
  if(!alarmOn){
     txmsg.id = 0x329;
     Can0.write(txmsg);
  }

  //Checks if alarm is being set using an external switch
  if(digitalRead(8) == LOW){
    alarmSet = 1;
  }
  else{
    alarmSet = 0;
  }

  //adjusts hour to display 12 on LCD after midnight
  if(hr == 0){
    LCDhr = 12;
  }
  else{
    LCDhr = hr;
  }

  //Displays time on LCD
  mySerial.write(254); // cursor to beginning of first line
  mySerial.write(128);
  mySerial.write("Time: ");
  mySerial.write((LCDhr/10)+48);
  mySerial.write((LCDhr%10)+48);
  mySerial.write(":");
  mySerial.write((mn/10)+48);
  mySerial.write((mn%10)+48);
  if(pm<0){
     mySerial.write(" PM  ");
  }
  else{
     mySerial.write(" AM  ");
  }

  //Displays Alarm time on LCD
  if(alarmHour == 0){
    LCDAhr = 12;
  }
  else{
    LCDAhr = alarmHour;
  }
  delay(0);
  mySerial.write(254); // cursor to 7th position on second line
  mySerial.write(192);
  mySerial.write("A: "); 
  mySerial.write((LCDAhr/10)+48);
  mySerial.write((LCDAhr%10)+48);
  mySerial.write(":");
  mySerial.write((alarmMin/10)+48);
  mySerial.write((alarmMin%10)+48);
  if(apm<0){
     mySerial.write(" PM");
  }
  else{
     mySerial.write(" AM");
  }
  mySerial.write(" On:");
  if(alarmOn){
     mySerial.write("Y");
  }
  else{
     mySerial.write("N");
  }

  //prints time on Serial monitor
 /*
  Serial.println(hr);
  Serial.println(mn);
  Serial.println(alarmHour);
  Serial.println(alarmMin);
  Serial.println(alarmOn);
 */

  //regular loop when alarm is not being set
  if(!alarmSet){

     //If it is the time the alarm is set to, continously send message to create beep from instrument cluster
     if(alarmHour == hr && alarmMin == mn && apm == pm){
      while(alarmOn){
        if(digitalRead(10) == LOW){
        alarmOn = 1;
        }
        else{
          alarmOn = 0;
        }
        txmsg.id = 0x153;
        txmsg.len = 8;
        txmsg.buf[7] = 0x30;
        Can0.write(txmsg);
        delay(300);
      }
    }

  
    hr = hour();
    mn = minute();

    //check for PM
    if(hr>=12){
      hr = hr-12;
      pm = -1;
    }
    else{
      pm = 1;
    }

    //displays P on instrument cluster signaling PM
    if(pm<0){
      txmsg.id = 0xC3F;
      txmsg.len = 8;
      txmsg.buf[1] = 0x08;
      txmsg.buf[2] = 0x80;
      Can0.write(txmsg);
    }
  
    //display regular time on instrument cluster   
    txmsg.id = 0x153;
    txmsg.len = 8;
    txmsg.buf[0] = 0;
    txmsg.buf[1] = 0;
    txmsg.buf[2] = ((.78)*hr*10)+(mn*.13);
    txmsg.buf[3] = 0;
    txmsg.buf[4] = 0;
    txmsg.buf[5] = 0;
    txmsg.buf[6] = 0;
    txmsg.buf[7] = 0;
    Can0.write(txmsg);

    txmsg.id = 0x316;
    txmsg.len = 8;
    txmsg.buf[0] = 0;
    txmsg.buf[1] = 0;
    txmsg.buf[2] = 0;
    txmsg.buf[3] = mn*2.5;
    txmsg.buf[4] = 0;
    txmsg.buf[5] = 0;
    txmsg.buf[6] = 0;
    txmsg.buf[7] = 0;
    Can0.write(txmsg);
  }

  //SetAlarm is turned on, changes instrument cluster to display alarm time
  else{

    //changes alarm hour time
    if(digitalRead(9) == LOW){
      if(millis() - lastmillisHour >=500){
         alarmHour = alarmHour+1;
         lastmillisHour = millis();
      }
    }

    //adjusts for pm
    if(alarmHour>=12){
      alarmHour = alarmHour-12;
      apm = apm*-1;
    }
    //displays P on instrument cluster signaling PM
    if(apm<0){
      txmsg.id = 0xC3F;
      txmsg.len = 8;
      txmsg.buf[1] = 0x08;
      txmsg.buf[2] = 0x80;
      Can0.write(txmsg);
    }

    //changes alarm minute time
    if(digitalRead(7) == LOW){
       if(millis() - lastmillisMin >=500){
          alarmMin = alarmMin+2;
         lastmillisMin = millis();
      }
    }
    //resets minute time
    if (alarmMin>=60){
      alarmMin = 0;
    }

    //displays alarm time on instrament cluster
    txmsg.id = 0x153;
    txmsg.len = 8;
    txmsg.buf[0] = 0;
    txmsg.buf[1] = 0;
    txmsg.buf[2] = ((.78)*alarmHour*10) + (alarmMin*.13);
    txmsg.buf[3] = 0;
    txmsg.buf[4] = 0;
    txmsg.buf[5] = 0;
    txmsg.buf[6] = 0;
    txmsg.buf[7] = 0;
    Can0.write(txmsg);
    txmsg.id = 0x316;
    txmsg.len = 8;
    txmsg.buf[0] = 0;
    txmsg.buf[1] = 0;
    txmsg.buf[2] = 0;
    txmsg.buf[3] = alarmMin*2.5;
    txmsg.buf[4] = 0;
    txmsg.buf[5] = 0;
    txmsg.buf[6] = 0;
    txmsg.buf[7] = 0;
    Can0.write(txmsg);
  }

}


/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
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
