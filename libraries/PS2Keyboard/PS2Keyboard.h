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
#define PS2_ENTER				13
#define PS2_BACKSPACE			127
#define PS2_ESC					3   // geaendert, eigentlich 27h
#define PS2_TAB					9
#define PS2_INSERT_AC1			5   // AC1=05h, CP/M=16h
#define PS2_INSERT_CPM			22
#define PS2_DELETE_AC1			19  // AC1=13h, CP/M=07h
#define PS2_DELETE_CPM			7
#define PS2_HOME_CPM			140 // CPM=^QS
#define PS2_END_CPM				141 // CPM=^QD
#define PS2_PAGEUP_AC1			21  // AC1=15h, CP/M=12h
#define PS2_PAGEUP_CPM			18
#define PS2_PAGEDOWN_AC1		17  // AC1=11h, CP/M=03h
#define PS2_PAGEDOWN_CPM		3
#define PS2_UPARROW_AC1			11  // AC1=0Bh, CP/M=05h
#define PS2_UPARROW_CPM			5
#define PS2_DOWNARROW_AC1		10  // AC1=0Ah, CP/M=18h
#define PS2_DOWNARROW_CPM		24
#define PS2_RIGHTARROW_AC1		9   // AC1=09h, CP/M=04h
#define PS2_RIGHTARROW_CPM		4
#define PS2_LEFTARROW_AC1		8   // AC1=08h, CP/M=08h
#define PS2_LEFTARROW_CPM		8
#define PS2_F1					128
#define PS2_F2					129
#define PS2_F3					130
#define PS2_F4					131
#define PS2_F5					132
#define PS2_F6					133
#define PS2_F7					134
#define PS2_F8					135
#define PS2_F9					136
#define PS2_F10					137
#define PS2_F11					138
#define PS2_F12					139
#define PS2_SCROLL				0
#define PS2_EURO_SIGN			0
#define PS2_SECTION_SIGN		0
#define PS2_DEGREE_SIGN			0
#define PS2_SUPERSCRIPT_TWO		0
#define PS2_SUPERSCRIPT_THREE	0
#define PS2_MICRO_SIGN			0
#define PS2_A_DIAERESIS			91  // Ä --> [
#define PS2_O_DIAERESIS			92  // Ö --> Backslash
#define PS2_U_DIAERESIS			93  // Ü --> ]
#define PS2_SHARP_S				126 // ß --> ~
#define PS2_a_DIAERESIS			123 // ä --> {
#define PS2_o_DIAERESIS			124 // ö --> |
#define PS2_u_DIAERESIS			125 // ü --> }


#define PS2_KEYMAP_SIZE 136

typedef struct {
	uint8_t noshift[PS2_KEYMAP_SIZE];
	uint8_t shift[PS2_KEYMAP_SIZE];
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
    static bool available(uint8_t kbd_mode);

    /**
	 * Discards any received data, sets available() to false without a call to read()
     */
    static void clear();

    /**
     * Returns the char last read from the keyboard.
     * If there is no char available, -1 is returned.
     */
    static int read(uint8_t kbd_mode);
};

#endif
