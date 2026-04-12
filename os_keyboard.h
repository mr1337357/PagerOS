#ifndef __OS_KEYBOARD_H__
#define __OS_KEYBOARD_H__

#include <Adafruit_TCA8418.h>

extern Adafruit_TCA8418 keypad;

void keyboard_interrupt();

void keyboard_init();

void keyboard_handle_event();

void wheel_handle_event();

#endif