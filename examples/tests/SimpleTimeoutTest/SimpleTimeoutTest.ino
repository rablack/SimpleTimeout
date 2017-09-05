#line 2 "SimpleTimeoutTest.ino"
// Tests for the SimpleTimeoutTemplate class
// normally instantiated with the millis() function
// TestClock::getTicks() will be our test function.

#include <ArduinoUnit.h>
#include <SimpleTimeout.h>

class TestClock {
 public:
  static unsigned long getTicks(void) {
    return ticks;
  }

  static void setTicks(unsigned long newTicks) {
    ticks = newTicks;
  }

  static void add(unsigned long addTicks) {
    ticks += addTicks;
  }

 private:
  static unsigned long ticks;
};

unsigned long TestClock::ticks = 0;

// Define the TestTimeout type to use our clock function.
typedef SimpleTimeoutTemplate<TestClock::getTicks> TestTimeout;

// A timeout with a negative value waits forever
test(forever)
{
  TestClock::setTicks(100);
  
  TestTimeout timeout(TestTimeout::FOREVER);

  assertTrue(timeout.isForever());
  assertEqual(TestClock::getTicks(), timeout.getStart());
  assertEqual(TestClock::getTicks(), timeout.getEnd());

  assertFalse(timeout.expired());
  TestClock::add(100);
  assertFalse(timeout.expired());
  TestClock::setTicks(0);
  assertFalse(timeout.expired());

  timeout.cancel();
  assertTrue(timeout.expired());
  assertFalse(timeout.isForever());

  timeout.restart();
  assertFalse(timeout.expired());
  assertTrue(timeout.isForever());  
}

// A timeout under normal conditions
test(normal_timeout)
{
  TestClock::setTicks(100);
  
  TestTimeout timeout(1000);

  // Now = 100
  // Expiry time = 100 + 1000 = 1100
  assertFalse(timeout.isForever());
  assertEqual(TestClock::getTicks(), timeout.getStart());
  assertEqual(1100, timeout.getEnd());
  assertFalse(timeout.expired());

  // One tick before expiry
  TestClock::setTicks(1099);
  assertFalse(timeout.expired());

  // Expiry time
  TestClock::setTicks(1100);
  assertTrue(timeout.expired());
}

// Test when the length of the timeout causes the millis() counter
// to overflow
test(timeout_overflow)
{
  TestClock::setTicks(ULONG_MAX - 100);

  TestTimeout timeout(1000);

  assertFalse(timeout.isForever());
  assertEqual(TestClock::getTicks(), timeout.getStart());
  assertEqual(899, timeout.getEnd());

  assertFalse(timeout.expired());
  TestClock::setTicks(ULONG_MAX);
  assertFalse(timeout.expired());
  TestClock::setTicks(0);
  assertFalse(timeout.expired());
  TestClock::setTicks(899);
  assertTrue(timeout.expired());
}

// Test when the clock overflows before the timeout is detected
test(clock_overflow_timeout)
{
  TestClock::setTicks(ULONG_MAX - 100);

  TestTimeout timeout(50);

  assertFalse(timeout.isForever());
  assertEqual(TestClock::getTicks(), timeout.getStart());
  assertEqual(ULONG_MAX - 50, timeout.getEnd());

  assertFalse(timeout.expired());
  TestClock::setTicks(ULONG_MAX); // Just before clock overflow, already timed out
  assertTrue(timeout.expired());
  TestClock::setTicks(timeout.getStart() - 1); // After clock overflow
  assertTrue(timeout.expired());
}

// Test when the timeout is zero. The timeout has always expired
test(zero_timeout)
{
  TestClock::setTicks(100);

  TestTimeout timeout(0);

  assertFalse(timeout.isForever());
  assertEqual(TestClock::getTicks(), timeout.getStart());
  assertEqual(TestClock::getTicks(), timeout.getEnd());

  assertTrue(timeout.expired());
  TestClock::setTicks(ULONG_MAX);
  assertTrue(timeout.expired());
  TestClock::setTicks(0);
  assertTrue(timeout.expired());
}

test(disabled_restart)
{
  TestClock::setTicks(100);
  
  TestTimeout timeout(1000);

  assertFalse(timeout.isForever());
  assertEqual(TestClock::getTicks(), timeout.getStart());
  assertEqual(1100, timeout.getEnd());
  assertFalse(timeout.expired());
  
  TestClock::setTicks(1099);
  assertFalse(timeout.expired());
  assertFalse(timeout.isDisabled());
  
  TestClock::setTicks(1101);
  assertTrue(timeout.expired());
  assertTrue(timeout.isDisabled());

  timeout.restart();
  assertFalse(timeout.expired());
  assertFalse(timeout.isDisabled());
  assertEqual(2101, timeout.getEnd());

  timeout.cancel();
  assertTrue(timeout.isDisabled());
  assertEqual(1101, timeout.getEnd());
  assertFalse(timeout.isForever());

  timeout.restart(TestTimeout::FOREVER);
  assertTrue(timeout.isForever());
}

void setup() {
  Serial.begin(9600);
  while (!Serial) { }
}

void loop() {
  Test::run();
}
