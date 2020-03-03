#include <Arduino.h>
#include <string.h>
#include <MuxShield.h>
#include <MIDI.h>

/* 
 Plays music chords a Arduino Leonardo and a MUX sheild
 Tim Wilson
 3/22/2015
 6/9/2019 
 2/26/2020
 */

MIDI_CREATE_DEFAULT_INSTANCE();

int numButtons = 48; //number of buttons set up. keeps empty inputs from wierding out
int buttonState[48];
int previousButtonState[48];

int selectedOctave = 5; // Octave
int OCTAVE = 12*selectedOctave;

int i;
int j;
int delayTime = 500;

int greenLED = 13;
int yellowLED = 9;

//Initialize the Mux Shield
MuxShield muxShield;

int keyMap [48] = {
46, 47, 44, 45,
33, 32, 35, 34,
22, 23, 20, 21,
9,  8,  11, 10,
43, 40, 41, 
29, 28, 31, 30,
18, 19, 16, 17,
5,  4,  7,  6,
42,
38, 39, 36, 37,
25, 24, 27, 26,
14, 13, 15, 12,
1,  0,  3,  2
};

const int VOL_HIGH = 127;
const int MAJOR_OFFSET_1 = 4;
const int MAJOR_OFFSET_2 = 7;
const int MINOR_OFFSET_1 = 3;
const int MINOR_OFFSET_2 = 7;
const int AUG_OFFSET_1 = 4;
const int AUG_OFFSET_2 = 8;
const int DIM_OFFSET_1 = 3;
const int DIM_OFFSET_2 = 6;


void ThreeChord(int base, int offset1, int offset2, bool isOn) {
  int baseWithOctave = base + OCTAVE;
  if(isOn){
    digitalWrite(greenLED, HIGH);
    MIDI.sendNoteOn(baseWithOctave, VOL_HIGH, 1);
    MIDI.sendNoteOn(baseWithOctave + offset1, VOL_HIGH, 1);
    MIDI.sendNoteOn(baseWithOctave + offset2, VOL_HIGH, 1);
  } else {
    digitalWrite(greenLED, LOW);
    MIDI.sendNoteOff(baseWithOctave, VOL_HIGH, 1);
    MIDI.sendNoteOff(baseWithOctave + offset1, VOL_HIGH, 1);
    MIDI.sendNoteOff(baseWithOctave + offset2, VOL_HIGH, 1);
  }
}


void setup() {
  
    //Set IO1, IO2, and IO3 as digital inputs
    //use DIGITAL_IN in place of DIGITAL_IN_PULLUP if internal pullups are not needed
    muxShield.setMode(1,DIGITAL_IN_PULLUP);
    muxShield.setMode(2,DIGITAL_IN_PULLUP);
    muxShield.setMode(3,DIGITAL_IN_PULLUP);

  //Populate previousButtonState with 0s
  for (i=0; i<numButtons; i++){
    previousButtonState[i] = 0;
  }
  
  // Initialize the LEDs
  pinMode(greenLED, OUTPUT); 
  pinMode(yellowLED, OUTPUT); 
  
  MIDI.begin();          // Launch MIDI and listen to channel 4. Might not need this
}

void loop() {
  
  for (i=0; i<numButtons; i++) {
    if (i<16){
      buttonState[i] = muxShield.digitalReadMS(1,i);      
    } else if ((i>=16) && (i<32)){
      buttonState[i] = muxShield.digitalReadMS(2,(i-15));
    } else if (i>=32) {
      buttonState[i] = muxShield.digitalReadMS(3,(i-32));
    }
    if (buttonState[i] != previousButtonState[i]) {
      previousButtonState[i] = buttonState[i]; 
      bool isButtonHigh = buttonState[i] == HIGH;
      int key = keyMap[i];
      int chordBase;
      int offset1;
      int offset2;
        if (key < 12) {
          offset1 = MAJOR_OFFSET_1;
          offset2 = MAJOR_OFFSET_2;
          chordBase = key;
        } else if (12 <= key && key < 24) {
          offset1 = MINOR_OFFSET_1;
          offset2 = MINOR_OFFSET_2;
          chordBase = key - 12;
        } else if (24 <= key && key < 36) {
          offset1 = AUG_OFFSET_1;
          offset2 = AUG_OFFSET_2;
          chordBase = key - 24;
        } else if (36 <= key && key < 48) {
          offset1 = DIM_OFFSET_1;
          offset2 = DIM_OFFSET_2;
          chordBase = key - 36;
        }
      ThreeChord(chordBase, offset1, offset2, isButtonHigh);
    }
  }  
}
