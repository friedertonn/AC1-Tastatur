// config.h
//
// In dieser Datei wird die Konfiguration der Tastatur vorgenommen.
// Im Originalzustand sind alle Einstellungen weitgehend identisch zum PIC16F877
// Bei Bedarf können die Parameter nach Vorliebe und Erfordernis angepasst werden,
// z.B. Ausführen eines Monitor-Befehls nach Reset, Änderungen der Funktionstasten, ...

// Vor der Leiterplatte gibt es 2 Versionen:
// - einen Prototypen "Nov. 2021" mit Arduino-Nano-Every (Reset --> D20 , NMI --> D21) und
// - die finale Version "Dez. 2021" mit Arduino-Nano (Reset --> D12 , NMI --> D19)
// Für die finale Version muss "Leiterplatte_Dez2021" definiert sein!
// #define Leiterplatte_Dez2021

// Die Tastatur kann nach jedem Tastatur-Reset eine Zeichenfolge ausgeben,
// z.B. "W F0 1\015", um den AC1 auf 4 MHz umzuschalten.
// Der Code '\015' steht dabei die die Enter-Taste.
// Weitere denkbare Szenarien sind AC1-Monitor-Initialisierungen wie V.24,
// Centronics, Color, Druckertreiber, usw. (ähnlich AUTOEXEC.BAT).
// Bei einer Definition: #define Tastatur_Init_String "" wird nichts ausgegeben.
// Der Befehl '\037' und die nachfolgenden 2 Ziffern 'xy' werden als Delay-Anweisung 
// mit 'xy' * 100 Millisekunden interpretiert.
// Beispiel: #define Tastatur_Init_String " Hallo\03710 :)\015"
#define Tastatur_Init_String "\007"  // Tülütütü

// Festlegen, wie die Tastatur bei Tastendrücken reagiert:
// true  --> bei jedem Tastendruck wird PA7 aktiv, bis die Taste losgelassen wird (Original-AC1)
// false --> bei jedem Tastendruck 40ms-Impuls auf PA7, dann 40ms Pause
// Umschaltung des Tastatur-Modus erfolgt mit der 
// --> AltGr+1-Tastenkombination: "Standard-Modus" --> PA7 ist aktiv, bis die Taste losgelassen wird
// --> AltGr+0-Tastenkombination: "Spezial-Modus"  --> 40ms-Impuls auf PA7
// Der Spezial-Modus (Tastatur_Init_Kbd_Mode = false) ist für Schnellschreiber besser geeignet.
// Auch EDAS*4 läuft im Spezial-Modus unter 4MHz sehr zuverlässig (keine doppelten Zeichen).
#define Tastatur_Init_Kbd_Mode false

// Festlegen, ob Caps-Lock nach dem Tastatur-Reset ein- oder ausgeschaltet ist.
// Beim Original-AC1 werden beim Tastendruck ohne SHIFT Großbuchstaben ausgegeben, also "true".
// Umschaltung des Caps-Lock-Modus erfolgt mit der Caps-Lock-Taste.
#define Tastatur_Init_CapsLock true

// Festlegen, ob die Tastatur nach Reset mit AC1- oder CP/M-Tastenbelegung startet.
// Die Definition ist hilfreich, wenn mit "Tastatur_Init_String" ein CP/M-System geladen wird.
// Die Umschaltung der CP/M-Tastenbelegung erfolgt mit der Rollen-Taste.
// Die Umschaltung kann auch über die Funktionstasten erfolgen:
// '\037C1' schaltet auf Tastenbelegung: CP/M
// '\037C0' schaltet zurück auf Tastenbelegung: AC1
#define Tastatur_Init_CPM_Mode false

// Festlegen, ob die Tastatur nach Reset mit aktivierter Grafiktaste startet.
// Bei der Tastatur mit dem PIC16F877 war dies der Fall, ist aber nervig,
// wenn nicht ständig mit GBASIC und dem Grafik-Zeichensatz gearbeitet wird.
// Umschaltung des Grafik-Modus erfolgt mit der Num-Lock-Taste.
#define Tastatur_Init_GrafMode false

// Bei den beiden nachfolgenden Definitionen wird die Impulslänge für den NMI- und Reset-Impuls
// beim AC1 festgelegt. Bei Reset können 50 Millisekunden nicht schaden, bei NMI ist dies von
// der Verschaltung abhängig: Wird der NMI-Impuls vom Arduino an den NMI-Taster gelegt, sind
// 50 Millisekunden ein guter Wert. Wird der NMI-Impuls direkt an die CPU gelegt, sind 0 Millisekunden
// ausreichend. Wichtig ist, dass nur 1x NMI ausgelöst wird.
#define Impulslaenge_Reset 50
#define Impulslaenge_NMI 0

// Definitionen der 12 Funktionstasten im AC1-Modus:
// Auch hier kann der Delay-Befehl '\037xy' eingesetzt werden.
// '\037C1' schaltet auf Tastenbelegung: CP/M
// '\037C0' schaltet zurück auf Tastenbelegung: AC1
const char* FKT_AC1[]={
  "?\015", "?", "?", "\014", 
  "\026", "\027", "?", " F8_AC1",
  "\022", "\025", "\024", "CONT\015"};

// mit Shift:
const char* FKT_SHIFT_AC1[]={
  "?", "WF0 0\015", "?", "WF0 1\015",
  "?", "?", "?", " F8_SHIFT_AC1",
  "?", "?", "?", "?"};

// mit Ctrl:
const char* FKT_CTRL_AC1[]={
  "?", "?", "?", "?",
  "?", "?", "?", " F8_CTRL_AC1",
  "?", "?", "?", "?"};

// mit Alt:
const char* FKT_ALT_AC1[]={
  "?", "?", "?", "?",
  "?", "?", "?", " F8_ALT_AC1",
  "?", "?", "?", " Nano-Every-Version vom 2.1.2022"};

// Definitionen der 12 Funktionstasten im CP/M-Modus:
const char* FKT_CPM[]={
  "FC\015", "?", "?", "?",
  "?", "?", "?", " F8_CPM",
  "?", "?", "?", "EXIT\015"};

// mit Shift:
const char* FKT_SHIFT_CPM[]={
  "?", "?", "?", "?",
  "?", "?", "?", " F8_SHIFT_CPM",
  "?", "?", "?", "?"};

// mit Ctrl:
const char* FKT_CTRL_CPM[]={
  "?", "?", "?", "?",
  "?", "?", "?", " F8_CTRL_CPM",
  "?", "?", "?", "?"};

// mit Alt:
const char* FKT_ALT_CPM[]={
  "?", "?", "?", "?",
  "?", "?", "?", " F8_ALT_CPM",
  "?", "?", "?", "?"};

// Zum Schluss noch die Joystick-Konfiguration:
// Welche Joystick-Tastendefinition soll nach dem Reset aktiv sein?
// Mode 1 ist die Joystickabfrage über den SCCH-Monitor 0x0EB4
// Mode 2 bis 11 emuliert Tastencodes, die frei definiert werden können
#define JOY_MODE 2

// Tastenentprellung für den Joystick:
// Zeit in Millisekunden nach jeder Änderung des Joystick-Codes
#define JOY_DELAY 40

// Joystick-Tastendefinitionen: 11 Definitionen, 5 Tastencodes: hoch, runter, links, rechts, Feuer
const byte JOY_CODE[11][5] = {
  {11, 10, 8, 9, 13},           // Mode 2 (AltGr-F2): AC1-Kursortasten + Enter, z.B. GHOSTTOWN
  {11, 10, 8, 9, 32},           // Mode 3 (AltGr-F3): AC1-Kursortasten + Space, z.B. ACBALL
  {32, 13, 8, 9, 32},           // Mode 4 (AltGr-F4): Belegung für ACTRIS
  {'5', '2', '1', '3', 0x20},   // Mode 5 (AltGr-F5): Ziffernblock und Space, z.B. WURMI
  {'G', 'B', 'K', 'L', 0x0D},   // Mode 6 (AltGr-F6): Belegung für Motocycle
  {'T', 'V', 'F', 'G', 0x0D},   // Mode 7 (AltGr-F7): Belegung für Pacman
  {0, 0, 0, 0, 0},              // Mode 8 (AltGr-F8):
  {0, 0, 0, 0, 0},              // Mode 9 (AltGr-F9):
  {0, 0, 0, 0, 0},              // Mode 10 (AltGr-F10):
  {0, 0, 0, 0, 0},              // Mode 11 (AltGr-F11):
  {0, 0, 0, 0, 0}};             // Mode 12 (AltGr-F12):

//
// Ende der Konfigurationsdatei
