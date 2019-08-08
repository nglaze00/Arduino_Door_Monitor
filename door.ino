/**
 * Baker Old Wing Door 2.0
 * 
 * Created by:  Nicholas Glaze (nkg2)
 *              Artun Bayer    (ab116)
 * ELEC 220
 */



#include "TM1637.h" 
#include "Ultrasonic.h"

/* Macro Define */
#define CLK               39          /* 4-Digit Display clock pin */
#define DIO               38          /* 4-Digit Display data pin */
#define ROTARY_ANGLE_P    24          /* pin of rotary angle sensor */
#define BUZZER            40
#define LIGHT_SENSOR      23
#define ULTRASONIC        25

/* Global Variables */
TM1637 tm1637(CLK, DIO);              /* 4-Digit Display object */
int analog_value = 0;                 /* variable to store the value coming from rotary angle sensor */



void setup() 
{
    /* Initialize 4-Digit Display */
    tm1637.init();
    tm1637.set(BRIGHT_TYPICAL);
    
    pinMode(BUZZER, OUTPUT);
    update_clock(0);
    delay(1000);
}
boolean door_open(){//True if door is open
  return analogRead(ROTARY_ANGLE_P) > 100;
}
int time_limit(){//Compute time limit with linear function
  return floor(.004082*analogRead(LIGHT_SENSOR)+5.102);
}
void scream(){//Display "AAAA" on 4-digit display
  for(int i = 0; i < 4; i++){
    tm1637.display(i, 10);
  }
}
void sound_open_alarm() 
{//Play alarm (sick beat)
  scream();
  while(door_open()) drop_beat();
}
int pitch(int dist){//Calculate pitch based on linear function

  return floor(45*dist+55)/2;
}
void sound_tamper_alarm(){//Sound alarm based on object distance from ultrasonic sensor
  scream();
  long dist = measure_dist();
  int p;
  while(dist < 3000){
    for(int i = 0; i < 10; i++){
    p = pitch(dist);
    //update_clock(dist);
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(p);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(p);
    }
    dist = measure_dist();
    
  }
  update_clock(0000);
}
void update_clock(int n){
  //Displays the number n on the 4-digit display
  for(int i = 3; i > -1; i--){
    tm1637.display(i, n % 10);
    n /= 10;
  }
 
}
void count_down(int time_limit){//Run countdown on timer, starting from time_limit
  //Counts the timer down from time_limit to 0
  int time_left = time_limit;
  while (time_left >= 0 and door_open()){
    if(measure_dist() < 3000) sound_tamper_alarm();
    update_clock(time_left);
    time_left -= 1;
    delay(1000);
  }
  if(door_open()){
    sound_open_alarm();
  }
  update_clock(0);
}
/* the loop() method runs over and over again */
void loop() 
{   
    if(door_open()){
      count_down(time_limit());
    }
    if(measure_dist() < 3000){
      sound_tamper_alarm();
    }
    
 
}


void drop_beat(){//Play a sick beat
  
  int length = 16;         /* the number of notes */
  char notes[] = "121 12123 3 4546"; //notes in the song
  float beats[] = { 1, 1, 1,    1,    1, 1, 1, 1, 0.6,       1 ,    0.6,      1,        1, 1, 1, 1}; //length of each note
  int tempo = 150;
  
  /* the setup() method runs once, when the sketch starts */
    int i = 0;
      //Loop through each note
    while(i < length && door_open()) 
    {
      //space indicates a pause
      if(notes[i] == ' ') 
      {
        delay(beats[i] * tempo);
      } 
      else 
      {
        playNote(notes[i], beats[i] * tempo);
      }
      delay(tempo / 2);    /* delay between notes */
      i++;
    }
  }
  
  /* play tone */
  void playTone(int tone, int duration) 
  {
    for (long i = 0; i < duration * 1000L; i += tone * 2) 
    {
      digitalWrite(BUZZER, HIGH);
      delayMicroseconds(tone);
      digitalWrite(BUZZER, LOW);
      delayMicroseconds(tone);
    }
  }
  
  char names[] = { '2', '1', '6', '5', '4', '3',};
  float tones[] = { 587/2, 698/2, 880/2, 554/2, 659/2, 440/2};
  void playNote(char note, int duration) 
  {
    
    
    // play the tone corresponding to the note name
    for (int i = 0; i < 6; i++) 
    {
      if (names[i] == note) 
      {
        playTone(tones[i], duration);
      }
    }
  
}

long measure_dist(){//Measure object distance from ultrasonic sensor
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC,LOW);
  pinMode(ULTRASONIC,INPUT);
  long duration;
  duration = pulseIn(ULTRASONIC,HIGH);
  long RangeIndist;
  RangeIndist = duration;
  return RangeIndist;
}
 
