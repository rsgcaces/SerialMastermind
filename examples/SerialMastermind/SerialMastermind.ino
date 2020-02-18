// PROJECT  :SerialMasterind
// PURPOSE  :Sample client of SerialMastermind library to be used in conjunction with
//          :the ACESSerialMastermind appliance for one game of Mastermind:
//          :http://darcy.rsgc.on.ca/ACES/PCBs/index.html#Mastermind
// COURSE   :ICS3U/ICS4U
// AUTHOR   :C. D'Arcy
// DATE     :2019 12 07
// MCU      :328P
// STATUS   :Working
// REVISION HISTORY
// Version 1.1  a) Default constructor requests users enter secret code
//                 through the Serial Monitor text box
//              b) Added Winner LED to pin 13
// Version 1.0  a) Initial release
// REFERENCE:Serial Input Basics
//          :https://forum.arduino.cc/index.php?topic=288234.0
#include "SerialMastermind.h"
//User enters his secret code on the Serial Input box at RUN time
//Invalid codes result in a random selection by the application
SerialMastermind sm;
//Alternate Constructors (overloaded)
//'Developers' enter their code at COMPILE time in a variety of ways
//SerialMastermind sm(200);  //developer accepts random code from supplied number
//SerialMastermind sm('R', 'G', 'B');   //developer chooses code explicitly
//SerialMastermind sm("RGB");           //developer chooses code explicitly

void setup() {
    sm.begin(true);  //true for instructions...
    sm.playGame();   // nothing else to do; class handles all activity
}
void loop() {       
}
