# AC1-Tastatur mit einem Arduino Nano Every
Die Software "Every.ino" ermöglicht die Nutzung einer PS/2-Tastatur am AC1.

Mit Hilfe eines Arduino Nano Every wird das serielle Signal der PS/2-Tastatur in ein paralleles, AC1-konformes Signal gewandelt.
In der Datei "config.h" können verschiedene Parameter eingestellt werden.

Ein wesentliches Element dieses Programms bildet die Bibliothek "PS2Keyboard" von Paul Stoffregen.
Die Dateien "PS2-Keyboard.cpp" und "PS2-Keyboard.h" wurden modifiziert.
Der Original-Quellcode steht hier: https://github.com/PaulStoffregen/PS2Keyboard

Als Hardware wird ein Arduino Nano Every eingesetzt. Er wird im ATmega328P-Kompatibilitätsmodus betrieben. 
Leider ist es mit der eingesetzten Bibliothek nicht möglich, die Status-LEDs einzustellen und abzufragen. 
Als Workaround können die 3 LEDs über 330 Ohm Widerstände an die PINs D14 bis D16 angeschlossen werden:
- Rollen-LED an D14,
- CapsLock-LED an D15,
- NumLock-LED an D16.

In der Datei config.h muss dann die Definition "#define LED_statt_Joystick" aktiv sein.
Wenn diese Option gewählt wird, ist eine Joystick-Nutzung nicht mehr möglich.

**Wichtiger Hinweis:** Bei einigen aktuell käuflichen Tastaturen, die das PS/2- und das USB-Protokoll unterstützen, 
müssen ggf. 10K-Pullup-Widerstände an die Takt- und die Datenleitung angelötet werden.
Danke an Volker und Detlef für die Information.
