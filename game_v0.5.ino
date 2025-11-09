/*
  File name: game_v0.5.ino
  Author: Julius Yang
  Date: 11-9-2025
  Utilizing code and resouces by:
  - Digital Clock using Franzininho, 74HC595, 7 Segment Display (4 digits) and DS1307 RTC
  - Uri Shaked
  - Anderson Costa
  - Visit https://wokwi.com to learn about the Wokwi Simulator
  - Visit https://franzininho.com.br to learn about the Franzininho
*/

// Define pins of the 7 segement display
#define DATA_PIN    2    // Pin connected to DS of 74HC595
#define LATCH_PIN   3    // Pin connected to STCP of 74HC595
#define CLOCK_PIN   4    // Pin connected to SHCP of 74HC595

// Button Pins
const int blueBtn = 5;
const int greenBtn = 8;
const int yellowBtn = 10;
const int redBtn = 0; //TX pin

// LED Pins:
const int blueLED = 6;
const int greenLED = 7;
const int yellowLED = 9;
const int redLED = 20; //RX pin

// Buzzer:
const int buzzPin = 1;

uint8_t digitPins[] = {8, 9, 10, 11}; // 8-Q0, 9-Q2, 10-Q3, 11-Q4

// Variables to hold last states of buttons:
int lastBState = LOW;
int lastGState = LOW;
int lastYState = LOW;
int lastRState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int counter = 0;
int numCorrect = 0;

// This is to hold the bits that will be shifted into the registers
// 2 registers that each hold a byte so 16bits total
uint16_t registers = 0;

// nums is used to tell the programs which numbers to display
// index 0 is for the first digit, and index 3 is the 4th digit
uint8_t nums[4] = {0,0,0,0};

// Array holds the binary 7 seg representation of digits 0-9
uint16_t digits[] = {
  0b11000000,
  0b11111001,
  0b10100100,
  0b10110000,
  0b10011001,
  0b10010010,
  0b10000010,
  0b11111000,
  0b10000000,
  0b10010000,
};

enum Color {
  GREEN,
  YELLOW,
  BLUE,
  RED
};

uint8_t colorArr [200]; // Initialize an array of 200 colors

enum Color pressed = RED;

// Start of functional code
void setup() {
  // Set display pins to output
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  // Set LED pins to output:
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // Set Button pins to input:
  pinMode(blueBtn, INPUT);
  pinMode(greenBtn, INPUT);
  pinMode(yellowBtn, INPUT);
  pinMode(redBtn, INPUT);

  // Buzzer to Output:
  pinMode(buzzPin, OUTPUT);

  // Initialize Serial Communication:
  Serial.begin(115200);

  // Now setting up the game:
  randomSeed(analogRead(21));
  initGame();
}

void loop() {

  // we have to scan the digits constantly. Whenever the esp is running.
  scanDigit(1);
  scanDigit(2);
  scanDigit(3);
  scanDigit(4);

  // read the state of the buttons
  int bBtnState = digitalRead(blueBtn);
  int gBtnState = digitalRead(greenBtn);
  int yBtnState = digitalRead(yellowBtn);
  int rBtnState = digitalRead(redBtn);

  // For debouncing, make sure that a button was pressed:
  if(bBtnState != lastBState || gBtnState != lastGState || yBtnState != lastYState || rBtnState != lastRState)
  {
    // Check to see what buttons were pressed:
    if(lastBState == HIGH)
    {
      pressed = BLUE;
      digitalWrite(blueLED, LOW);
      btnCheck(pressed);
    }
    else if(lastGState == HIGH)
    {
      pressed = GREEN;
      digitalWrite(greenLED, LOW);
      btnCheck(pressed);
    }
    else if(lastYState == HIGH)
    {
      pressed = YELLOW;
      digitalWrite(yellowLED, LOW);
      btnCheck(pressed);
    }
    else if(lastRState == HIGH)
    {
      pressed = RED;
      digitalWrite(redLED, LOW);
      btnCheck(pressed);
    }

    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if((millis() - lastDebounceTime) > debounceDelay)
{
    // If the green button was pressed
    if(gBtnState == HIGH)
    {
        tone(buzzPin, 196);
        digitalWrite(greenLED, HIGH);
        altDelay(300);
        noTone(buzzPin);
      
    }
    else
    {
        noTone(buzzPin);
        digitalWrite(greenLED, LOW);
    }

    // If the yellow button was pressed
    if(yBtnState == HIGH)
    {
        tone(buzzPin, 250);
        digitalWrite(yellowLED, HIGH);
        altDelay(300);
        noTone(buzzPin);
      
    }
    else
    {
        noTone(buzzPin);
        digitalWrite(yellowLED, LOW);
    }

    // If the blue button was pressed
    if(bBtnState == HIGH)
    {
        tone(buzzPin, 300);
        digitalWrite(blueLED, HIGH);
        altDelay(300);
        noTone(buzzPin);
    }
    else
    {
        noTone(buzzPin);
        digitalWrite(blueLED, LOW);
    }

    // If the red button was pressed
    if(rBtnState == HIGH)
    {
        tone(buzzPin, 350);
        digitalWrite(redLED, HIGH);
        altDelay(300);
        noTone(buzzPin);
    }
    else
    {
        noTone(buzzPin);
        digitalWrite(redLED, LOW);
    }
}

  // save the reading:
  lastBState = bBtnState;
  lastGState = gBtnState;
  lastYState = yBtnState;
  lastRState = rBtnState;
}

/*
initGame is responsible for setting up the game at the start
and after the game is determined to be over

Input: None.
Output: void. Resets and adds a color for the new sequence
*/
void initGame()
{
  altDelay(500);
  // Set all the digits in the display back to 0
  nums[0] = 0;
  nums[1] = 0;
  nums[2] = 0;
  nums[3] = 0;

  counter = 0;  // Reset the count of the game back to 0;
  numCorrect = 0;
  generateColor();
  altDelay(600);
  playSequence();
}

void generateColor()
{
  // "adds" a new color to choose in the list
  // A color is an enum from 0 to 3
  if(counter < 200)
  {
    colorArr[counter] = (int) random(0,4);
    counter++;
  }
}

/*
playSequence loops through the colorArr array and lights up the corresponding LED and
plays the corresponding tone of the color that was stored.

Input: None.
Output: Lights LED and plays tone for colors in the colorArr.
*/
void playSequence()
{
  noTone(buzzPin);
  // go through the colors we have added to the array and display them to the player
  for(int i = 0; i < counter; i++)
  {
    scanDigit(1);
    scanDigit(2);
    scanDigit(3);
    scanDigit(4);
    
    switch (colorArr[i])
    {
      //GREEN
      case 0:
      {
        tone(buzzPin, 196);
        digitalWrite(greenLED, HIGH);
        altDelay(500);
        digitalWrite(greenLED, LOW);
        noTone(buzzPin);
        altDelay(200);
        break;
      }
      //YELLOW
      case 1:
      {
        tone(buzzPin, 250);
        digitalWrite(yellowLED, HIGH);
        altDelay(500);
        digitalWrite(yellowLED, LOW);
        noTone(buzzPin);
        altDelay(200);
        break;
      }
      //BLUE
      case 2:
      {
        tone(buzzPin, 300);
        digitalWrite(blueLED, HIGH);
        altDelay(500);
        digitalWrite(blueLED, LOW);
        noTone(buzzPin);
        altDelay(200);
        break;
      }
      //RED
      case 3:
      {
        tone(buzzPin, 350);
        digitalWrite(redLED, HIGH);
        altDelay(500);
        digitalWrite(redLED, LOW);
        noTone(buzzPin);
        altDelay(200);
        break;
      }
    }
  }
}

/*
btnCheck checks to see if the button that was pressed was the correct
one in the sequence

Input: Color enum of the button that was pressed
Output: Void. Either increments counter or ends the game
*/
void btnCheck(enum Color c)
{
  if((Color) colorArr[numCorrect] == c)
  {
    
    numCorrect++;
    if(numCorrect == counter)
    {
      incrementNums();
      numCorrect = 0;
      generateColor();
      altDelay(600);
      playSequence();
    }
  }
  else
  {
    numCorrect = 0;
    gameOver();
    initGame();
  }
    
}

/*
incrementNums is responsible for incrementing the count on the 7 segment display

Input: None
Output: Void. 7 segment display will show a new count representing
the score of the player
*/
void incrementNums()
{
  for(int i = 3; i >= 1; i--)
  {
    int digit = nums[i];
    if(digit + 1 > 9)
    {
      nums[i] = 0;
      nums[i-1]++;
      break;
    }
    else
    {
      nums[i]++;
      break;
    }
      
  }
}

/*
scanDigit will write the registers to update a digit on the display

Input: int index. The index of the digit to update.
Output: Void. 7 segment display will have its digit updated
*/
void scanDigit(int index)
{
  registers = digits[nums[index - 1]] | bit(digitPins[index - 1]);
  writeRegisters();
}

/*
writeRegisters sets and shifts into the registers of the 7 segment display
the bits that will determine what is shown for the whole display.

Input: None
Output: void. Whole display gets updated with new bits from registers
*/
void writeRegisters()
{
  // Set and display registers
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, highByte(registers));
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, lowByte(registers));
  digitalWrite(LATCH_PIN, HIGH);
}

/*
altDelay updates the display while we are waiting for LEDs to blink and such.
Ensures that the display runs smoothly.

Input: unsigned long ms. The number of ms to wait before updating digits.
Output: void. All 4 digits are scanned
*/
void altDelay(unsigned long ms)
{
  unsigned long current = millis();
  while(millis() < current + ms)
  {
    scanDigit(1);
    scanDigit(2);
    scanDigit(3);
    scanDigit(4);
  }
}

/*
gameOver plays the jingle to indicate that the game is over. Triggered
when the player messes up a sequence.

Input: None.
Output: void. Game Over jingle is played.
*/
void gameOver()
{
  tone(buzzPin, 200);
  altDelay(300);
  tone(buzzPin, 170);
  altDelay(300);
  tone(buzzPin, 140);
  altDelay(300);
  tone(buzzPin, 110);
  altDelay(500);
  noTone(buzzPin);
  altDelay(300);
}
