// MultiBlink
//
// Blink multiple LEDs at frequencies that are not related
// to each other. This is a demonstration of using SimpleTimeout
// to provide a non-blocking delay.

#include <SimpleTimeout.h>

// Set the pins connected to the LEDs to blink
constexpr int led1Pin = 12;
constexpr int led2Pin = 13;

// Define flash delays in milliseconds
constexpr long led1Delay = 1000;
constexpr long led2Delay = 620;

bool led1High = false;
bool led2High = false;

// The delays default to 0 ms. They will timeout
// as soon as loop() checks expired().

SimpleTimeout delay1;
SimpleTimeout delay2;

void setup() {
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
}

void loop() {
  // LED 1: Check the state
  if (delay1.expired()) {
    // Invert the state and update the LED
    led1High = !led1High;
    digitalWrite(led1Pin, led1High ? HIGH : LOW);

    // Start the timing again
    delay1.restart(led1Delay);
  }

  // LED 2: Check the state
  if (delay2.expired()) {
    // Invert the state and update the LED
    led2High = !led2High;
    digitalWrite(led2Pin, led2High ? HIGH : LOW);

    // Start the timing again
    delay2.restart(led2Delay);
  }
}

