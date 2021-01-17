/*
  The MIT License (MIT)

  Copyright (c) 2021 razvanlevarda

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
  Main code by Thomas O Fredericks (tof@t-o-f.info)
  Previous contributions by Eric Lowry, Jim Schimpf and Tom Harkaway
  Modifications to add PCF8574 library compatability, made by Razvan Levarda
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Bounce2pcf.h"

#define DEBOUNCED_STATE 0
#define UNSTABLE_STATE  1
#define STATE_CHANGED   3

Bounce2pcf::Bounce2pcf()
    : previous_millis(0)
    , interval_millis(10)
    , state(0)
    , pin(0)
{}

void Bounce2pcf::attach(PCF8574* _pcfX, uint8_t pin) {
    pcfX = _pcfX;
    this->pin = pin;
    this->interval_millis = interval_millis;
    bool read = pcfX->read(pin);
    state = 0;
    if (pcfX->read(pin)) {
        state = _BV(DEBOUNCED_STATE) | _BV(UNSTABLE_STATE);
    }
#ifdef BOUNCE_LOCK_OUT
    previous_millis = 0;
#else
    previous_millis = millis();
#endif
}

void Bounce2pcf::interval(uint16_t interval_millis)
{
    this->interval_millis = interval_millis;
}

bool Bounce2pcf::update()
{
#ifdef BOUNCE_LOCK_OUT
    state &= ~_BV(STATE_CHANGED);
    if (millis() - previous_millis >= interval_millis) {
        bool currentState = pcfX->read(pin);
        if ((bool)(state & _BV(DEBOUNCED_STATE)) != currentState) {
            previous_millis = millis();
            state ^= _BV(DEBOUNCED_STATE);
            state |= _BV(STATE_CHANGED);
        }
    }
    return state & _BV(STATE_CHANGED);
#else
    bool currentState = pcfX->read(pin);
    state &= ~_BV(STATE_CHANGED);
	
    if ( currentState != (bool)(state & _BV(UNSTABLE_STATE)) ) {
        previous_millis = millis();
        state ^= _BV(UNSTABLE_STATE);
    } else
        if ( millis() - previous_millis >= interval_millis ) {
            if ((bool)(state & _BV(DEBOUNCED_STATE)) != currentState) {
                previous_millis = millis();
                state ^= _BV(DEBOUNCED_STATE);
                state |= _BV(STATE_CHANGED);
            }
        }

    return state & _BV(STATE_CHANGED);
#endif
}

bool Bounce2pcf::read()
{
    return state & _BV(DEBOUNCED_STATE);
}

bool Bounce2pcf::rose()
{
    return ( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}

bool Bounce2pcf::fell()
{
    return !( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}
