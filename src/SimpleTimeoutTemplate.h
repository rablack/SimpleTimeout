// SimpleTimeoutTemplate class for creating simple timeouts and delays


// MIT License
//
// Copyright (c) 2017 Robert Black
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.



// Typical Usage:
//   SimpleTimeout timeout(ms); // Timeout length in milliseconds
//
//   for (i = 0; i < count; i++) {
//     while (!timeout.expired() && !finished) {
//       ... // Do something
//     }
//     timeout.restart();
//   }
// The ms parameter is signed. However, negative values result
// in undefined behavior.
//
// If ms is set to FOREVER (or LONG_MAX) the timeout will only expire
// when cancelled.
//
// Note that the SimpleTimeout implementation on the Arduino uses millis()

#ifndef SIMPLETIMEOUTTEMPLATE_H
#define SIMPLETIMEOUTTEMPLATE_H

#include <Arduino.h>
#include <limits.h>

template <unsigned long(*T_TICKS)(void)>
class SimpleTimeoutTemplate {
 public:
  static constexpr long FOREVER = LONG_MAX;
  
  // Create an object with a timeout starting now and expiring in durationTicks
  // clock ticks. The SimpleTimeout instance works in milliseconds, but
  // SimpleTimeoutMicros() gives you a timeout in microseconds.
  //
  // Durations of zero or negative numbers result in a timeout that has already
  // expired. SimpleTimeout::FOREVER is a duration (equal to LONG_MAX) that
  // will never expire.
  SimpleTimeoutTemplate(long durationTicks = 0) {
    start = T_TICKS();
    duration = durationTicks;
  }

  // Start the timeout again.
  void restart(void) {
    restart(getDuration());
  }
  
  // Start the timeout again with a different duration.
  void restart(long newDurationTicks) {
    duration = newDurationTicks;
    start = T_TICKS();
  }

  // Has the timer expired yet?
  //
  // This deals with clock wraps as long as the
  // clock doesn't cycle all the way back to the timeout start time before you
  // call expired(). Once the timeout has been detected by expired() it
  // will be reported as expired forever.
  bool expired(void) {
    if (isForever()) {
      return false;
    }
    
    if (isDisabled()) {
      return true;
    }

    unsigned long now = T_TICKS();
    unsigned long end = getEnd();

    // Deal with timeout overflow without a clock overflow
    if (end < start && now >= start) {
      return false;
    }

    // hasExpired = true for clock overflow without a timeout overflow
    bool hasExpired = (end >= start && now < start);

    if (!hasExpired) {  // Both overflowed or neither
      hasExpired = now >= end;
    }
    
    if (hasExpired) {
      // Once we have expired, cancel the timer to prevent clock-wrap
      // from making it un-expire at some point in the future.
      cancel();
    }
    
    return hasExpired;
  }

  inline unsigned long getStart(void) const {
    return start;
  }

  inline unsigned long getEnd(void) const {
    if (isDisabled() || isForever()) {
      return start;
    }
    return start + duration;
  }
  
  inline long getDuration(void) const {
    if (isDisabled()) {
      return getInvertedDuration();
    }
    return duration;
  }

  inline bool isForever(void) const {
    return duration == FOREVER;
  }
  
  // Has the timeout been cancelled or detected as expired?
  inline bool isDisabled(void) const {
    return duration < 0;
  }
  
  // Set the timeout to expired.
  inline void cancel(void) {
    if (!isDisabled()) {
      duration = getInvertedDuration();
    }
  }


 protected:
  // The current implementation uses the sign bit to indicate that that the
  // timeout has been cancelled or expired. Since the bit storage of
  // negative numbers is implementation-defined and we want to be able to
  // flag a timeout of zero, we are using an arithmetic of one's complement. 
  inline long getInvertedDuration() const {
    return -1 - duration;
  }


 private:
  unsigned long start;
  long duration;
};  // class SimpleTimeoutTemplate

#endif  // SIMPLETIMEOUTTEMPLATE_H
