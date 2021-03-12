//NU Sound Workshop
//Feb 23 2021

//for 1 drum pad
//not force/velocity sensitive

#define MIDI_CHANNEL 10 //usual channel for drums
#define NOTE 0x30 // C4. kinda arbitrary. Ideally you map it to something useful


//defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int readPin = A0;//read drumpad through A0 pin

int val;
int threshold = 400;//about 2V
int d=30;//delay between hits

void setup() {
  
  //supposedly makes analog read faster. idk haven't really tested it
  // Taken from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
    
  pinMode(readPin,INPUT);
  //rate for communicataing through serial port (COM)
  Serial.begin(115200);
}

void loop() {
    
  val=analogRead(readPin);//0 to 1023
  
  if (val>threshold){
    hit();
  }

}

//sends the MIDI message that the drum was hit
void hit(){
  noteOn(MIDI_CHANNEL,NOTE,127);
  delay(d);
  noteOff(MIDI_CHANNEL,NOTE);
  val=0;
}




// Functions from MIDI_Controller.ino
// Authors: Michael Balzer, Tim Crawford
// https://github.com/BEEMB/MIDI_Controller/blob/master/MIDI_Controller.ino

// Send a MIDI note on message
void noteOn(byte channel, byte pitch, byte velocity)
{
  // 0x90 is the first of 16 note on channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0x90 - 1;
  
  // Ensure we're between channels 1 and 16 for a note on message
  if (channel >= 0x90 && channel <= 0x9F)
  {
    #ifdef DEBUG
      Serial.print("Button pressed: ");
      Serial.println(pitch);
    #elif defined(TEENSY_PLUS_PLUS) || defined(TEENSY_2) || defined(TEENSY_PLUS_PLUS_2)
      usbMIDI.sendNoteOn(pitch, velocity, channel);
    #else
      Serial.write(channel);
      Serial.write(pitch);
      Serial.write(velocity);
    #endif
  }
}

// Send a MIDI note off message
void noteOff(byte channel, byte pitch)
{
  // 0x80 is the first of 16 note off channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0x80 - 1;
  
  // Ensure we're between channels 1 and 16 for a note off message
  if (channel >= 0x80 && channel <= 0x8F)
  {
    #ifdef DEBUG
      Serial.print("Button released: ");
      Serial.println(pitch);
    #elif defined(TEENSY_PLUS_PLUS) || defined(TEENSY_2) || defined(TEENSY_PLUS_PLUS_2)
      usbMIDI.sendNoteOff(pitch, 0x00, channel);
    #else 
      Serial.write(channel);
      Serial.write(pitch);
      Serial.write((byte)0x00);
    #endif
  }
}
