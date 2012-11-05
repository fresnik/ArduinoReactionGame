// Assignment 9. ARDUINO PROGRAMMING - INTO
// Last modification: 31.10.2012
// Authors:
//  Freyr Bergsteinsson <freyrb12@ru.is>
//  Egill Bjornsson <egillb12@ru.is>

// Play a reaction game

// Win the game by being quick on the button and reach 10 points.
// When you start the game, you have 0 points. To get a point you need to
// press the button on the board within 1100ms after the RGB led lights
// up with a blue color. The light goes off at random, so you'll have to be
// on your toes. If you don't press the button within this period, the
// light goes red and you get a minus point. If you press too soon, this
// will also happen. Sometimes the light that comes up isn't blue,
// but red, in which case the player should not press the button, which
// will result in a minus point.
//
// If the player manages to press the button while the blue light is on,
// the light goes green, a positive buzz is played and one point is received
// (the 7-segment display will show 1).

// After each reaction (red or green light), you do the above process again,
// but now with a smaller reaction period. The reaction period then goes down
// by 100ms for each point you get, so with 9 points you only have 200ms to
// press the button and win the game!

// Video showing the assembled Arduino running this code:
//  http://youtu.be/mJl5DEDiNxk


const int segmentPin   =   7; // Control pin for the 7-segment display
const int buttonPin    =   8; // Input pin for the button
const int firstRGBPin  =   9; // First of the three RGB led pins
const int buzzerPin    =  12; // Output pin for the buzzer

enum GameState
{
  not_running,
  pre_reaction,
  reaction_period_press,
  reaction_period_nopress
};

// Arduino pins used for seven segment display: 0,1,2,3,4,5,6
// Arduino pins used for RGB leds: 9, 10, 11
// This array contains 10 elements, one for each digit (0-9). The first
// 7 numbers in each elements controls which pins should light up the
// seven segment display (0 = light on, 1 = light off) and the last
// three numbers control which colors (red, green, blue) should be turned
// on for the RGB led (0 = off, 1 = on)
// The 11th element is an underscore
const byte seven_seg_digits[12][10] =
                           { { 0,0,0,0,0,0,1, 0,0,0 },  // = 0
                             { 1,0,0,1,1,1,1, 1,0,0 },  // = 1
                             { 0,0,1,0,0,1,0, 0,1,0 },  // = 2
                             { 0,0,0,0,1,1,0, 1,1,0 },  // = 3
                             { 1,0,0,1,1,0,0, 0,0,1 },  // = 4
                             { 0,1,0,0,1,0,0, 1,0,1 },  // = 5
                             { 0,1,0,0,0,0,0, 0,1,1 },  // = 6
                             { 0,0,0,1,1,1,1, 1,1,1 },  // = 7
                             { 0,0,0,0,0,0,0, 0,0,0 },  // = 8
                             { 0,0,0,1,1,0,0, 0,0,0 },  // = 9
                             { 1,1,1,0,1,1,1, 0,0,0 },  // = _
                             { 0,1,1,0,1,1,0, 0,0,0 },  // = ≡
                           };

const int LIGHT_OFF   = 0;
const int RED_LIGHT   = 1;
const int GREEN_LIGHT = 2;
const int BLUE_LIGHT  = 4;

// Global variable to store the last known state of the button
int lastButtonState = LOW;
// Global variable to store which digit we're currently displaying
int currentScore    = 0;
// Keep track of what's going on in the game
GameState currentGameState = not_running;
// Start time of current period, whether pre_reaction or reaction_period
unsigned long startReactionTime = 0;
// Length of the current period, whether pre_reaction or reaction_period, in ms
int reactionPeriod = 0;
// If we fail 3 times, turn game off
int failures = 0;
// Current high score
int highScore = 0;

void setup() {
  // Set all the pins for the 7-segment display as output pins
  for (int pin = 0; pin < 8; pin++)
  {
    pinMode(pin, OUTPUT);
  }

  pinMode(buttonPin, INPUT);  // The button pin is an input
  pinMode(buzzerPin, OUTPUT); // The buzzer pin is an output

  // Activate the 7 segment led display
  digitalWrite( segmentPin, HIGH );

  // Initialize the pseudo-random number generator
  randomSeed(analogRead(0));

  resetGame();
}

// Display a specific digit on the 7-segment display
void sevenSegWrite(byte digit) {
  byte pin = 0;
  for (byte segCount = 0; segCount < 7; ++segCount)
  {
    digitalWrite(pin, seven_seg_digits[digit][segCount]);
    ++pin;
  }
}

// Set the RGB led to the color(s) associated with a specific digit
void setRGBLed( byte digit ) {
  byte pin = firstRGBPin;
  for (byte pinCount = 0; pinCount < 3; ++pinCount) {
    digitalWrite(pin, seven_seg_digits[digit][7+pinCount]);
    pin++;
  }
}

// Play a sound with a specific frequency for a specified length
// Made by Rob Faludi - http://www.faludi.com/itp/arduino/buzzer_example.pde
void buzz(long frequency, long length) {
  long delayValue = 1000000/frequency/2;
  long numCycles = frequency * length/ 1000;

  for (long i=0; i < numCycles; i++)
  {
    digitalWrite(buzzerPin,HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(buzzerPin,LOW);
    delayMicroseconds(delayValue);
  }
}

void resetGame()
{
  sevenSegWrite( 10 );
  currentGameState = not_running;
  failures = 0;
  highScore = 0;
  currentScore = 0;
}

void doWinAnimation()
{
  // Do a little animation, begin by turning off all the leds on the
  // 7-segment display
  for (byte segCount = 0; segCount < 7; ++segCount)
    digitalWrite(segCount, 1);

  // Do twelve steps (quick) - for each step, display a different part of
  // the 7-segment display and play a sound of a different frequency
  int segLeg = 0;
  for (long hz = 880; hz > 220; hz -= 55)
  {
    int leg = segLeg++ % 6;
    digitalWrite(leg, 0);
    buzz(hz, 25);
    digitalWrite(leg, 1);
  }
  // End on a high note
  buzz(880, 50);
  
  sevenSegWrite( 11 );
  delay( 4000 );

  resetGame();
}

void setupPreReaction()
{
  setRGBLed( LIGHT_OFF );
  sevenSegWrite( currentScore );
  currentGameState = pre_reaction;
  // Set the pre-reaction period to anything between 2 and 7 seconds
  reactionPeriod = random(5000) + 2000;
  startReactionTime = millis();
}

void doLoseAnimation()
{
  // Do a "game over" jingle
  buzz( 523.25, 200 );
  buzz( 261.63, 200 );
  for (int i = 0; i < 3; i++)
  {
    buzz( 329.63, 50 );
    buzz( 392.0, 50 );
  }
  buzz( 329.63, 200 );

  // Blink the high score a few times
  sevenSegWrite( highScore );
  for (int i = 0; i < 4; i++)
  {
    digitalWrite( segmentPin, LOW );
    delay(1000);
    digitalWrite( segmentPin, HIGH );
    delay(1000);
  }

  resetGame();
}

void reactionFailed()
{
  if ( currentScore > 0 )
    currentScore--;
  failures++;

  // Show a red light and make a "fail" sound
  setRGBLed( RED_LIGHT );
  buzz(440, 100);
  buzz(220, 200);
  delay( 400 );
  setRGBLed( LIGHT_OFF );

  // Game over or continue?
  if ( failures > 2 )
  {
    doLoseAnimation();
  }
  else
    setupPreReaction();
}

void reactionSuccess()
{
  currentScore++;
  sevenSegWrite( currentScore );

  // Show a green light and make a "success" sound
  setRGBLed( GREEN_LIGHT );
  buzz(440, 100);
  buzz(880, 200);
  delay( 400 );
  setRGBLed( LIGHT_OFF );

  // Update the highscore
  if ( currentScore > highScore )
    highScore = currentScore;

  // Has the player won, or should we continue?
  if ( currentScore > 9 )
    doWinAnimation();
  else
    setupPreReaction();
}

void loop() {
  // Get the state of the push-button
  int buttonState = digitalRead( buttonPin );

  // If the player just pressed the button...
  if ( (buttonState != lastButtonState) && (buttonState == HIGH) )
  {
    switch ( currentGameState )
    {
      case not_running:
        // We just started a new game, go to pre_reaction
        setupPreReaction();
        break;
      case pre_reaction:
      case reaction_period_nopress:
        // Oh dear, the player pressed the button too soon, or pressed
        // it when the reaction period was in "no press" state
        reactionFailed();
        break;
      case reaction_period_press:
        // Yay, the player pressed the button in time
        reactionSuccess();
        break;
      default:
        ;
    }
  }
  
  // Check if we're supposed to go to reaction_period or not
  if ( currentGameState == pre_reaction )
  {
    if ( millis() - startReactionTime > reactionPeriod )
    {
      // Pre-reaction period is over, let's start the reaction period
      // Choose randomly if we get a "press" or "no press" reaction state
      if ( random(2) == 1 )
      {
        currentGameState = reaction_period_press;
        setRGBLed( BLUE_LIGHT );
      }
      else
      {
        currentGameState = reaction_period_nopress;
        setRGBLed( RED_LIGHT );
      }
      startReactionTime = millis();
      reactionPeriod = 1100 - (currentScore * 100);
    }
  }
  else if ( currentGameState == reaction_period_press )
  {
    // Is the reaction period over?
    if ( millis() - startReactionTime > reactionPeriod )
    {
      // Oh dear, the player wasn't fast enough on the button
      reactionFailed();
    }
  }
  else if ( currentGameState == reaction_period_nopress )
  {
    // Is the reaction period over?
    if ( millis() - startReactionTime > reactionPeriod )
    {
      // The player managed to not press the button, which is good
      setupPreReaction();
    }
  }

  // Store the last known button state
  lastButtonState = buttonState;

  // Small delay to prevent sporadic hits on a button press
  delay(30);
}
