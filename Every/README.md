# AC1-Tastatur mit einem Arduino Nano Every
Die Software "Every.ino" ermöglicht die Nutzung einer PS/2-Tastatur am AC1.

Mit Hilfe eines Arduino Nano Every wird das serielle Signal der PS/2-Tastatur in ein paralleles, AC1-konformes Signal gewandelt.
In der Datei "config.h" können verschiedene Parameter eingestellt werden.

Ein wesentliches Element dieses Programms bildet die Bibliothek "PS2Keyboard" von Paul Stoffregen.
Die Dateien "PS2-Keyboard.cpp" und "PS2-Keyboard.h" wurden modifiziert.
Der Original-Quellcode steht hier: https://github.com/PaulStoffregen/PS2Keyboard

Als Hardware wird ein Arduino Nano Every eingesetzt. Er wird im ATmega328P-Kompatibilitätsmodus betrieben. 
Leider können mit der eingesetzten Bibliothek "PS2Keyboard" die folgenden Funktionen nicht umgesetzt werden:
- Tastatur-Reset,
- Einstellen und Abfrage der Status-LEDs.

Die Ursache liegt in einer beim ATMega4809-Prozessor geänderten Interrupt-Behandlung bei der Umschaltung zwischen seriellem Senden und Empfang über die gleiche Datenleitung. Als Workaround können die 3 LEDs über 330 Ohm Widerstände an die PINs D14 bis D16 angeschlossen werden:
- Rollen-LED an D14
- CapsLock-LED an D15
- NumLock-LED an D16

In der Datei config.h muss dann die Definition "#define LED_statt_Joystick" aktiv sein. Wenn diese Option gewählt wird, ist eine Joystick-Nutzung nicht mehr möglich.