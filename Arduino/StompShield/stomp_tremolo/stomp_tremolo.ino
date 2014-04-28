/*
stomp_tremolo.pde
guest openmusiclabs.com 7.14.13
this program does a tremolo effect.  it uses a sinewave stored
in program memory to modulate the signal.  the rate at which
it goes through the sinewave is set by the rotary encoder,
which is min/maxed by B_MIN/B_MAX.
*/

#include "StompShield.h"

#define B_MAX 100 // max value for rotary encoder input (min freq)
#define B_MIN 1 // min value for rotary encoder input (max freq)
PROGMEM  prog_int16_t sinewave[]  = {
  // this file is stored in StompShield and is a 1024 value
  // sinewave lookup table of signed 16bit integers
  // you can replace it with your own waveform if you like
  #include <sinetable.inc>
};
unsigned int location = 0; // incoming data buffer pointer
byte button; // button checking timer
byte last_state; // last rotary encoder state
byte counter = 4; // rotary encoder rotation counter (and start value)
unsigned int fractional = 0x00; // fractional sample position
int data_buffer; // temporary data storage to give a 1 sample buffer

void setup() {
  Serial.begin(9600);
  StompShield_init(); // setup the arduino for the shield
}

void loop() {
   if(Serial.available()){ 
      readCommand();
      cli();
        setValues();
      sei();
  }}

ISR(TIMER1_OVF_vect) { // all processing happens here

  // output the last value calculated
  OCR1AL = ((data_buffer + 0x8000) >> 8); // convert to unsigned, send out high byte
  OCR1BL = data_buffer; // send out low byte
  
  // get ADC data
  byte temp1 = ADCL; // you need to fetch the low byte first
  byte temp2 = ADCH; // yes it needs to be done this way
  int input = ((temp2 << 8) | temp1) + 0x8000; // make a signed 16b value
  
  button--; // check buttons every so often
  if (button == 0) {
    byte temp3 = (PIND & 0x94); // mask off button pins
    if (((last_state | temp3) & 0x10) == 0) { // falling edge on pin4
      if ((temp3 & 0x04) == 0) { // low value on pin2
        if (counter > B_MIN) counter--; // if not at min, decrement
      }
      else { // high value on pin2
        if (counter < B_MAX) counter++; // if not at max, increment
      }
    }
    last_state = (temp3 ^ 0x94); // invert buttons states and save
    button = 0x20; // reset counter (determines how often buttons are checked)
  }
  
  fractional += counter; // increment sinewave lookup counter
  if (fractional >= 0x0100) { // if its large enough to go to next sample
    fractional &= 0x00ff; // round off
    location += 1; // go to next location
    location &= 0x03ff; // fast boundary wrap for 2^n boundaries
  }
  // fetch current sinewave value
  int amplitude = pgm_read_word_near(sinewave + location);
  amplitude += 0x8000; // convert to unsigned
  int output;
  MultiSU16X16toH16(output, input, amplitude);
  // save value for playback next interrupt
  data_buffer = output; 
}
//Used for reading Serial Data
char command[4];
byte b;
int index=0;
void readCommand(){
  while(Serial.available()>0){
    if(index<4){
      b=Serial.read();
      command[index]=b;
      index++;
    }
    else if(index>=4){
      //Serial.println(command);
      index=0;
    }
  }
}
void setValues(){
  int k1=command[1] - '0';
  int k2=command[2] - '0';
  int k3=command[3] - '0';
  int k=(k1*100)+(k2*10)+k3;
  switch(command[0]){
    case 'X':
    //Upshift the value to a 16b value with << 8
      counter=k << 8;
      Serial.println(counter);
      break;
    case 'Y': 
      //counter=k;  
      break;
  }
}


