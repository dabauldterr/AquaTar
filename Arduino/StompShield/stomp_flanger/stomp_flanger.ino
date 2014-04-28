/*
stomp_flanger.pde
guest openmusiclabs.com 7.14.13
this program does a flanger effect, and interpolates between samples
for a smoother sound output.  a rampwave is used to set the variable
delay.  the min and max delay times it swing between is set by MIN
and MAX.  these are in samples, divide by 31.25ksps to get ms delay
times.  the rotary encoder determines how much the ramp increments
by each time.  this sets the frequency of the delay sweep, which is
min/maxed by B_MIN/B_MAX.
*/

#include "StompShield.h"

#define MIN 10 // min delay of ~300us
#define MAX 200 // max delay of ~6ms
#define SIZE MAX+10 // data buffer size - must be more than MAX
#define B_MAX 100 // max value for rotary encoder input (min freq)
#define B_MIN 1 // min value for rotary encoder input (max freq)
int buffer[SIZE]; // create a data buffer
byte dir = 1; // keeps track of up/down counting
unsigned int location = 0; // incoming data buffer pointer
byte button; // button checking timer
byte last_state; // last rotary encoder state
byte counter = 10; // rotary encoder rotation counter (and start value)
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
  }
}

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
  
  // fetch/store data
  buffer[location] = input; // store current sample
  location++; // go to next sample position
  if (location >= SIZE) location = 0; // deal with buffer wrap
  int temp = location - (fractional >> 8); // find delayed sample
  if (temp < 0) temp += SIZE; // deal with buffer wrap
  int output = buffer[temp]; // fetch delayed sample
  temp -= 1; // find adjacent sample
  if (temp < 0) temp += SIZE; // deal with buffer wrap
  int output2 = buffer[temp]; // get adjacent sample
  
  // interpolate between adjacent samples
  int temp4; // create some temp variables
  int temp5;
  // multiply by distance to fractional position
  MultiSU16X8toH16(temp4, output, (0xff - (fractional & 0x00ff)));
  MultiSU16X8toH16(temp5, output2, (fractional & 0x00ff));
  output = temp4 + temp5; // sum weighted samples
  // save value for playback next interrupt
  data_buffer = output; 
  
  // up or down count as necessary till MIN/MAX is reached
  if (dir) {
    if ((fractional >> 8) >= MAX) dir = 0;
    fractional += counter;
  }
  else {
    if ((fractional >> 8) <= MIN) dir = 1;
    fractional -= counter;;
  } 
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
      Serial.println(command);
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
      counter=k;
      break;
    case 'Y': 
     //counter=k << 8;  
      break;
  }
}

