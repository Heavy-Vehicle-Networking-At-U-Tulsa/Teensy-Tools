//Example that displays text to an lcd screen via UART serial communication
//Uses Arduino's SoftwareSerial library:
//    https://www.arduino.cc/en/Reference/SoftwareSerial

//John Maag
//June 2017
//University of Tulsa: Student CyberTruck Experience

#include <SoftwareSerial.h>


int led = 13;
SoftwareSerial mySerial(0,1); // pin 1 = TX, pin 0 = RX (unused)
 
void setup() {
  
  //starts serial communication to screen
  mySerial.begin(9600); //monitor baud rate

  //On-board LED
  pinMode(led, OUTPUT); 
  digitalWrite(led,HIGH); 
  
}
void loop() {

  mySerial.write(254); // cursor to beginning of first line of screen
  mySerial.write(128);
  mySerial.write("Hello");
  mySerial.write(" **     ");
  delay(0);
  mySerial.write(254); // cursor to 7th position on second line of screen
  mySerial.write(192);
  mySerial.write("World             ");
  
}

