// PROJECT  :SerialMastermind
// PURPOSE  :SerialMastermind::Class implementation
// COURSE   :ICS3U
// AUTHOR   :C. D'Arcy
// DATE     :2019 12 07
// MCU      :328P (dual)
// STATUS   :Working
#include "Arduino.h"
#include <SoftwareSerial.h>
#include "SerialMastermind.h"
SoftwareSerial chat(SM_RX, SM_TX);

//Constructor options...
SerialMastermind::SerialMastermind() {
  helper();
  _solicit = true;
}

SerialMastermind::SerialMastermind(uint8_t num) {
  uint8_t number = num % 27;            //There are 3^3=27 possible codes....
  _color1 = _choices[number / 9];
  _color2 = _choices[(number % 9) / 3];
  _color3 = _choices[number % 3];
  helper();
}

SerialMastermind::SerialMastermind(char color1, char color2, char color3) {
  _color1 = color1 & ~32;   //map to uppercase...
  _color2 = color2 & ~32;   // "
  _color3 = color3 & ~32;   // "
  helper();
}

SerialMastermind::SerialMastermind(String str) {
  _color1 = str.charAt(0);
  _color2 = str.charAt(1);
  _color3 = str.charAt(2);
  helper();
}
void SerialMastermind::helper() {
  pinMode(SM_R1, OUTPUT);
  pinMode(SM_G1, OUTPUT);
  pinMode(SM_B1, OUTPUT);
  pinMode(SM_R2, OUTPUT);
  pinMode(SM_G2, OUTPUT);
  pinMode(SM_B2, OUTPUT);
  pinMode(SM_R3, OUTPUT);
  pinMode(SM_G3, OUTPUT);
  pinMode(SM_B3, OUTPUT);
  pinMode(winPin,OUTPUT);
  digitalWrite(winPin,LOW);
  allOff();
}

void SerialMastermind::showCode() {
  digitalWrite(SM_R1, _color1 == 'R');
  digitalWrite(SM_G1, _color1 == 'G');
  digitalWrite(SM_B1, _color1 == 'B');
  digitalWrite(SM_R2, _color2 == 'R');
  digitalWrite(SM_G2, _color2 == 'G');
  digitalWrite(SM_B2, _color2 == 'B');
  digitalWrite(SM_R3, _color3 == 'R');
  digitalWrite(SM_G3, _color3 == 'G');
  digitalWrite(SM_B3, _color3 == 'B');
  //set the number of each for later analysis
  for (uint8_t i = 0; i < 3; i++) {
    if (_color1 == _choices[i]) _numOfEach[i]++;
    if (_color2 == _choices[i]) _numOfEach[i]++;
    if (_color3 == _choices[i]) _numOfEach[i]++;
  }
  _code = String(_color1) + String(_color2) + String(_color3);
}

void SerialMastermind::begin(boolean instructions) {
  Serial.begin(9600);
  Serial.println("ACES Serial Mastermind Activity");
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  if (instructions)
    showInstructions();
  //This is probably how the game should work..
  //Users are required to submit their code to through the Serial input box...
  if (_solicit) {
    Serial.println("Enter your secret 3-character code (eg. RRB, gGB, BbB) then click Send...");
    Serial.println("(Note. An incorrect code entry will result in a random code chosen for you)");
    while (!Serial.available());
    _code = Serial.readString();      //grab what the user entered
    if (!valid()) {                   //Did the user enter exactly three characters?
      Serial.println("Invalid Code: " + _code);
      _setRandomCode = true;          //if not, it will generate a random code
    }
  }
}

boolean SerialMastermind::valid() {
  if (_code.length() != 4) return false;
  _color1 = _code.charAt(0) & ~32;
  _color2 = _code.charAt(1) & ~32;
  _color3 = _code.charAt(2) & ~32;
  if ( !((_color1 == 'R') | (_color1 == 'G') | (_color1 == 'B'))) return false;
  if ( !((_color2 == 'R') | (_color2 == 'G') | (_color2 == 'B'))) return false;
  if ( !((_color3 == 'R') | (_color3 == 'G') | (_color3 == 'B'))) return false;
  return true;
}

void SerialMastermind::playGame() {
  if (_setRandomCode) {
    randomSeed(analogRead(A5));
    uint8_t number = analogRead(A5) % 27;
    _color1 = _choices[number / 9];
    _color2 = _choices[(number % 9) / 3];
    _color3 = _choices[number % 3];
  }
  showCode();
  thinking();
  displaySummary();
  prompt();
  chat.begin(9600);
  while (true) {
    if (chat.available()) {  //anything incoming?
      _guess = chat.readString();
      uint8_t result = analysis();
      Serial.println("Confirming analysis of " + _code + " by " + _guess + " as " + String(result));
      chat.println(String(result/10)+String(result%10));
    }
    if (Serial.available())  {            //anything outgoing?
      _entry = Serial.readString();
      if (_entry.length() != 4)
        Serial.println("Incorrect length (3). Please try again.");
      else {
        _entry = _entry.substring(0, 3);
        _entry.toUpperCase();
        _guesses[_numGuesses] = _entry;
        Serial.println("Guess:\t" + _entry);
        chat.println(_entry);
        while (!chat.available());
        _response = chat.readString();
        _response = _response.substring(0, 3);
        //        Serial.println(_response.length());
        _responses[_numGuesses] = _response;
        _numGuesses++;
        displaySummary();
        if (_response.toInt() == 30){
          digitalWrite(winPin,HIGH);
          Serial.println("CORRECT!");
        }
        else
          prompt();
      }
    }
  }
}

void SerialMastermind::displaySummary() {
  Serial.println("#  Guess Response");
  for (uint8_t i = 0; i < 6; i++)
    Serial.println(String(i + 1) + ". " + _guesses[i] + "   " + _responses[i]);
}

void SerialMastermind::prompt() {
  Serial.println("Ready. Enter your 3-character guess and Send! " + _code);
}

void SerialMastermind::thinking() {
  uint64_t timeStamp = millis();
  while (millis() - timeStamp < 2000) {
    Serial.print('.'); delay(600);
  }
  Serial.println();
}

//Refer to https://en.wikipedia.org/wiki/Mastermind_(board_game)#Gameplay_and_rules
uint8_t SerialMastermind::analysis() {
  //Check for black (right colour and right position)
  uint8_t numBlack = 0;
  boolean matched[] = {false, false, false};
  for (uint8_t i = 0; i < 3; i++) {
    if (_code.charAt(i) == _guess.charAt(i)) {
      numBlack++;
      matched[i] = true;
    }
  }
  //check for white (correct color, wrong position)
  uint8_t numOfEachLeft[] = {0, 0, 0};
  uint8_t numOfEachRequest[] = {0, 0, 0};
  for (uint8_t i = 0; i < 3; i++) {
    if (!matched[i]) {
      if (_code.charAt(i) == 'R') numOfEachLeft[0]++;
      if (_code.charAt(i) == 'G') numOfEachLeft[1]++;
      if (_code.charAt(i) == 'B') numOfEachLeft[2]++;
      if (_guess.charAt(i) == 'R') numOfEachRequest[0]++;
      if (_guess.charAt(i) == 'G') numOfEachRequest[1]++;
      if (_guess.charAt(i) == 'B') numOfEachRequest[2]++;
    }
  }

  uint8_t numWhite = 0;
  Serial.println("Num of Each Left");
  for (uint8_t i = 0; i < 3; i++)
    Serial.print(numOfEachLeft[i]);
  Serial.println("\nNum of Each Request");
  for (uint8_t i = 0; i < 3; i++)
    Serial.print(numOfEachRequest[i]);
  Serial.println();
  if (numOfEachRequest[0] > 0) numWhite += min(numOfEachRequest[0], numOfEachLeft[0]);
  if (numOfEachRequest[1] > 0) numWhite += min(numOfEachRequest[1], numOfEachLeft[1]);
  if (numOfEachRequest[2] > 0) numWhite += min(numOfEachRequest[2], numOfEachLeft[2]);

  return numBlack * 10 + numWhite;
}

void SerialMastermind::showInstructions() {
  Serial.println("Instructions...");
  Serial.println("1. To win, you must crack your opponent's 3-colour RGB code");
  Serial.println("   faster than your opponent (maximum 6 attempts)");
  Serial.println("2. Accepted codes are RRR, rgR, Bgg, etc.");
  Serial.println("3. Two-digit response codes are to be interpreted as");
  Serial.println("      the number of CORRECT COLOURS in the CORRECT POSITION");
  Serial.println("   followed by,");
  Serial.println("      the number of CORRECT colours");
  Serial.println("4. A response of 30 signifies you've cracked your opponents code!");
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

uint8_t SerialMastermind::getRxPin() {
  return SM_RX;
}

void SerialMastermind::allOff() {
  digitalWrite(SM_R1, LOW);
  digitalWrite(SM_G1, LOW);
  digitalWrite(SM_B1, LOW);
  digitalWrite(SM_R2, LOW);
  digitalWrite(SM_G2, LOW);
  digitalWrite(SM_B2, LOW);
  digitalWrite(SM_R3, LOW);
  digitalWrite(SM_G3, LOW);
  digitalWrite(SM_B3, LOW);
}

char SerialMastermind::getColor(uint8_t i) {
  switch (i) {
    case 1: return _color1;
    case 2: return _color2;
    case 3: return _color3;
    default: return '?';
  }
}
