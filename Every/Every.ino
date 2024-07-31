/* PS/2-Keyboard + Joystick fuer den AC1
 * Das Programm wurde von Gerd Staffen und Frieder Tonn erstellt
 * Es nutzt eine angepasste Bibliothek "PS2Keyboard" von Paul Stoffregen
 * Die Dateien PS2Keyboard.cpp und PS2Keyboard.h wurden modifiziert
 * und müssen im Sketch-Verzeichnis stehen.
 * Original-Quellcode: https://github.com/PaulStoffregen/PS2Keyboard
 * 
 * Die Konfiguration der Tastatur (Verhalten beim Einschalten, Belegung der 
 * Funktionstasten, Joystick-Tasterbelegungen, Zeitschleifen) erfolgt in der 
 * Datei config.h
 */

// TODO:
// - PS2-Tastatur LED's
// - PS2-Tastatur Reset

#include "PS2-Keyboard.h"
#include "config.h"

const int DataPin = 2;    // D2  - PS/2-Data
const int IRQpin  = 3;    // D3  - PS/2-CLK
const int GrafPin = 13;   // D13 - SCCH-Grafiktaste --> PIO-B2
const int RESETpin = 20;  // D20 - AC1-RESET
const int NMIpin = 21;    // D21 - AC1-NMI
#ifdef LED_statt_Joystick
const int CPM_LED = 14;   // D14 - Rollen-LED (Anzeige CP/M-Tastencodes)
const int CAPS_LED = 15;  // D15 - CapsLock-LED (Anzeige CapsLock)
const int GRAF_LED = 16;  // D16 - NumLock-LED (Anzeige Grafiktaste)
#endif

byte c;
bool kbd_mode;  // Keyboard-Mode: true => PA7=HIGH, bis Taste losgelassen wird; false => 40ms-Impuls auf PA7  
bool capslock;  // Flag fuer die Caps-Lock Taste
bool cpm_mode;  // CP/M-Mode: false => AC1-Tastenbelegung; true => CP/M-Tastenbelegung 
bool grafmode;  // SCCH-Grafikmodus --> PIO B2 
byte joy_mode;  // Joystick-Mode
byte joy_code;  // aktueller Joystick-Code

PS2Keyboard keyboard;

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
  keyboard.begin(DataPin, IRQpin, PS2Keymap_German);
  
  Serial.begin(9600); 
  Serial.println F("Bitte 115200 BAUD einstellen!");
  delay(500);
  Serial.begin(115200); 
  Serial.println F("*** Version vom 31.07.2024 ***");
  if (kbd_mode) Serial.println F("Tastendruck:  Taste-PA7");
  else Serial.println F("Tastendruck:  40ms-Impuls");
  if (capslock) Serial.println F("Caps-Lock:    an");
  else Serial.println F("Caps-Lock:    aus");
  if (cpm_mode) Serial.println F("Tastencodes:  CP/M");
  else Serial.println F("Tastencodes:  AC1");
  if (grafmode) Serial.println F("Grafiktaste:  an");
  else Serial.println F("Grafiktaste:  aus");
#ifdef LED_statt_Joystick
  Serial.println F("Joystick/LED: LED");
#else
  Serial.println F("Joystick/LED: Joystick");
#endif

  Serial.println();

  // Initialisierung der 8-bit Ausgabe
  DDRB |= B00001111;       // BIT 0-3 Ausgang
  PORTB &= B11110000;      // gesetzte BITs B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
  DDRD |= B11110000;       // BIT 4-7 Ausgang
  PORTD &= B00001111;      // gesetzte BITs D4-D7 -> D4-D7  = PA4-PA7 rücksetzen

#ifdef LED_statt_Joystick
  // Initialisierung Tastatur-LEDs
  pinMode(CAPS_LED, OUTPUT); 
  digitalWrite(CAPS_LED, !capslock);
  pinMode(CPM_LED, OUTPUT); 
  digitalWrite(CPM_LED, !cpm_mode);
  pinMode(GRAF_LED, OUTPUT); 
  digitalWrite(GRAF_LED, !grafmode);
#else
  // Initialisierung Joystick
  pinMode(14, INPUT_PULLUP); 
  pinMode(15, INPUT_PULLUP); 
  pinMode(16, INPUT_PULLUP); 
  pinMode(17, INPUT_PULLUP); 
  pinMode(18, INPUT_PULLUP); 
#endif

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
  if (keyboard.available(kbd_mode, cpm_mode)) {
    
    // read the next key
    c = keyboard.read(kbd_mode, cpm_mode);

    // Zeichenausgabe
    if ((c > 0) && (c < 128)) {
      if (kbd_mode) parallelausgabe_PA7(c);
      else parallelausgabe_40ms(c, true);  // Caps-Lock verwenden, wenn eingeschaltet
    }

    // Funktionstasten F1 bis F12:
    else if ((c >= PS2_F1) && (c <= PS2_F12)) {
      if (!cpm_mode) tastenstring(FKT_AC1[c - PS2_F1]);
      else tastenstring(FKT_CPM[c - PS2_F1]);
    }
    
    // Funktionstasten Shift+F1 bis Shift+F12:
    else if ((c >= PS2_SHIFT_F1) && (c <= PS2_SHIFT_F12)) {
      if (!cpm_mode) tastenstring(FKT_SHIFT_AC1[c - PS2_SHIFT_F1]);
      else tastenstring(FKT_SHIFT_CPM[c - PS2_SHIFT_F1]);
    }
    
    // Funktionstasten Ctrl+F1 bis Ctrl+F12:
    else if ((c >= PS2_CTRL_F1) && (c <= PS2_CTRL_F12)) {
      if (!cpm_mode) tastenstring(FKT_CTRL_AC1[c - PS2_CTRL_F1]);
      else tastenstring(FKT_CTRL_CPM[c - PS2_CTRL_F1]);
    }
    
    // Funktionstasten Alt+F1 bis Alt+F12:
    else if ((c >= PS2_ALT_F1) && (c <= PS2_ALT_F12)) {
      if (!cpm_mode) tastenstring(FKT_ALT_AC1[c - PS2_ALT_F1]);
      else tastenstring(FKT_ALT_CPM[c - PS2_ALT_F1]);
    }
    
    // Joystick-Mode einstellen (AltGr+F1 bis AltGr+F12):
    else if ((c >= PS2_ALTGR_F1) && (c <= PS2_ALTGR_F12)) {
      joy_mode = c - PS2_ALTGR_F1 + 1;
      Serial.print("Joystick-Mode = ");
      Serial.println(joy_mode);
    }
    
    else switch (c) {  // und jetzt die Funktionstasten mit code > 187
      case PS2_ALTGR_0: {
        kbd_mode = false; 
        Serial.println F("Tastendruck: 40ms-Impuls"); 
      } break;
      
      case PS2_ALTGR_1: {
        kbd_mode = true; 
        Serial.println F("Tastendruck: Taste-PA7"); 
      } break;
      
      case PS2_CAPS: {
        capslock = !capslock; 
#ifdef LED_statt_Joystick
        digitalWrite(CAPS_LED, !capslock);
#endif
        if (capslock) Serial.println F("Caps-Lock: ein"); else (Serial.println F("Caps-Lock: aus")); 
      } break;
      
      case PS2_SCROLL: { 
        cpm_mode = !cpm_mode; 
#ifdef LED_statt_Joystick
        digitalWrite(CPM_LED, !cpm_mode);
#endif
        if (!cpm_mode) Serial.println F("Tastencodes: AC1"); else Serial.println F("Tastencodes: CP/M"); 
      } break;
      
      case PS2_NUM: {
        grafmode = !grafmode;
#ifdef LED_statt_Joystick
        digitalWrite(GRAF_LED, !grafmode);
#endif
        digitalWrite(GrafPin, grafmode);
        if (grafmode) Serial.println F("Grafiktaste: an"); else Serial.println F("Grafiktaste: aus");
      } break;     
          
      case PS2_HOME_CPM: { 
        tastenstring("\021S");               // unter CP/M: Pos1 = ^QS
      } break;
      
      case PS2_END_CPM: { 
        tastenstring("\021D");               // unter CP/M: Ende = ^QD
      } break;
      
      case PS2_CTRL_ESC: {                   // Ctrl+ESC --> AC1-NMI
        Serial.println F(" ==NMI== ");
        digitalWrite(NMIpin,HIGH);
        delay(Impulslaenge_NMI);             // siehe config.h
        digitalWrite(NMIpin,LOW);
      } break;       
         
      case PS2_ALT_ESC: {                    // Alt+ESC --> AC1-Reset
        Serial.println F(" ==RESET== ");
        digitalWrite(RESETpin,HIGH);
        delay(Impulslaenge_Reset);           // siehe config.h
        digitalWrite(RESETpin,LOW);
        delay(500);                          // kurz warten, sonst werden Zeichen verschluckt...
        tastenstring(Tastatur_Init_String);  // Ausgabe Init-String, siehe config.h
      } break;
      
      case AFFENGRIFF: {                     // Ctrl+Alt+Entf --> AC1-Reset
        Serial.println F(" ==AFFENGRIFF== ");
        digitalWrite(RESETpin,HIGH);
        delay(Impulslaenge_Reset);           // siehe config.h
        digitalWrite(RESETpin,LOW);
      } break;  

      case PS2_ESC: { 
        if (!cpm_mode) parallelausgabe_40ms(PS2_ESC_AC1, false);
        else parallelausgabe_40ms(PS2_ESC_CPM, false);
      } break;

      case PS2_BACKSPACE: { 
        if (!cpm_mode) parallelausgabe_40ms(PS2_BACKSPACE_AC1, false);
        else parallelausgabe_40ms(PS2_BACKSPACE_CPM, false);
      } break;

      case PS2_ALTGR_G: { 
        grafmode = !grafmode;
        digitalWrite(GrafPin, grafmode);
        if (grafmode) Serial.println F("Grafiktaste: an");
        else Serial.println F("Grafiktaste: aus");
      } break;

      case PS2_ALTGR_T: { 
        kbd_mode = !kbd_mode;
        if (kbd_mode) Serial.println F("Tastendruck: Taste-PA7");
        else Serial.println F("Tastendruck: 40ms-Impuls");
      } break;

      default: tastenstring("???"); break;
    }
  }
#ifndef LED_statt_Joystick  
  else joystickabfrage();
#endif
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
#ifdef LED_statt_Joystick
        digitalWrite(CPM_LED, !cpm_mode);
#endif
        Serial.println F("Tastencodes: AC1");
      }
      else if (d == 19100) {  // 'xy' = 'C1' --> Tastenbelegung: CP/M
        cpm_mode = true;
#ifdef LED_statt_Joystick
        digitalWrite(CPM_LED, !cpm_mode);
#endif
        Serial.println F("Tastencodes: CP/M");
      }
      else {
        if (d > 9900) d = 9900;  // nur zur Sicherheit, delay = max. 9900 Millisekunden
        Serial.print("[delay");
        Serial.print(d);
        Serial.print("]");
        delay(d);
      }
      delay(40);
      i += 2;  // die Ziffern 'xy' für die weitere Bearbeitung überspringen
    }
    else parallelausgabe_40ms(puffer[i], false);  // kein Caps-Lock verwenden!
    delay(Tastenstring_Delay);  // Verzögerung zwischen den einzelnen Zeichen, siehe config.h
  }
  grafmode = grafsave;  // ursprünglichen Grafik-Modus wieder einstellen
  digitalWrite(GrafPin, grafmode);
}

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
  // das Rücksetzen von PA7 erfolgt in get_iso8859_code() in PS2-Keyboard.cpp
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

#ifndef LED_statt_Joystick
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

void parallelausgabe_Joystick(byte code) {  // Joystick-Ausgabe
  byte TASTE = 7;  // D7 -> PA7

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
      digitalWrite(TASTE, HIGH);    // Ausgabe gültig -> PA7 = HIGH
      delay(40);                    // 40ms warten
    }
  }
  else {  // (code == 0)
    digitalWrite(TASTE, LOW);        // PA7 rücksetzen
    PORTB &= B11110000;              // gesetzte BITs B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
    PORTD &= B10001111;              // gesetzte BITs D4-D6 -> D4-D6  = PA4-PA6 rücksetzen
  }
  delay(10);  // Entprellung Joystick-Taster
}
#endif
