#ifndef SerialMastermind_h
#define SerialMastermind_h
#define winPin 13
#define SM_R1 12
#define SM_G1 11
#define SM_B1 10
#define SM_R2 9
#define SM_TX 8   //7 and 8 appear to be good choices as they avoid all 3 Timers (PWM)
#define SM_RX 7   //
#define SM_G2 6
#define SM_B2 5
#define SM_R3 4
#define SM_G3 3
#define SM_B3 2

class SerialMastermind {
  public:
    SerialMastermind();
    SerialMastermind(uint8_t num);
    SerialMastermind(char color1, char color2, char color3);
    SerialMastermind(String str);
    void begin(boolean instructions);
    uint8_t getActive();
    uint8_t getRxPin();
    void allOff();
    char getColor(uint8_t i);
    void guess();
    void setCode();
    void playGame();
  private:
    char _color1;
    char _color2;
    char _color3;
    String _code = "RRR";
    boolean _setRandomCode = false;
    uint8_t _numGuesses = 0;
    boolean _solicit = false;
    char _choices[3] = {'R', 'G', 'B'};
    String _entry;
    String _request;
    String _response;
    String _guess;
    uint8_t _numOfEach[3] = {0, 0, 0};
    String _guesses[6] = {"---", "---", "---", "---", "---", "---"};
    String _responses[6] = {"--", "--", "--", "--", "--", "--"};
    //functions
    void helper();
    uint8_t analysis();
    void showCode();
    void thinking();
    void showInstructions();
    void prompt();
    void displaySummary();
    boolean valid();
};

#endif
