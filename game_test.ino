/*
  ESP32 Supermini code for Simon-Says project
  Coded by: Julius Yang
*/

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

uint8_t digitPins[] = {8, 9, 10, 11}; // 8-Q0, 9-Q2, 10-Q3, 11-Q4

// Array holds the binary sev seg representation of digits 0-9
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

enum Color pressed = RED;

//Going to need to make sure that we don't 
uint8_t colorArr [200]; // Initialize a list of 200 colors but don't set anything in there yet


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

  // Now setting up the game:
  randomSeed(analogRead(21));
  initGame();

  Serial.begin(115200);
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
      delay(300);
      
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
      delay(300);
      
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
      delay(300);
      
      
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
      delay(300);
      Serial.println("Red pressed!");
      
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

void initGame()
{
  delay(500);
  // Set all the digits in the display back to 0
  nums[0] = 0;
  nums[1] = 0;
  nums[2] = 0;
  nums[3] = 0;

  counter = 0;  // Reset the count of the game back to 0;
  numCorrect = 0;
  generateColor();
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

void playSequence()
{
  for(int i = 0; i < counter; i++)
  {
    
    switch (colorArr[i])
    {
      //GREEN
      case 0:
      {
        tone(buzzPin, 196);
        digitalWrite(greenLED, HIGH);
        delay(500);
        digitalWrite(greenLED, LOW);
        noTone(buzzPin);
        delay(200);
        break;
      }
      //YELLOW
      case 1:
      {
        tone(buzzPin, 250);
        digitalWrite(yellowLED, HIGH);
        delay(500);
        digitalWrite(yellowLED, LOW);
        noTone(buzzPin);
        delay(200);
        break;
      }
      //BLUE
      case 2:
      {
        tone(buzzPin, 300);
        digitalWrite(blueLED, HIGH);
        delay(500);
        digitalWrite(blueLED, LOW);
        noTone(buzzPin);
        delay(200);
        break;
      }
      //RED
      case 3:
      {
        tone(buzzPin, 350);
        digitalWrite(redLED, HIGH);
        delay(500);
        digitalWrite(redLED, LOW);
        noTone(buzzPin);
        delay(200);
        break;
      }
    }
  }
}

void btnCheck(enum Color c)
{
  if((Color) colorArr[numCorrect] == c)
  {
    
    numCorrect++;
    // Serial.print("numCorrect");
    // Serial.println(numCorrect);
    if(numCorrect == counter)
    {
      incrementNums();
      numCorrect = 0;
      generateColor();
      delay(300);
      playSequence();
    }
  }
  else
  {
    numCorrect = 0;
    // Serial.print("numCorrect");
    // Serial.println(numCorrect);
    initGame();
  }
    
}

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

void scanDigit(int index)
{
  registers = digits[nums[index - 1]] | bit(digitPins[index - 1]);
  writeRegisters();
}

void writeRegisters()
{
  // Set and display registers
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, highByte(registers));
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, lowByte(registers));
  digitalWrite(LATCH_PIN, HIGH);
}
