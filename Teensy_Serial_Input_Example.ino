//Example that reads input from serial monitor and stores the characters in an array

//John Maag
//June 2017
//University of Tulsa: Student CyberTruck Experience


// Teensy 3.x / Teensy LC have the LED on pin 13
const int ledPin = 13;


int character;
int i = 0;
char transmittedData[256];


void setup() {
  
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  delay(10);
  Serial.println("Input any text");
  
}


void loop() {

  digitalWrite(ledPin, LOW);    // set the LED off
  delay(10);  // wait for a second
  
  //checking if text was input
  if (Serial.available()){
    digitalWrite(ledPin, HIGH);   // set the LED on

    //reading single character and storing in array until input is empty 
    while(Serial.available()){
      character = Serial.read();
      Serial.print(char (character));
      transmittedData[i] = character;
      i++;
    }

    //displaying the data stored
    //This data could be used in a program for any number of reasons
    Serial.println();
    for(int j = 0; j<i; j++){
      Serial.println(transmittedData[j]);
    }
    Serial.println();

    //reset size of input to recieve new input
    i = 0;
    delay(500);
  }
 
}

