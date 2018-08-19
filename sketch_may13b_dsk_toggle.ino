

#define BTN_PIN 14 
#define LED_PIN 15 

#define TO_PC_REQUEST_TRANSITION 1 

#define FROM_PC_KEY_ON 1 
#define FROM_PC_KEY_OFF 2 

// To prevent bounce, we use timings of some milliseconds: 
#define SHORTEST_ALLOWED_TIME_BETWEEN_PRESSES 256000 
#define LONGEST_TIME_BEFORE_WRAP_PROTECTION 1073741824 

#ifndef TRUE 
#define TRUE 1 
#endif 

#ifndef FALSE 
#define FALSE 0 
#endif 

void setup () {
  pinMode (BTN_PIN, INPUT_PULLUP); 
  pinMode (LED_PIN, OUTPUT); 
  Serial.begin (9600); 
}

void loop () { 
  static byte live = FALSE; 
  if (Serial.available () > 0) { 
    // Computer sends us a byte whenever the key status changes; we then just update whether the LED should be on or off ... 
    byte cmd = Serial.read (); 
    live = cmd == FROM_PC_KEY_ON ? TRUE : FALSE; 
    digitalWriteFast (LED_PIN, live); 
  }
  static int prevBtnState = 0; 
  static unsigned long prevBtnPressTime = micros () - SHORTEST_ALLOWED_TIME_BETWEEN_PRESSES - 1; // Will always start with a clock number that's big enough for the next press to be allowed now. 
  // Time: 
  unsigned long currentTime = micros (); 
  unsigned long deltaT = currentTime - prevBtnPressTime; // How long ago was the button last pressed? 
  if (deltaT > LONGEST_TIME_BEFORE_WRAP_PROTECTION) { 
    // In case deltaT itself wraps around, we need to prevent the bug of it won't do anything on rare occasion of exact wrap and button-press time coincidence ... 
    prevBtnPressTime = currentTime - SHORTEST_ALLOWED_TIME_BETWEEN_PRESSES - 1; // Restart from the minimum button-press separation time ... 
  }
  // Button pressed check: 
  int btnPressed = !digitalReadFast (BTN_PIN); 
  if (!btnPressed || prevBtnState) { 
    prevBtnState = btnPressed; 
    return; // There's nothing else left to do ... 
  } 
  // Button went from UP to DOWN! 
  if (deltaT >= SHORTEST_ALLOWED_TIME_BETWEEN_PRESSES) { 
    Serial.write (TO_PC_REQUEST_TRANSITION); 
    prevBtnPressTime = currentTime; 
  } 
  prevBtnState = btnPressed; 
}


