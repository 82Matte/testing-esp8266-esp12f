/*
  fade PWM pins
  Example for HP http://werner.rothschopf.net/microcontroller/202304_millis_dimmer_fading_led.htm

  inspired by https://forum.arduino.cc/t/1-button-1-led-fade/1113139/4 (code in forum)
  2023-04-10 by noiasca 
*/
#include <Arduino.h>
// a class for one button, one LED, one Relay
class Dimmer {
    const uint8_t buttonPin;         // the input GPIO, active LOW
    const uint8_t ledPin;            // the output GPIO, active HIGH, must be a PWM pin
    uint8_t brightness = 0;          // current PWM for output pin
    uint32_t previousMillis = 0;     // time management
    enum State {
      IDLE,                          // wait for button press
      DIMMUP,                        // push and hold button results in fade in (stop at max)
      HOLD,                          // stay in setted brightness
      DIMMDOWN                       // press button second time results in fade out (stop at light off)
    } state;
    const uint8_t intervalUp = 50;   // up interval / debounce button
    const uint8_t intervalDown = 10;

  public:
    Dimmer(uint8_t buttonPin, uint8_t ledPin) : buttonPin{buttonPin}, ledPin{ledPin}
    {}

    void begin() {                         // to be called in setup()
      pinMode(buttonPin, INPUT_PULLUP);
      pinMode(ledPin, OUTPUT);
    }

    // just a function to print debug information to Serial
    void debug() {
      Serial.print(ledPin); 
      Serial.print(F("\t")); 
      Serial.print(state); 
      Serial.print(F("\t")); 
      Serial.println(brightness);
    }

    void update(uint32_t currentMillis = millis()) {   // to be called in loop()
      debug();
      uint8_t buttonState = digitalRead(buttonPin);
      switch (state) {
        case State::IDLE :
          if (buttonState == LOW) {
            previousMillis = currentMillis;
            brightness = 1;
            analogWrite(ledPin, brightness);
            debug();
            state = State::DIMMUP;             // switch to next state
          }
          break;
        case State::DIMMUP :
          if (currentMillis - previousMillis > intervalUp) {
            if (buttonState == LOW) {
              previousMillis = currentMillis;
              if (brightness < 255) brightness++;  // increase if possible
              debug();
              analogWrite(ledPin, brightness);
            }
            else {
              state = State::HOLD;                // button isn't pressed any more - goto to next state
            }
          }
          break;
        case State::HOLD :
          if (currentMillis - previousMillis > intervalDown) {
            if (buttonState == LOW) {
              previousMillis = currentMillis;
              if (brightness > 0) brightness--;  // decrease if possible
              analogWrite(ledPin, brightness);
              debug();
              state = DIMMDOWN;                  // goto next state
            }
          }
          break;
        case State::DIMMDOWN :
          if (currentMillis - previousMillis > intervalDown) {
            previousMillis = currentMillis;
            if (brightness > 0) brightness--;
            analogWrite(ledPin, brightness);
            debug();
            //if (brightness == 0) state = State::IDLE;  // go back to IDLE
            if (brightness == 0 && buttonState != LOW) state = State::IDLE;  // go back to IDLE - alternative if you don't want to restart in case button is still pressed.
          }
          break;
      }
    }
};

//create  instances (each with one button and one led/LED)
Dimmer dimmer[] {
  {D1, D2},  // buttonPin, ledPin
};

void setup() {
  Serial.begin(9600);
  for (auto &i : dimmer) i.begin();  // call begin for all instances
  Serial.println("sdlkasjdlkjasl");
}

void loop() {
  for (auto &i : dimmer) i.update(); // call update() for all instances
}


