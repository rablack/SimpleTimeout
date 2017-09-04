# Introduction
SimpleTimeout is a timeout library for Arduino.

It is designed to allow the creation of SimpleTimeout objects that tell you
when a time period has expired. These can be used to replace delay() with code
that allows other actions to take place whilst the software is waiting.

# Simple Example

    // Multi-blink. Flash LEDs at different rates.
    #include <SimpleTimeout.h>

    // Set the pins connected to the LEDs to blink
    constexpr int led1Pin = 12;
    constexpr int led2Pin = 13;

    // Define flash delays in milliseconds
    constexpr long led1Delay = 1000;
    constexpr long led2Delay = 620;

    bool led1High = false;
    bool led2High = false;

    // The timeouts default to a delay of 0 ms. They will timeout as soon
    // as loop() starts.
    SimpleTimeout timeout1;
    SimpleTimeout timeout2;

    void setup() {
      pinMode(led1Pin, OUTPUT);
      pinMode(led2Pin, OUTPUT);
    }

    void loop() {
      // LED 1: Check the state
      if (timeout1.expired()) {
        // Invert the state and update the LED
        led1High = !led1High;
        digitalWrite(led1Pin, led1High ? HIGH : LOW);

        // Start the timing again
        timeout1.restart(led1Delay);
      }

      // LED 2: Check the state
      if (timeout2.expired()) {
        // Invert the state and update the LED
        led2High = !led2High;
        digitalWrite(led2Pin, led2High ? HIGH : LOW);

        // Start the timing again
        timeout2.restart(led2Delay);
      }
    }

# A More Realistic Example

    #include <SimpleTimeout.h>

    ...

    void loop() {
      sendMessage();

      // Wait up to 1 second for a response
      SimpleTimeout timeout(1000);
      byte response;

      do {
        response = getResponse();
      } while (response == 0 && !timeout.expired());

      if (response != 0) {
        processResponse(response);
      } else {
        timeoutError();
      }
    }

# Method Summary

## SimpleTimeout(long duration = 0)
Create an object with a timeout starting now and expiring in duration clock
ticks. The default clock is milliseconds, but SimpleTimeoutMicros() gives you a
timeout in microseconds.

Durations of zero or negative numbers result in a timeout that has already
expired. SimpleTimeout::FOREVER is a duration (equal to LONG_MAX) that will
never expire.

## bool expired()
Has the timer expired yet? This deals with clock wraps.

Once the timeout has expired it will be considered expired forever.

## void restart()
Start the timeout again.

## void restart(long duration)
Start the timeout again with a different duration.

## void cancel()
Set the timeout to expired.

## unsigned long getStart()
Get the start time in the units of the current clock.

## unsigned long getEnd()
Get the end time in the units of the current clock. Note that if a clock wrap
will happen during the timeout, this will be lower in value than the start time.

## long getDuration()
Get the duration in the units of the current clock. If the timeout is forever,
this will return MAX_LONG.

## bool isForever()
Returns true if the timeout is forever (until a call to cancel() ).

## bool isDisabled()
Returns true if cancel() has been called or expired() has previously returned
true.
