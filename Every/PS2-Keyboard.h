/*
  PS2Keyboard.h - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Mostly rewritten Paul Stoffregen <paul@pjrc.com>, June 2010
  ** Modified for use with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
  ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **

  *** Anpassungen für den AC1 von Gerd Staffen und Frieder Tonn, 2021

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

#ifndef PS2Keyboard_h
#define PS2Keyboard_h

#include "Arduino.h" // for attachInterrupt, FALLING

// Every call to read() returns a single byte for each
// keystroke.  These configure what byte will be returned
// for each "special" key.  To ignore a key, use zero.
#define PS2_ESC_AC1           3  // AC1 = 03h, CP/M = 1Bh
#define PS2_ESC_CPM          27
#define PS2_SHIFT_ESC        27  // Shift-ESC = 1BH
#define PS2_TAB            0x0F  // siehe config.h
#define PS2_SHIFT_TAB         9  // Shift-TAB = 09H
#define PS2_NUM_KOMMA      0x2E  // siehe config.h
#define PS2_ENTER            13
#define PS2_INSERT_AC1        5  // AC1 = 05h, CP/M = 16h
#define PS2_INSERT_CPM       22
#define PS2_DELETE_AC1        4  // AC1 = 04h, CP/M = 07h
#define PS2_DELETE_CPM        7
#define PS2_HOME_AC1          1
#define PS2_END_AC1           3
#define PS2_PAGEUP_AC1       21  // AC1 = 15h, CP/M = 12h
#define PS2_PAGEUP_CPM       18
#define PS2_PAGEDOWN_AC1     17  // AC1 = 11h, CP/M = 03h
#define PS2_PAGEDOWN_CPM      3
#define PS2_UPARROW_AC1      11  // AC1 = 0Bh, CP/M = 05h
#define PS2_UPARROW_CPM       5
#define PS2_DOWNARROW_AC1    10  // AC1 = 0Ah, CP/M = 18h
#define PS2_DOWNARROW_CPM    24
#define PS2_RIGHTARROW_AC1    9  // AC1 = 09h, CP/M = 04h
#define PS2_RIGHTARROW_CPM    4
#define PS2_LEFTARROW_AC1     8  // AC1 = 08h, CP/M = 13h
#define PS2_LEFTARROW_CPM    19
#define PS2_BACKSPACE_AC1   127  // AC1 = 7Fh, CP/M = 08h
#define PS2_BACKSPACE_CPM     8
#define PS2_SHIFT_BACKSPACE 127  // Shift-Backspace = 7FH
#define PS2_CTRL_BACKSPACE   31  // Ctrl-Backspace = 1FH
#define PS2_EURO_SIGN         0
#define PS2_SECTION_SIGN      0
#define PS2_DEGREE_SIGN       0
#define PS2_SUPERSCRIPT_TWO   0
#define PS2_SUPERSCRIPT_THREE 0
#define PS2_MICRO_SIGN        0
#define PS2_A_DIAERESIS      91  // Ä --> [
#define PS2_O_DIAERESIS      92  // Ö --> Backslash
#define PS2_U_DIAERESIS      93  // Ü --> ]
#define PS2_a_DIAERESIS     123  // ä --> {
#define PS2_o_DIAERESIS     124  // ö --> |
#define PS2_u_DIAERESIS     125  // ü --> }
#define PS2_SHARP_S         126  // ß --> ~

#define PS2_F1              128
#define PS2_F2              129
#define PS2_F3              130
#define PS2_F4              131
#define PS2_F5              132
#define PS2_F6              133
#define PS2_F7              134
#define PS2_F8              135
#define PS2_F9              136
#define PS2_F10             137
#define PS2_F11             138
#define PS2_F12             139
#define PS2_SHIFT_F1        140
#define PS2_SHIFT_F2        141
#define PS2_SHIFT_F3        142
#define PS2_SHIFT_F4        143
#define PS2_SHIFT_F5        144
#define PS2_SHIFT_F6        145
#define PS2_SHIFT_F7        146
#define PS2_SHIFT_F8        147
#define PS2_SHIFT_F9        148
#define PS2_SHIFT_F10       149
#define PS2_SHIFT_F11       150
#define PS2_SHIFT_F12       151
#define PS2_CTRL_F1         152
#define PS2_CTRL_F2         153
#define PS2_CTRL_F3         154
#define PS2_CTRL_F4         155
#define PS2_CTRL_F5         156
#define PS2_CTRL_F6         157
#define PS2_CTRL_F7         158
#define PS2_CTRL_F8         159
#define PS2_CTRL_F9         160
#define PS2_CTRL_F10        161
#define PS2_CTRL_F11        162
#define PS2_CTRL_F12        163
#define PS2_ALT_F1          164
#define PS2_ALT_F2          165
#define PS2_ALT_F3          166
#define PS2_ALT_F4          167
#define PS2_ALT_F5          168
#define PS2_ALT_F6          169
#define PS2_ALT_F7          170
#define PS2_ALT_F8          171
#define PS2_ALT_F9          172
#define PS2_ALT_F10         173
#define PS2_ALT_F11         174
#define PS2_ALT_F12         175
#define PS2_ALTGR_F1        176
#define PS2_ALTGR_F2        177
#define PS2_ALTGR_F3        178
#define PS2_ALTGR_F4        179
#define PS2_ALTGR_F5        180
#define PS2_ALTGR_F6        181
#define PS2_ALTGR_F7        182
#define PS2_ALTGR_F8        183
#define PS2_ALTGR_F9        184
#define PS2_ALTGR_F10       185
#define PS2_ALTGR_F11       186
#define PS2_ALTGR_F12       187

#define PS2_ALTGR_0         188  // AltGr+0 --> 40ms Impulse beim Tastendruck
#define PS2_ALTGR_1         189  // AltGr+1 --> PA7 = high, so lange die Taste gedrückt ist
#define PS2_HOME_CPM        190
#define PS2_END_CPM         191
#define PS2_CTRL_ESC        192  // NMI
#define PS2_ALT_ESC         193  // RESET
#define PS2_CAPS            194  // Caps-Lock
#define PS2_SCROLL          195  // Rollen
#define PS2_NUM             196  // NUM-Lock 
#define AFFENGRIFF          197  // CTRL-ALT-ENTF
#define PS2_ESC             198  // ESC
#define PS2_BACKSPACE       199  // Backspace
#define PS2_ALTGR_G         200  // AltGr+G - Umschaltung grafmode
#define PS2_ALTGR_T         201  // AltGr+T - Umschaltung kbd_mode

#define PS2_KEYMAP_SIZE 136

typedef struct {
  uint8_t noshift[PS2_KEYMAP_SIZE];
  uint8_t shift[PS2_KEYMAP_SIZE];
  uint8_t ctrl[PS2_KEYMAP_SIZE];
  uint8_t alt[PS2_KEYMAP_SIZE];
  uint8_t altgr[PS2_KEYMAP_SIZE];
} PS2Keymap_t;

extern const PROGMEM PS2Keymap_t PS2Keymap_German;

/**
 * Purpose: Provides an easy access to PS2 keyboards
 * Author:  Christian Weichel
 */
class PS2Keyboard {
  public:
    /**
     * This constructor does basically nothing. Please call the begin(int,int)
     * method before using any other method of this class.
     */
    PS2Keyboard();

    /**
     * Starts the keyboard "service" by registering the external interrupt.
     * setting the pin modes correctly and driving those needed to high.
     * The propably best place to call this method is in the setup routine.
     */
    static void begin(uint8_t dataPin, uint8_t irq_pin, const PS2Keymap_t &map = PS2Keymap_German);

    /**
     * Returns true if there is a char to be read, false if not.
     */
    static bool available(bool kbd_mode, bool cpm_mode);

    /**
     * Discards any received data, sets available() to false without a call to read()
     */
    static void clear();

    /**
     * Returns the char last read from the keyboard.
     * If there is no char available, -1 is returned.
     */
    static int read(bool kbd_mode, bool cpm_mode);
};

#endif
