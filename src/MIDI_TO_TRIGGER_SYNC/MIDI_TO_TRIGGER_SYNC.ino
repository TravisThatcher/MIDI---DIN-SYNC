#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

// MIDI - DIN SYNC
// Rev 01.1 05-16 Travis Thatcher - recompas@gmail.com
// Creative Commons Attribution 4.0 International License

// midi values
#define OFF 1
#define ON 2
#define WAIT 3

#define CLOCK_LENGTH 9000 // Play with this value if you are getting strange clock jitter, time in Micro-seconds
#define CLOCK_OUT 9
#define LED_OUT 12
#define MODE_INPUT 5

boolean DEBUG = false;  // to turn off all debug print messages

int clockDiv=24;
int clockCount=0;
int clockState = LOW;
int prevClockState = LOW;

long lastDebounceTime = 0;
long debounceDelay = 50;

byte incomingByte;
byte velocity;

MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  
  MIDI.turnThruOff();
  if(!DEBUG){
    MIDI.begin(1);  // Set midi channel here
  }else{
    Serial.begin(19200); // Othewise debug mode = no midi, allows for printing debug info to serial monitor
  }
  if(!DEBUG){
    Serial.println("hello!");
  }  


  // analog pins
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  pinMode(A1, OUTPUT);
  digitalWrite(A1, LOW);

  // Clock Mode
  pinMode(MODE_INPUT, INPUT);
  
  // Clock out
  pinMode(CLOCK_OUT, OUTPUT);
  digitalWrite(CLOCK_OUT, LOW);
  
  pinMode(LED_OUT, OUTPUT);
  digitalWrite(LED_OUT, LOW);
  
  if(!DEBUG){
    MIDI.setHandleStart(HandleStart);
    MIDI.setHandleStop(HandleStop);
    MIDI.setHandleContinue(HandleContinue);
    MIDI.setHandleClock(HandleClock);
  }
}

void loop() {
  MIDI.read();
  int reading = digitalRead(MODE_INPUT);

  if(reading != prevClockState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
   
    if (reading != clockState) {
      clockState = reading;
      if (clockState == HIGH) {
        if(DEBUG){
          Serial.println("clock!");
        }
        clockCount=0;  
        switch(clockDiv){
          case 24:
            clockDiv = 12;
          break;
          case 12:
            clockDiv = 6;
          break;
          case 6:
            clockDiv = 48;
          break;
          case 48:
            clockDiv = 24;
          break;
          default:
          break;
        }
      }
    }
  }

  prevClockState = reading;
  
}

void HandleStart() {
  digitalWrite(A1, HIGH);
}

void HandleContinue() {
  digitalWrite(A1, HIGH);
}

void HandleStop() {
  digitalWrite(A1, LOW);
    clockCount=0;
}

void HandleClock() { 
  if(clockCount>=clockDiv){
    clockCount = 0;
  }
  digitalWrite(A0, HIGH);
  if(clockCount==0){ 
    digitalWrite(CLOCK_OUT, HIGH);
    digitalWrite(LED_OUT, HIGH);
  }
  delayMicroseconds(CLOCK_LENGTH); 
  
    digitalWrite(CLOCK_OUT, LOW);
    digitalWrite(LED_OUT, LOW ); 
   
  clockCount++;
  digitalWrite(A0, LOW);      
}


