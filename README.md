# AC1-Tastatur
Das Programm ermöglicht die Nutzung einer PS/2-Tastatur am AC1.

Mit Hilfe eines Arduino wird das serielle Signal der PS/2-Tastatur in ein paralleles, AC1-konformes Signal gewandelt.
Die Funktionstasten F1 bis F12 können in der Datei "config.h" mit Strings oder Steuerkommandos belegt werden.
Eine Umschaltung der Kursortasten-Codes für AC1- und CP/M-Programme ist ebenfalls möglich.

Das Kernstück des Programms bildet eine angepasste Bibliothek "PS2Keyboard" von Paul Stoffregen.
Die Dateien PS2-Keyboard.cpp und PS2-Keyboard.h wurden modifiziert.
Der Original-Quellcode steht hier: https://github.com/PaulStoffregen/PS2Keyboard

Als Hardware wird ein Arduino Nano Every eingesetzt, der auf Grund seiner geringen Größe im Tastaturgehäuse Platz hat. 
Die Stromversorgung erfolgt über das AC1-Anschlusskabel.
Ein 4-poliger Steckverbinder für die Programmierung des Arduino ist am Tastaturgehäuse angebracht.
Über die PINs D13, D20 und D21 lassen sich zusätzlich die Grafiktaste (D13), AC1-Reset (D20) und AC1-NMI (D21) ansteuern.

Die PINs A0 bis A4 können für einen Atari-Joystick verwendet werden. Der Joystick nutzt die Routine 0x0EB4 im SCCH-Monitor
und kann alternativ Tastencodes emulieren.
Die 5 Taster hoch (A0), runter (A1), links (A2), rechts (A3) und Feuer (A4) werden dazu mit den genannten PINs und
GND verbunden.

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/PS2-Tastatur_mit_Text.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Arduino_Nano_mit_Text.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Arduino-Schaltbild.gif)
