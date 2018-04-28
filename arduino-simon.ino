const int LEDS[] = {5, 7, 10, 12}; // pins corresponding to the LEDs
const int BUTTONS[] = {4, 6, 9, 11}; // pins corresponnding to the buttons
const int FREQUENCIES[] = {170, 200, 230, 260}; // frequency for the tone for each LED
const int NUM_LEDS = 4;
const int LOSE_FREQUENCY = 50; // frequency for game over tone
const int WIN_FREQUENCY = 350; // frequency for win tone
const int PIEZO = 3; // pin number for piezo
const int MAX_ROUND = 25; // you win if you get here
int duration = 400; // generic duration constant, in ms
int potDuration = 500; // duration of pattern display (in ms), set by potentiometer
bool isReading = false; // whether we're reading input from the user, or displaying the pattern
int lightOn = -1; // holds the index of the currently lit light. -1 means no light is lit.
unsigned long tOn; // the time the light turned on, in milliseconds since reset
int pattern[MAX_ROUND];
int roundNum = 1; // which round we're on
int patternInd = 0; // where the user is in entering the pattern

void setup() {
  // set all LED pins to output and all button pins to input
  for(int i=0; i<NUM_LEDS; i++) {
    pinMode(LEDS[i], OUTPUT);
    pinMode(BUTTONS[i], INPUT);
  }

  randomSeed(analogRead(A5));
}

void loop() {
  int potReading = analogRead(A0);
  potDuration = map(potReading, 0, 1023, 100, 1000);
  
  if (isReading) {
    // Poll each button to see if it is pressed
    for(int i=0; i<NUM_LEDS; i++) {
      
      // if this button is pressed, and its LED was not lit just before
      if (digitalRead(BUTTONS[i]) && lightOn != i) {
        digitalWrite(LEDS[i], HIGH); // light up its LED
        
        // if there was already a different light on, turn that one off
        if (lightOn != -1){
          digitalWrite(LEDS[lightOn], LOW);
        }

        tOn = millis(); // remember the time we turned it on so it will stay on for the specified duration
        lightOn = i;
        
        // output a tone, depending on whether this was the next light in the pattern
        if (lightOn == pattern[patternInd]) {
          tone(PIEZO, FREQUENCIES[i]);
          patternInd++;
          
          if (patternInd == roundNum) { // if they've reached the end of the pattern successfully
            roundNum++;
            patternInd = 0;
            isReading = false;
            // We can use delay because we don't need to worry about getting new button presses in the meantime
            delay(duration);

            digitalWrite(LEDS[lightOn], LOW);
            noTone(PIEZO);
            
            delay(duration);
          }
          
        } else { // wrong input, restart
          // Instead of the normal tone, we play a special bad-sounding one
          tone(PIEZO, LOSE_FREQUENCY);
          // We can use delay because we don't need to worry about getting new button presses in the meantime
          delay(duration);
          
          digitalWrite(LEDS[lightOn], LOW);
          noTone(PIEZO);
          
          delay(duration);
          for(int i=0; i<NUM_LEDS; i++) {
            flashLed(i, 100);
          }
          reset(); // reset all relevant variables so we can restart
        }
      }
    }

    // If the light has been on for longer than the duration,
    // turn it off and reset lightOn.
    if ((lightOn != -1) && (millis() - tOn > duration)) {
      digitalWrite(LEDS[lightOn], LOW);
      noTone(PIEZO);

      lightOn = -1;
    }
  } else { // Otherwise, we're not in reading mode. Either output the next pattern or a celebratory signal
    if (roundNum == MAX_ROUND) { // they won!
      tone(PIEZO, WIN_FREQUENCY);
      for(int j = 0; j<3; j++) {
        for(int i = NUM_LEDS-1; i>=0; i--) {
          flashLed(i, 100);
        }
      }
      noTone(PIEZO);
      
      reset();
    } else { // They haven't won yet. Keep playing and output a lengthened pattern
      pattern[roundNum - 1] = random(NUM_LEDS); // append a new random LED to the pattern array

      // display the next pattern, with the duration specified by the potentiometer
      for(int i=0; i<roundNum; i++) {
        delay(potDuration / 2);
        tone(PIEZO, FREQUENCIES[pattern[i]]);
        flashLed(pattern[i], potDuration);
        noTone(PIEZO);
      }
      
      isReading = true; // now that we've displayed the pattern, it's time for the user to repeat it
    }
  }

} // end loop()

/**
 * flashLed - lights up an LED for a designated amount of time, then turns it off
 * @parameter int ledIndex - the index of the LED to light up, as defined in LEDS array
 * @parameter int duration - time, in milliseconds, to light up for
 */
void flashLed(int ledIndex, int duration) {
  int led = LEDS[ledIndex]; // the pin of the LED to light up
  
  digitalWrite(led, HIGH);
  delay(duration);
  digitalWrite(led, LOW);
}

/**
 * reset - reset all relevant variables so the game can start over.
 */
void reset() {
  roundNum = 1;
  patternInd = 0;
  isReading = false;
  delay(duration*2);
}

