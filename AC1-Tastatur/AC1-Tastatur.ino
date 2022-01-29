/* PS/2-Keyboard + Joystick fuer den AC1
 * Das Programm wurde von Gerd Staffen und Frieder Tonn erstellt.
 * Es nutzt die Arduino-Bibliothek "PS2KeyAdvanced" von Paul Carpenter,
 * Quellcode: https://github.com/techpaul/PS2KeyAdvanced
 * 
 * Die Konfiguration der Tastatur (Verhalten beim Einschalten, Belegung der 
 * Funktionstasten, Joystick-Tasterbelegungen, Zeitschleifen) erfolgt in der 
 * Datei config.h
 */

// #define Debugging 1

#include "PS2KeyAdvanced.h"
#include "config.h"

const int DataPin = 2;    // D2  - PS/2-Data
const int IRQpin  = 3;    // D3  - PS/2-CLK
const int GrafPin = 13;   // D13 - SCCH-Grafiktaste --> PIO-B2
const int RESETpin = 12;  // D12 - AC1-RESET
const int NMIpin = 19;    // D19 - AC1-NMI

bool kbd_mode;  // Keyboard-Mode: true => PA7=HIGH, bis Taste losgelassen wird; false => 40ms-Impuls auf PA7  
bool capslock;  // Flag fuer die Caps-Lock Taste
bool cpm_mode;  // CP/M-Mode: false => AC1-Tastenbelegung; true => CP/M-Tastenbelegung 
bool grafmode;  // SCCH-Grafikmodus --> PIO B2 
byte joy_mode;  // Joystick-Mode
byte joy_code;  // aktueller Joystick-Code

PS2KeyAdvanced keyboard;

void anzeige_tastencode(byte code)   // Anzeige des Tastencodes auf dem seriellen Monitor
{
  if ( code < 1 || code > 127) return;
  if (code == 13) Serial.println("[Enter]");
  else if (code == 127) Serial.print("[DEL]");
  else if (code >= 32 && code < 127) Serial.print(char(code));
  else {
    Serial.print(char(94));
    Serial.print(char(code + 0x40));
  }
}

void parallelausgabe_PA7(byte code)   // AC1 ParallelZeichencode PA0-PA7 Original-AC1-Tastatur
{
  byte bit0bis3;     // PortB -> B0-B3 -> D8-D11 = PA0-PA3
  byte bit4bis6;     // PortD -> D4-D6 -> D4-D6  = PA4-PA6
  byte TASTE = 7;    // PortD -> D7    -> D7     = PA7

  // Caps-Lock:
  if (!capslock) {
    if (code >= 'A' && code <= 'Z') code += 32;
    else if (code >= 'a' && code <= 'z') code -= 32;
  }

  anzeige_tastencode(code);
  
  // Ausgabe des Tastencodes auf D4 bis D11:
  bit0bis3 = code & B00001111;  // PortB -> B0-B3 -> D8-D11 = PA0-PA3
  PORTB |= bit0bis3;            // benoetigte BITs B0-B3 -> D8-D11 = PA0-PA3 setzen 
  bit4bis6 = code & B01110000;  // PortD -> D4-D6 -> D4-D6  = PA4-PA6
  PORTD |= bit4bis6;            // benoetigte BITs D4-D6 -> D4-D6  = PA4-PA6 setzen
  digitalWrite(TASTE, HIGH);    // Ausgabe gültig -> PA7 = HIGH
  delay(40);                    // 40ms warten
  // das Rücksetzen von PA7 erfolgt beim nächsten BREAK-Code mit parallelausgabe_PA7_BREAK();
}

void parallelausgabe_PA7_BREAK()  // bei einem BREAK-Code wird PA7 zurückgesetzt 
{
  digitalWrite(7, LOW);  // PA7 rücksetzen
  PORTB &= B11110000;    // gesetzte BITs B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
  PORTD &= B10001111;    // gesetzte BITs D4-D6 -> D4-D6  = PA4-PA6 rücksetzen
}

void parallelausgabe_40ms(byte code, bool use_capslock)   // AC1 ParallelZeichencode PA0-PA7 40ms + 40ms Pause
{
  byte bit0bis3;     // PortB -> B0-B3 -> D8-D11 = PA0-PA3
  byte bit4bis6;     // PortD -> D4-D6 -> D4-D6  = PA4-PA6
  byte TASTE = 7;    // PortD -> D7    -> D7     = PA7

  // Caps-Lock:
  if ((!capslock) && (use_capslock)) {
    if (code >= 'A' && code <= 'Z') code += 32;
    else if (code >= 'a' && code <= 'z') code -= 32;
  }

  anzeige_tastencode(code);

  // Ausgabe des Tastencodes auf D4 bis D11:
  bit0bis3 = code & B00001111;  // PortB -> B0-B3 -> D8-D11 = PA0-PA3
  PORTB |= bit0bis3;            // benoetigte BITs B0-B3 -> D8-D11 = PA0-PA3 setzen 
  bit4bis6 = code & B01110000;  // PortD -> D4-D6 -> D4-D6  = PA4-PA6
  PORTD |= bit4bis6;            // benoetigte BITs D4-D6 -> D4-D6  = PA4-PA6 setzen
  digitalWrite(TASTE, HIGH);    // Ausgabe gültig -> PA7 = HIGH
  delay(40);                    // 40ms warten
  digitalWrite(TASTE, LOW);     // PA7 rücksetzen
  PORTB &= B11110000;           // gesetzte BITs B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
  PORTD &= B10001111;           // gesetzte BITs D4-D6 -> D4-D6  = PA4-PA6 rücksetzen
  delay(40);                    // 40ms warten
}

void tastenstring(String taste)  // Die Funktion zerlegt einen String in Einzelzeichen und
{                                // sendet diese an die Parallel-Ausgabe. 
  char puffer[64] = "";          // Der Befehl '\037' und die nachfolgenden 2 Ziffern werden als
  int size = taste.length();     // Delay-Anweisung mit 100 * xy Millisekunden interpretiert.
  bool grafsave = grafmode;
  grafmode = false;  // Grafik-Taste für die Zeichenausgabe abschalten
  digitalWrite(GrafPin, grafmode);
  if (size > 63) size = 63;
  taste.toCharArray(puffer, size + 1); 
  puffer[63] = '\0';
  for (byte i = 0; i < size; i++) {
    if ((puffer[i] == '\037') && (i < (size - 2))){  // '\037' zeigt den Beginn einer Delay-Anweisung an
      int d = (1000 * ((int)puffer[i + 1] - 48) + 100 * ((int)puffer[i + 2] - 48));
      if (d == 19000) {       // 'xy' = 'C0' --> Tastenbelegung: AC1
        cpm_mode = false;
        set_LED();
        Serial.println F("Tastencodes: AC1");
      }
      else if (d == 19100) {  // 'xy' = 'C1' --> Tastenbelegung: CP/M
        cpm_mode = true;
        set_LED();
        Serial.println F("Tastencodes: CP/M");
      }
      else {
        if (d > 9900) d = 9900;  // nur zur Sicherheit, delay = max. 9900 Millisekunden
        Serial.print("[delay");
        Serial.print(d);
        Serial.print("]");
        delay(d);
      }
      i += 2;  // die Ziffern 'xy' für die weitere Bearbeitung überspringen
    }
    else parallelausgabe_40ms(puffer[i], false);  // kein Caps-Lock verwenden!
    delay(Tastenstring_Delay);  // Verzögerung zwischen den einzelnen Zeichen, siehe config.h
  }
  grafmode = grafsave;  // ursprünglichen Grafik-Modus wieder einstellen
  digitalWrite(GrafPin, grafmode);
}

void parallelausgabe_Joystick(byte code) {  // Joystick-Ausgabe
  byte taste = 7;  // D7 -> PA7

  if (code > 0) {
    anzeige_tastencode(code);

    // Ausgabe des Joystickcodes auf D4 bis D11:
    bitWrite(PORTB, 0, bitRead(code, 0));
    bitWrite(PORTB, 1, bitRead(code, 1));
    bitWrite(PORTB, 2, bitRead(code, 2));
    bitWrite(PORTB, 3, bitRead(code, 3));
    bitWrite(PORTD, 4, bitRead(code, 4));
    bitWrite(PORTD, 5, bitRead(code, 5));
    bitWrite(PORTD, 6, bitRead(code, 6));
   
    if (joy_mode > 1) {
      digitalWrite(taste, HIGH);    // Ausgabe gültig -> PA7 = HIGH
      delay(40);                    // 40ms warten
    }
  }
  else {  // (code == 0)
    digitalWrite(taste, LOW);        // PA7 rücksetzen
    PORTB &= B11110000;              // gesetzte BITs B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
    PORTD &= B10001111;              // gesetzte BITs D4-D6 -> D4-D6  = PA4-PA6 rücksetzen
    delay(10);                       // Entprellung Joystick-Taster
  }
}

void joystickabfrage() {
  byte hoch, runter, links, rechts, feuer;
  byte j=0 ;  

  hoch = 1 - digitalRead(14);
  runter = 1 - digitalRead(15);
  links = 1 - digitalRead(16);
  rechts = 1 - digitalRead(17);
  feuer = 1 - digitalRead(18);

  j = hoch + 2 * runter + 4 * links + 8 * rechts + 16 * feuer;
  if (j != joy_code) {
    joy_code = j;
    if (joy_mode == 1) parallelausgabe_Joystick(joy_code);  // auch mehrere Taster gleichzeitig
    else {                          
      if (joy_code & 0x01)      parallelausgabe_Joystick(JOY_CODE[joy_mode - 2][0]);  // hoch
      else if (joy_code & 0x02) parallelausgabe_Joystick(JOY_CODE[joy_mode - 2][1]);  // runter
      else if (joy_code & 0x04) parallelausgabe_Joystick(JOY_CODE[joy_mode - 2][2]);  // links
      else if (joy_code & 0x08) parallelausgabe_Joystick(JOY_CODE[joy_mode - 2][3]);  // rechts
      else if (joy_code & 0x16) parallelausgabe_Joystick(JOY_CODE[joy_mode - 2][4]);  // "Feuer"
      else parallelausgabe_Joystick(0);  // Joystick losgelassen
    }
    delay(JOY_DELAY);  // Tasterentprellung
  }
}

void set_LED() {  // Ansteuerung der Tastatur-LED's
  byte led = 0;

  if (capslock) led += 4;
  if (cpm_mode) led += 1;
  if (grafmode) led += 2;
  keyboard.setLock(led);
}

void get_LED() {  // Ansteuerung der Tastatur-LED's
  byte led;

  led = keyboard.getLock();
  if (led & 0x04) capslock = true; else capslock = false;
  if (led & 0x01) cpm_mode = true; else cpm_mode = false;
  if (led & 0x02) grafmode = true; else grafmode = false;
}

void setup() {
  delay(500);  // Selbstfindungsphase...

  // Tastatur- und Joystickeinstellungen aus config.h übernehmen
  kbd_mode = Tastatur_Init_Kbd_Mode;  // Keyboard-Mode: true => PA7=HIGH, bis Taste losgelassen wird
                                      //                false => 40ms-Impuls auf PA7
  capslock = Tastatur_Init_CapsLock;  // Flag fuer die Caps-Lock Taste
  cpm_mode = Tastatur_Init_CPM_Mode;  // CP/M-Mode: false => AC1-Tastenbelegung; true => CP/M-Tastenbelegung
  grafmode = Tastatur_Init_GrafMode;  // SCCH-Grafikmodus --> PIO B2
  joy_mode = JOY_MODE;
  joy_code = 0;
  
  // Initialisierung Keyboard
  keyboard.begin(DataPin, IRQpin);
  keyboard.setNoRepeat(1);  // kein Repeat für Ctrl,...
  set_LED();                // LED's ansteuern
   
  Serial.begin(9600); 
  Serial.println F("Bitte 115200 BAUD einstellen!");
  delay(500);
  Serial.begin(115200); 
  Serial.println F("*** Version vom 29.01.2022 ***");
  if (kbd_mode) Serial.println F("Tastendruck:  Taste-PA7");
  else Serial.println F("Tastendruck:  40ms-Impuls");
  if (capslock) Serial.println F("Caps-Lock:    an");
  else Serial.println F("Caps-Lock:    aus");
  if (cpm_mode) Serial.println F("Tastencodes:  CP/M");
  else Serial.println F("Tastencodes:  AC1");
  if (grafmode) Serial.println F("Grafiktaste:  an");
  else Serial.println F("Grafiktaste:  aus");
#ifdef Debugging
  Serial.println F("Debugging:    ein");
#else
  Serial.println F("Debugging:    aus");
#endif
  Serial.println();

  // Initialisierung der 8-bit Ausgabe
  DDRB |= B00001111;       // BIT 0-3 Ausgang
  PORTB &= B11110000;      // gesetzte BITs B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
  DDRD |= B11110000;       // BIT 4-7 Ausgang
  PORTD &= B00001111;      // gesetzte BITs D4-D7 -> D4-D7  = PA4-PA7 rücksetzen

  // Initialisierung Joystick
  pinMode(14, INPUT_PULLUP); 
  pinMode(15, INPUT_PULLUP); 
  pinMode(16, INPUT_PULLUP); 
  pinMode(17, INPUT_PULLUP); 
  pinMode(18, INPUT_PULLUP); 

  // Initialisierung Grafiktaste
  pinMode(GrafPin, OUTPUT); 
  digitalWrite(GrafPin, grafmode);
  
  // Initialisierung Reset und NMI
  pinMode(RESETpin, OUTPUT); 
  pinMode(NMIpin, OUTPUT); 

  // Ausgabe einer Zeichenfolge beim Tastatur-Reset, siehe config.h
  tastenstring(Tastatur_Init_String);
}

void loop() {
  uint16_t c16;
  byte c8, s8, code;
  
  if (keyboard.available()) {
    
    // read the next key
    c16 = keyboard.read( );
    
    if( c16 > 0 ) {
      if ((c16 & 0x8000) && (kbd_mode)) parallelausgabe_PA7_BREAK();  // BREAK-Code erkannt
      c8 = c16 & 0xFF;  // Tasten-Code (8 bit)
      s8 = c16 >> 8;    // Status-Byte (8 bit)
      s8 = s8 & 0xEF;   // CapsLock-Bit zurücksetzen
      code = 0;         // zu erzeugender Tastencode für den AC1

#ifdef Debugging
      Serial.println(" ");
      Serial.print( "16bit-Wert = 0x" );
      Serial.print( c16, HEX );
      Serial.print( "   8bit-Status = 0x" );
      Serial.print( s8, HEX );
      Serial.print( "   8bit-Code = 0x" );
      Serial.println( c8, HEX );
#endif

      if (c8 == 0xFA) {
        get_LED();                        // Tastatur-LED's abfragen
        digitalWrite(GrafPin, grafmode);  // Grafiktasten-PIN nach LED einstellen
      }

#ifdef Debugging
      if (capslock) Serial.print F("Caps-Lock: ein"); else Serial.print F("Caps-Lock: aus");
      if (!cpm_mode) Serial.print F("   Tastencodes: AC1"); else Serial.print F("   Tastencodes: CP/M");
      if (grafmode) Serial.println F("   Grafiktaste: ein"); else Serial.println F("   Grafiktaste: aus");
#endif
 
      if (c8 >= '0' && c8 <= '9') {  // Ziffern 0 bis 9
        if (s8 == 0x00) code = c8;   // ohne Shift
        else if (s8 == 0x40) {       // mit Shift
          switch (c8) {
            case '1': code = 0x21; break; // !
            case '2': code = 0x22; break; // "
            case '3': code = 0x40; break; // @ (Zeichensatz ACC) bzw. § (Zeichensatz SCCH)
            case '4': code = 0x24; break; // $
            case '5': code = 0x25; break; // %
            case '6': code = 0x26; break; // &
            case '7': code = 0x2F; break; // /
            case '8': code = 0x28; break; // (
            case '9': code = 0x29; break; // )
            case '0': code = 0x3D; break; // =
          }
        }
        else if (s8 == 0x04) {       // mit AltGr
          switch (c8) {
            case '1': {                   // Taste-PA7
              kbd_mode = true; 
              Serial.println F("Tastendruck: Taste-PA7"); 
            } break;
            case '7': code = 0x7B; break; // {
            case '8': code = 0x5B; break; // [
            case '9': code = 0x5D; break; // ]
            case '0': code = 0x7D; break; // }
          }
        }
      }
      
      else if (c8 >= 'A' && c8 <= 'Z') {  // Buchstaben A bis Z
        if (c8 >= 'Y') c8 ++;  // Codes für 'Y' und 'Z' tauschen
        if (c8 > 'Z') c8 = 'Y';
        if (s8 == 0x00) code = c8;              // ohne Shift
        else if (s8 == 0x40) code = c8 + 0x20;  // mit Shift
        else if (s8 == 0x20) code = c8 - 0x40;  // mit Ctrl
        else if ((s8 == 0x04) && (c8 == 'Q')) code = 0x40;  // AltGr+Q --> '@'
      }

      else if (c8 >= 0x61 && c8 <= 0x6C) {  // Funktionstasten F1 bis F12
        if ((s8 == 0x01) && (!cpm_mode)) tastenstring(FKT_AC1[c8 - 0x61]);             // ohne Shift
        else if ((s8 == 0x01) && (cpm_mode)) tastenstring(FKT_CPM[c8 - 0x61]);
        else if ((s8 == 0x41) && (!cpm_mode)) tastenstring(FKT_SHIFT_AC1[c8 - 0x61]);  // mit Shift
        else if ((s8 == 0x41) && (cpm_mode)) tastenstring(FKT_SHIFT_CPM[c8 - 0x61]);
        else if ((s8 == 0x21) && (!cpm_mode)) tastenstring(FKT_CTRL_AC1[c8 - 0x61]);   // mit Ctrl
        else if ((s8 == 0x21) && (cpm_mode)) tastenstring(FKT_CTRL_CPM[c8 - 0x61]);
        else if ((s8 == 0x09) && (!cpm_mode)) tastenstring(FKT_ALT_AC1[c8 - 0x61]);    // mit Alt
        else if ((s8 == 0x09) && (cpm_mode)) tastenstring(FKT_ALT_CPM[c8 - 0x61]);
        else if (s8 == 0x05) {                                                         // Joystick-Mode
          joy_mode = c8 - 0x60;
          Serial.print("Joystick-Mode = ");
          Serial.println(joy_mode);
        }
      }

      else if ((s8 == 0x00) || (s8 == 0x01)) {  // weitere Tastencodes ohne Shift
        switch (c8) {
          case 0x04: {                   // Druck-Taste
            if (!cpm_mode) code = 0x18;  // ^X
            else code = 0x10;            // unter CP/M: ^P 
          } break;
          case 0x06: {                   // Pause-Taste
            if (!cpm_mode) code = 0x13;  // ^S
            else code = 0x13;            // unter CP/M: ^S 
          } break;
          case 0x11: {                   // Pos1-Taste
            if (!cpm_mode) code = 0x01;  // ^A
            else tastenstring("\021S");  // unter CP/M: ^QS 
          } break;
          case 0x12: {                   // Ende-Taste
            if (!cpm_mode) code = 0x03;  // ^C
            else tastenstring("\021D");  // unter CP/M: ^QD
          } break;
          case 0x13: {                   // PgUp-Taste
            if (!cpm_mode) code = 0x15;  // ^U
            else code = 0x12;            // unter CP/M: ^R 
          } break;
          case 0x14: {                   // PgDn-Taste
            if (!cpm_mode) code = 0x11;  // ^Q
            else code = 0x03;            // unter CP/M: ^C
          } break;
          case 0x15: {                   // Leftarrow-Taste
            if (!cpm_mode) code = 0x08;  // ^H
            else code = 0x08;            // unter CP/M: ^H
          } break;
          case 0x16: {                   // Rightarrow-Taste
            if (!cpm_mode) code = 0x09;  // ^I
            else code = 0x04;            // unter CP/M: ^D
          } break;
          case 0x17: {                   // Uparrow-Taste
            if (!cpm_mode) code = 0x0B;  // ^K
            else code = 0x05;            // unter CP/M: ^E
          } break;
          case 0x18: {                   // Downarrow-Taste
            if (!cpm_mode) code = 0x0A;  // ^J
            else code = 0x18;            // unter CP/M: ^X
          } break;
          case 0x19: {                   // Einfg-Taste
            if (!cpm_mode) code = 0x05;  // ^E
            else code = 0x16;            // unter CP/M: ^V 
          } break;
          case 0x1A: {                   // Entf-Taste
            if (!cpm_mode) code = 0x04;  // ^D
            else code = 0x07;            // unter CP/M: ^G
          } break;
          case 0x1B: code = 0x03; break; // ESC --> ohne Shift als Ctrl+C
          case 0x1C: code = 0x7F; break; // Backspace
          case 0x1D: code = Tastatur_TAB_Taste; break; // TAB, siehe config.h
          case 0x1E: code = 0x0D; break; // Enter
          case 0x1F: code = 0x20; break; // Space
          case 0x20: code = 0x30; break; // NUM+'0'
          case 0x21: code = 0x31; break; // NUM+'1'
          case 0x22: code = 0x32; break; // NUM+'2'
          case 0x23: code = 0x33; break; // NUM+'3'
          case 0x24: code = 0x34; break; // NUM+'4'
          case 0x25: code = 0x35; break; // NUM+'5'
          case 0x26: code = 0x36; break; // NUM+'6'
          case 0x27: code = 0x37; break; // NUM+'7'
          case 0x28: code = 0x38; break; // NUM+'8'
          case 0x29: code = 0x39; break; // NUM+'9'
          case 0x2A: code = Tastatur_NUM_Komma; break; // NUM+',' siehe config.h
          case 0x2B: code = 0x0D; break; // NUM+Enter
          case 0x2C: code = 0x2B; break; // NUM+'+'
          case 0x2D: code = 0x2D; break; // NUM+'-'
          case 0x2E: code = 0x2A; break; // NUM+'*'
          case 0x2F: code = 0x2F; break; // NUM+'/'
          case 0x3A: code = 0x5B; break; // Ä
          case 0x3B: code = 0x2C; break; // ,
          case 0x3C: code = 0x7E; break; // ß
          case 0x3D: code = 0x2E; break; // .
          case 0x3E: code = 0x2D; break; // -
          case 0x40: code = 0x5E; break; // ^
          case 0x5B: code = 0x5C; break; // Ö
          case 0x5C: code = 0x23; break; // #
          case 0x5D: code = 0x5D; break; // Ü
          case 0x5E: code = 0x2B; break; // +
          case 0x5F: code = 0x27; break; // '
          case 0x8B: code = 0x3C; break; // <
        }
      }

      else if ((s8 == 0x40) || (s8 == 0x41)) {  // weitere Tastencodes mit Shift
        switch (c8) {
          case 0x1B: code = 0x1B; break; // ESC --> mit Shift als "ESC"
          case 0x3A: code = 0x7B; break; // ä
          case 0x3B: code = 0x3B; break; // ;
          case 0x3C: code = 0x3F; break; // ?
          case 0x3D: code = 0x3A; break; // :
          case 0x3E: code = 0x5F; break; // _
          case 0x5B: code = 0x7C; break; // ö
          case 0x5C: code = 0x27; break; // '
          case 0x5D: code = 0x7D; break; // ü
          case 0x5E: code = 0x2A; break; // *
          case 0x5F: code = 0x60; break; // `
          case 0x8B: code = 0x3E; break; // >
        }
      }
      
      else if ((s8 == 0x20) || (s8 == 0x21)) {  // weitere Tastencodes mit Ctrl
        switch (c8) {
          case 0x1B: {                   // Ctrl+ESC --> NMI am AC1
            Serial.println F(" ==NMI== ");
            digitalWrite(NMIpin,HIGH);
            delay(Impulslaenge_NMI);     // siehe config.h
            digitalWrite(NMIpin,LOW);
          } break;       
          case 0x40: code = 0x1E; break; // ^^
          case 0x3A: code = 0x1B; break; // ^Ä
          case 0x5B: code = 0x1C; break; // ^Ö
          case 0x5D: code = 0x1D; break; // ^Ü
        }
      }
      
      else if (s8 == 0x09) {  // weitere Tastencodes mit Alt
        switch (c8) {
          case 0x1B: {                           // Alt+ESC --> AC1-Reset + PS/2-Tastatur-Reset
            Serial.println F(" ==RESET== ");
            digitalWrite(RESETpin,HIGH);
            delay(Impulslaenge_Reset);           // siehe config.h
            digitalWrite(RESETpin,LOW);
            keyboard.resetKey();                 // Reset PS/2-Tastatur
            keyboard.setNoRepeat(1);             // kein Repeat für Ctrl,...
            set_LED();                           // LED's an der Tastatur
            delay(500);                          // kurz warten, sonst werden Zeichen verschluckt...
            tastenstring(Tastatur_Init_String);  // Ausgabe Init-String, siehe config.h
          }
        }
      }
      
      else if ((s8 == 0x04) || (s8 == 0x05)) {  // weitere Tastencodes mit AltGr
        switch (c8) {
          case 0x12: {                   // AltGr+NUM_1
            kbd_mode = true; 
            Serial.println F("Tastendruck: Taste-PA7"); 
          } break;
          case 0x19: {                   // AltGr+NUM_0
            kbd_mode = false; 
            Serial.println F("Tastendruck: 40ms-Impuls"); 
          } break;
          case 0x3C: code = 0x5C; break; // Backslash
          case 0x5E: code = 0x7E; break; // ~
          case 0x8B: code = 0x7C; break; // |
        }
      }

      else if ((c16 == 0x282A) || (c16 == 0x291A) || (c16 == 0x382A) || (c16 == 0x391A)) {  // Ctrl+Alt+Entf --> AC1-Reset
        Serial.println F(" ==AFFENGRIFF== ");
        digitalWrite(RESETpin,HIGH);
        delay(Impulslaenge_Reset);  // siehe config.h
        digitalWrite(RESETpin,LOW);        
      }
      
      // Zeichenausgabe
      if ((code > 0x00) && (code < 0x80)) {
        if (kbd_mode) parallelausgabe_PA7(code);
        else parallelausgabe_40ms(code, true);  // Caps-Lock verwenden, wenn eingeschaltet
      }
    }
  }
  else joystickabfrage();
}
