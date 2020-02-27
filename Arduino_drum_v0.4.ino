/*
Title:         Arduino_drum
Version:       0.4
Author:        Lukas Fechner
Miscellaneous: Project's hardware decription can be found here:
               http://arduinodrumkit.blogspot.com/

PLEASE share your opinion and feel encouraged to contribute in ANY way!
ALL ERRORS EXCLUDE LIABILITY FROM THE AUTHOR. But they are human, so report them kindly.
*/

static const int BAUDRATE = 19200;

#include <MIDI.h>
#include <String.h>

// Create a foreach loop by code extension
// you can use the variables pos = 'position in' and sze = 'size of' array
#define foreach(item, arr)\
          for(int keep = 1, pos = 0, sze = sizeof(arr)/ (sizeof *(arr));\
            keep && pos < sze; keep = !keep, pos++)\
          for(item = (arr) +  pos; keep; keep = 0)

// DEBUG info code extension for enabling a debug printout the shorthand way
#define debugPrint()\
          String pd  = (String)*pad;\
          String str = "\nPad:" + pd +  ",\tvelocity= " + ((int) velocity*0.7874)\
            + "\tdeltaT: " + (millis()-timeLastHit[pos]) + "\t|";\
          Serial.print(str);

struct HairlessMidiSettings : public midi::DefaultSettings
{
   static const bool UseRunningStatus = false;
   static const long BaudRate = BAUDRATE;
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessMidiSettings);

/* RememberMe for MIDIsetup
#Id    DrumPart
38  =  snare drum
36  =  kick 
53  =  hihat closed
77  =  crash
56  =  hihat opened
*/

// AnalogIn threshholds for counting a pad 'hit'
const int volumeThreshold = 30;
const int minDelayBetweenHits = 15;
 
// Defines pad pins and volume for each Pad
// Analogic positions in drumkit[], MIDIsetup[] and volume[] correlate
int  drumkit[]    = {A0};
byte MIDIsetup[]  = {36, 36, 53, 77, 56};    
float volume[]    = {3,  1,  1,  1,  1};

unsigned long  timeLastHit[] = {0,  0,  0,  0,  0};
int            lastValue[]   = {0,  0,  0,  0,  0};

// Takes analogRead() value, scope: [0,1023]
int  testValue;
void setup()
{
  MIDI.begin();
  Serial.begin(BAUDRATE);
}

void loop()
{  
  // Runs over every in drumkit[] defined int and does an analogRead for the
  // specified pins. Uses syntax defined in foreach.
  foreach(int* pad, drumkit){
    testValue = analogRead(*pad);

    if(testValue > volumeThreshold && (testValue - lastValue[pos]) > 20 \
       && (millis() - timeLastHit[pos] > minDelayBetweenHits))
    {

      int velocity = (int) (testValue * volume[pos] / 8);
      if(velocity > 127)
        velocity = 127;
      else if(velocity < 0)
        velocity = 0;

      // Prints Debug info - look at corresponding #define directive at the biginning of the file.
      // Either Addictive Drums OR the Serial Monitor can be listening to the serial data transfer.
           debugPrint();
      
      timeLastHit[pos] = millis();
      MIDI.sendNoteOn(MIDIsetup[pos], velocity, 1);
    }
    lastValue[pos] = testValue;
  }
}
