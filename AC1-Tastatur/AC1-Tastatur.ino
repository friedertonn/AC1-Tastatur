/* PS/2-Keyboard fuer den AC1 */
/* Das Programm wurde von Gerd Staffen und Frieder Tonn erstellt */
/* Es nutzt eine angepasste Bibliothek "PS2Keyboard" von Paul Stoffregen */
/* Original-Quellcode: https://github.com/PaulStoffregen/PS2Keyboard */

#include <PS2Keyboard.h>
const int DataPin = 2;
const int IRQpin =  3;

byte zcode; 
char c;
uint8_t kbd_mode;  

PS2Keyboard keyboard;

void setup() {
  delay(1000);
  kbd_mode = 1;
  keyboard.begin(DataPin, IRQpin, PS2Keymap_German);
  Serial.begin(9600); 
  Serial.println("Version vom 08.04.2021");

  // https://forum.arduino.cc/index.php?topic=475976.0
  DDRB = 0x0F;     // BIT 0-3 = Ausgang
  PORTB = 0xF0;    // B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
  DDRD = 0xF0;     // BIT 4-7 = Ausgang
  PORTD = 0x0F;    // D4-D7 -> D4-D7  = PA4-PA7 rücksetzen

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (keyboard.available(kbd_mode)) {
    
    // read the next key
    c = keyboard.read(kbd_mode); zcode = c;
    
    // check for some of the special keys
    if (zcode == 128) {
      Serial.print("[F1]");
      TastenString("RUN\015");
      Serial.println();
    } else if (zcode == 129) {
      Serial.print("[F2]");
      TastenString("LIST\015");
      Serial.println();
    } else if (zcode == 130) {
      Serial.print("[F3]");
      TastenString("SYSTEM\015");
      Serial.println();
    } else if (zcode == 131) {
      Serial.print("[F4]");
      TastenString("EXIT\015");
      Serial.println();
    } else if (zcode == 132) {
      Serial.print("[F5]");
      TastenString("\021R");   // = ^QR (Wordstar: 1. Zeile)
      Serial.println();
    } else if (zcode == 133) {
      Serial.print("[F6]");
      TastenString("\021C");   // = ^QC (Wordstar: letzte Zeile)
      Serial.println();
    } else if (zcode == 134) {
      Serial.print("[F7]");
      TastenString("\013D");   // = ^KD (Wordstar: Dokument verlassen + speichern)
      Serial.println();
    } else if (zcode == 135) {
      Serial.print("[F8]");
      TastenString("\013X");   // = ^KX (Wordstar: Dokument verlassen ohne speichern)
      Serial.println();
    } else if (zcode == 136) {
      Serial.print("[F9]");
      kbd_mode = 1;
      Serial.println("Tastaturbelegung: AC1 + Großbuchstaben");
    } else if (zcode == 137) {
      Serial.print("[F10]");
      kbd_mode = 2;
      Serial.println("Tastaturbelegung: AC1 + Kleinbuchstaben");
    } else if (zcode == 138) {
      Serial.print("[F11]");
      kbd_mode = 3;
      Serial.println("Tastaturbelegung: CP/M + Großbuchstaben");
    } else if (zcode == 139) {
      Serial.print("[F12]");
      kbd_mode = 4;
      Serial.println("Tastaturbelegung: CP/M + Kleinbuchstaben");
    } else if (zcode == 140) {
      Serial.print("[Pos1]");  // nur unter CP/M: Pos1 = ^QS
      parallel_ausgabe(17);
      parallel_ausgabe(83);
    } else if (zcode == 141) {
      Serial.print("[End]");   // nur unter CP/M: Pos1 = ^QD
      parallel_ausgabe(17);
      parallel_ausgabe(68);
    } else {
      if (zcode == 127) Serial.print("[DEL]");
      if (c == PS2_ENTER) Serial.println();
      parallel_ausgabe(c);
    }
  }
}

void TastenString(String taste)   // zerlegt einen String in Einzelzeichen und
{                                 // sendet diese an parallel-Ausgabe 
  char FTASTE[16]=" ";
  taste.toCharArray(FTASTE, taste.length()+1); 
  for (byte i = 0; i<taste.length(); i++) {
     parallel_ausgabe(FTASTE[i]);
  }
}

void parallel_ausgabe(byte code)   // AC1 ParallelZeichencode PA0-PA7 80 milliSek
{
 byte bit0bis3;     // PortB -> B0-B3 -> D8-D11 = PA0-PA3
 byte bit4bis6;     // PortD -> D4-D6 -> D4-D6  = PA4-PA6
 byte TASTE = 7;    // PortD -> D7    -> D7     = PA7

  bit0bis3 = code & B00001111;     // PortB -> B0-B3 -> D8-D11 = PA0-PA3
  PORTB = bit0bis3;                // benoetigte BITs setzen 
  bit4bis6 = code & B01110000;     // PortD -> D4-D6 -> D4-D6  = PA4-PA6
  PORTD = bit4bis6;                // benoetigte BITs setzen
  digitalWrite(TASTE, HIGH);       // Ausgabe gültig -> PA7 = HIGH
  digitalWrite(LED_BUILTIN, HIGH); // D13 (LED) = (PA7)  nur zur optischen Signalisierung
  if (code >= 32) {
    if (code < 127) Serial.print(char(code));
  }
  else {
    Serial.print(char(94));
    Serial.print(char(code+64));
  }
  delay(40);                      // 40 milliSek warten
  digitalWrite(TASTE, LOW);       // PA7 rücksetzen
  digitalWrite(LED_BUILTIN, LOW); // D13 (LED) = (PA7)  nur zur optischen Signalisierung
  PORTB = 0xF0;                   // gesetzte BITs B0-B3 -> D8-D11 = PA0-PA3 rücksetzen
  PORTD = 0x0F;                   // gesetzte BITs D4-D7 -> D4-D7  = PA4-PA7 rücksetzen
  delay(40);                      // 40 milliSek warten
}
