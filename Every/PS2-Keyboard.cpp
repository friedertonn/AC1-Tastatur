/*
  PS2Keyboard.cpp - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Mostly rewritten Paul Stoffregen <paul@pjrc.com> 2010, 2011
  ** Modified for use beginning with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
  ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **

  *** Anpassungen fuer den AC1 von Gerd Staffen und Frieder Tonn, 2021
  
  for more information you can read the original wiki in arduino.cc
  at http://www.arduino.cc/playground/Main/PS2Keyboard
  or http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html

  ******************************
  *                            *
  *  Anpassungen fuer den AC1  *
  *                            *
  ******************************
  - deutsche Tastatur
  - Gross- und Kleinbuchstaben getauscht, Umschalten mit Caps-Lock
  - F-Tasten belegt mit Code 128-187 (= 5 Ebenen)
  - weitere Funktionen ab Code 188
  - Affengriff hinzugefügt

  Version 2.4 (March 2013)
  - Support Teensy 3.0, Arduino Due, Arduino Leonardo & other boards
  - French keyboard layout, David Chochoi, tchoyyfr at yahoo dot fr

  Version 2.3 (October 2011)
  - Minor bugs fixed

  Version 2.2 (August 2011)
  - Support non-US keyboards - thanks to Rainer Bruch for a German keyboard :)

  Version 2.1 (May 2011)
  - timeout to recover from misaligned input
  - compatibility with Arduino "new-extension" branch
  - TODO: send function, proposed by Scott Penrose, scooterda at me dot com

  Version 2.0 (June 2010)
  - Buffering added, many scan codes can be captured without data loss
    if your sketch is busy doing other work
  - Shift keys supported, completely rewritten scan code to ascii
  - Slow linear search replaced with fast indexed table lookups
  - Support for Teensy, Arduino Mega, and Sanguino added

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "PS2-Keyboard.h"

#define BUFFER_SIZE 45
static volatile uint8_t buffer[BUFFER_SIZE];
static volatile uint8_t head, tail;
static uint8_t DataPin;
static uint8_t CharBuffer = 0;
static const PS2Keymap_t *keymap = NULL;

// The ISR for the external interrupt
void ps2interrupt(void)
{
  static uint8_t bitcount = 0;
  static uint8_t incoming = 0;
  static uint32_t prev_ms = 0;
  uint32_t now_ms;
  uint8_t n, val;

  val = digitalRead(DataPin);
  now_ms = millis();
  if (now_ms - prev_ms > 250) {
    bitcount = 0;
    incoming = 0;
  }
  prev_ms = now_ms;
  n = bitcount - 1;
  if (n <= 7) {
    incoming |= (val << n);
  }
  bitcount++;
  if (bitcount == 11) {
    uint8_t i = head + 1;
    if (i >= BUFFER_SIZE) i = 0;
    if (i != tail) {
      buffer[i] = incoming;
      head = i;
    }
    bitcount = 0;
    incoming = 0;
  }
}

static inline uint8_t get_scan_code(void)
{
  uint8_t c, i;

  i = tail;
  if (i == head) return 0;
  i++;
  if (i >= BUFFER_SIZE) i = 0;
  c = buffer[i];
  tail = i;
  return c;
}

// http://www.quadibloc.com/comp/scan.htm
// http://www.computer-engineering.org/ps2keyboard/scancodes2.html

// These arrays provide a simple key map, to turn scan codes into ISO8859
// output.  If a non-US keyboard is used, these may need to be modified
// for the desired output.

const PROGMEM PS2Keymap_t PS2Keymap_German = {   // AC1-Mode + Grossbuchstaben
  //
  // without shift
  //
  { 0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '^', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '1', 0,
  0, 0, 'Y', 'S', 'A', 'W', '2', 0,
  0, 'C', 'X', 'D', 'E', '4', '3', 0,
  0, ' ', 'V', 'F', 'T', 'R', '5', 0,
  0, 'N', 'B', 'H', 'G', 'Z', '6', 0,
  0, 0, 'M', 'J', 'U', '7', '8', 0,
  0, ',', 'K', 'I', 'O', '0', '9', 0,
  0, '.', '-', 'L', PS2_O_DIAERESIS, 'P', PS2_SHARP_S, 0,
  0, 0, PS2_A_DIAERESIS, 0, PS2_U_DIAERESIS, '\'', 0, 0,
  PS2_CAPS /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '+', 0, '#', 0, 0,
  0, '<', 0, 0, 0, 0, PS2_BACKSPACE, 0,
  0, '1', 0, '4', '7', 0, 0, 0,
  '0', PS2_NUM_KOMMA, '2', '5', '6', '8', PS2_ESC /*ESC*/, PS2_NUM /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 },
  //
  // with shift
  //
  { 0, PS2_SHIFT_F9, 0, PS2_SHIFT_F5, PS2_SHIFT_F3, PS2_SHIFT_F1, PS2_SHIFT_F2, PS2_SHIFT_F12,
  0, PS2_SHIFT_F10, PS2_SHIFT_F8, PS2_SHIFT_F6, PS2_SHIFT_F4, 0, PS2_DEGREE_SIGN, 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '!', 0,
  0, 0, 'y', 's', 'a', 'w', '"', 0,
  0, 'c', 'x', 'd', 'e', '$', PS2_SECTION_SIGN, 0,
  0, ' ', 'v', 'f', 't', 'r', '%', 0,
  0, 'n', 'b', 'h', 'g', 'z', '&', 0,
  0, 0, 'm', 'j', 'u', '/', '(', 0,
  0, ';', 'k', 'i', 'o', '=', ')', 0,
  0, ':', '_', 'l', PS2_o_DIAERESIS, 'p', '?', 0,
  0, 0, PS2_a_DIAERESIS, 0, PS2_u_DIAERESIS, '`', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '*', 0, '\'', 0, 0,
  0, '>', 0, 0, 0, 0, PS2_BACKSPACE, 0,
  0, '1', 0, '4', '7', 0, 0, 0,
  '0', PS2_NUM_KOMMA, '2', '5', '6', '8', PS2_SHIFT_ESC /*ESC*/, 0 /*NumLock*/,
  PS2_SHIFT_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_SHIFT_F7 },
  //
  // with ctrl
  //
  {0, PS2_CTRL_F9, 0, PS2_CTRL_F5, PS2_CTRL_F3, PS2_CTRL_F1, PS2_CTRL_F2, PS2_CTRL_F12,
  0, PS2_CTRL_F10, PS2_CTRL_F8, PS2_CTRL_F6, PS2_CTRL_F4, 0, 30, 0,
  0, 0, 0, 0, 0, 17, 0, 0,
  0, 0, 25, 19, 1, 23, 0, 0,
  0, 3, 24, 4, 5, 0, 0, 0,
  0, 0, 22, 6, 20, 18, 0, 0,
  0, 14, 2, 8, 7, 26, 0, 0,
  0, 0, 13, 10, 21, 0, 0, 0,
  0, 0, 11, 9, 15, 0, 0, 0,
  0, 0, 31, 12, 28, 16, 0, 0,
  0, 0, 27, 0, 29, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, PS2_CTRL_ESC, 0,
  PS2_CTRL_F11, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, PS2_CTRL_F7 },
  //
  // with alt
  //
  { 0, PS2_ALT_F9, 0, PS2_ALT_F5, PS2_ALT_F3, PS2_ALT_F1, PS2_ALT_F2, PS2_ALT_F12,
  0, PS2_ALT_F10, PS2_ALT_F8, PS2_ALT_F6, PS2_ALT_F4, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, PS2_ALT_ESC, 0,
  PS2_ALT_F11, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, PS2_ALT_F7 },
  //
  // with altgr
  //
  { 0, PS2_ALTGR_F9, 0, PS2_ALTGR_F5, PS2_ALTGR_F3, PS2_ALTGR_F1, PS2_ALTGR_F2, PS2_ALTGR_F12,
  0, PS2_ALTGR_F10, PS2_ALTGR_F8, PS2_ALTGR_F6, PS2_ALTGR_F4, 0, 0, 0,
  0, 0, 0, 0, 0, '@', 0, 0,
  0, 0, 0, 0, 0, 0, PS2_SUPERSCRIPT_TWO, 0,
  0, 0, 0, 0, PS2_EURO_SIGN, 0, PS2_SUPERSCRIPT_THREE, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, PS2_MICRO_SIGN, 0, 0, '{', '[', 0,
  0, 0, 0, 0, 0, '}', ']', 0,
  0, 0, 0, 0, 0, 0, '\\', 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, '~', 0, 0, 0, 0,
  0, '|', 0, 0, 0, 0, 0, 0,
  0, PS2_ALTGR_1, 0, 0, 0, 0, 0, 0,
  PS2_ALTGR_0, 0, 0, 0, 0, 0, 0, 0,
  PS2_ALTGR_F11, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, PS2_ALTGR_F7 }
};


#define BREAK     0x01
#define MODIFIER  0x02
#define SHIFT_L   0x04
#define SHIFT_R   0x08
#define CTRL      0x10
#define ALT       0x20
#define ALTGR     0x40

static char get_iso8859_code(bool kbd_mode, bool cpm_mode)
{
  static uint8_t state=0;
  uint8_t s;
  char c;

  while (1) {
    s = get_scan_code();
    if (!s) return 0;
    if (s == 0xF0) {         // Taste losgelassen - BREAK setzen
      state |= BREAK;
      if (kbd_mode) {  // bei einem Break-Code wird PA7 zurückgesetzt    
        digitalWrite(7, LOW);  // PA7 rücksetzen
        PORTB &= B11110000;    // gesetzte BITs B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
        PORTD &= B10001111;    // gesetzte BITs D4-D6 -> D4-D6  = PA4-PA6 rücksetzen
      }
    } else if (s == 0xE0) {  // Modifier-Code - MODIFIER setzen
      state |= MODIFIER;
    } else {
      if (state & BREAK) {  // wenn Break-Code gesetzt ist:
        if (s == 0x12) {
          state &= ~SHIFT_L;  // Shift_L ruecksetzen
        } else if (s == 0x59) {
          state &= ~SHIFT_R;  // Shift_R ruecksetzen
        } else if (s == 0x14) {
          state &= ~CTRL;     // Ctrl ruecksetzen
        } else if (s == 0x11 && !(state & MODIFIER)) {
          state &= ~ALT;      // Alt ruecksetzen
        } else if (s == 0x11 && (state & MODIFIER)) {
          state &= ~ALTGR;    // AltGr ruecksetzen
        }
        state &= ~(BREAK | MODIFIER);  // Break-Bit und Modifier-Bit ruecksetzen
        continue;
      }
      if (s == 0x12) {
        state |= SHIFT_L;  // Shift_L setzen
        continue;
      } else if (s == 0x59) {
        state |= SHIFT_R;  // Shift_R setzen
        continue;
      } else if (s == 0x14) {
        state |= CTRL;     // Ctrl setzen
        if (!(state & MODIFIER)) continue;  // jetzt geht auch die rechte Ctrl-Taste ;-)
      } else if (s == 0x11 && !(state & MODIFIER)) {
        state |= ALT;      // Alt setzen
        continue;
      } else if (s == 0x11 && (state & MODIFIER)) {
        state |= ALTGR;    // AltGr setzen
      }
      c = 0;
      if (state & MODIFIER) {  // Tastencodes, die mit 0xE0 beginnen
        switch (s) {
          case 0x70: if (!cpm_mode) c = PS2_INSERT_AC1;     else c = PS2_INSERT_CPM;     break;
          case 0x71: { if (state == (MODIFIER | CTRL | ALT)) c = AFFENGRIFF;
                     else if (!cpm_mode) c = PS2_DELETE_AC1; else c = PS2_DELETE_CPM; }  break;
          case 0x6C: if (!cpm_mode) c = PS2_HOME_AC1;       else c = PS2_HOME_CPM;       break;
          case 0x69: if (!cpm_mode) c = PS2_END_AC1;        else c = PS2_END_CPM;        break;
          case 0x7D: if (!cpm_mode) c = PS2_PAGEUP_AC1;     else c = PS2_PAGEUP_CPM;     break;
          case 0x7A: if (!cpm_mode) c = PS2_PAGEDOWN_AC1;   else c = PS2_PAGEDOWN_CPM;   break;
          case 0x75: if (!cpm_mode) c = PS2_UPARROW_AC1;    else c = PS2_UPARROW_CPM;    break;
          case 0x6B: if (!cpm_mode) c = PS2_LEFTARROW_AC1;  else c = PS2_LEFTARROW_CPM;  break;
          case 0x72: if (!cpm_mode) c = PS2_DOWNARROW_AC1;  else c = PS2_DOWNARROW_CPM;  break;
          case 0x74: if (!cpm_mode) c = PS2_RIGHTARROW_AC1; else c = PS2_RIGHTARROW_CPM; break;
          case 0x4A: c = '/';             break;  // "/" im Num-Block
          case 0x5A: c = PS2_ENTER;       break;  // "Enter" im Num-block
          default: break;
        }
      } else if (state & (SHIFT_L | SHIFT_R)) {
        if (s < PS2_KEYMAP_SIZE) c = pgm_read_byte(keymap->shift + s);
      } else if (state & CTRL) {
        if ((s == 0x71) && (state == (CTRL | ALT))) c = AFFENGRIFF;
        else if (s < PS2_KEYMAP_SIZE) c = pgm_read_byte(keymap->ctrl + s);
      } else if (state & ALT) {
        if (s < PS2_KEYMAP_SIZE) c = pgm_read_byte(keymap->alt + s);
      } else if (state & ALTGR) {
        if (s < PS2_KEYMAP_SIZE) c = pgm_read_byte(keymap->altgr + s);
      } else {
        if (s < PS2_KEYMAP_SIZE) c = pgm_read_byte(keymap->noshift + s);
      }
      state &= ~(BREAK | MODIFIER);  // Break-Bit und Modifier-Bit ruecksetzen
      if (c) return c;  // Funktion verlassen, wenn ein Keycode vorliegt. Ansonsten Endlosschleife.
    }
  }
}

bool PS2Keyboard::available(bool kbd_mode, bool cpm_mode) {
  if (CharBuffer) return true;
  CharBuffer = get_iso8859_code(kbd_mode, cpm_mode);
  if (CharBuffer) return true;
  return false;
}

void PS2Keyboard::clear() {
  CharBuffer = 0;
}

int PS2Keyboard::read(bool kbd_mode, bool cpm_mode) {
  uint8_t result;

  result = CharBuffer;
  if (result) {
    CharBuffer = 0;
  } 
  else {
    result = get_iso8859_code(kbd_mode, cpm_mode);
  }
  if (!result) return -1;
  return result;
}

PS2Keyboard::PS2Keyboard() {
  // nothing to do here, begin() does it all
}

void PS2Keyboard::begin(uint8_t data_pin, uint8_t irq_pin, const PS2Keymap_t &map) {
  DataPin = data_pin;
  keymap = &map;

  // initialize the pins
  pinMode(irq_pin, INPUT);
  digitalWrite(irq_pin, HIGH);
  pinMode(data_pin, INPUT);
  digitalWrite(data_pin, HIGH);

  head = 0;
  tail = 0;
  attachInterrupt(digitalPinToInterrupt(irq_pin), ps2interrupt, FALLING);
}
