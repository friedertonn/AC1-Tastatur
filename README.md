# AC1-Tastatur
Das Programm ermöglicht die Nutzung einer PS/2-Tastatur am AC1.

Mit Hilfe eines Arduino wird das serielle Signal der PS/2-Tastatur in ein paralleles, AC1-konformes Signal gewandelt.
Die Funktionstasten F1 bis F12 können in der Datei "AC1-Tastatur.ino" mit Strings oder Spezialkommandos belegt werden.

Es wird eine angepasste Bibliothek "PS2Keyboard" von Paul Stoffregen verwendet.
Die Dateien PS2Keyboard.cpp und PS2Keyboard.h wurden modifiziert.
Der Original-Quellcode steht unter: https://github.com/PaulStoffregen/PS2Keyboard

Als Hardware wird ein Arduino Nano Every eingesetzt, der im Tastaturgehäuse Platz hat. 
Die Stromversorgung erfolgt über das Anschlusskabel.
Ein 4-poliger Steckverbinder für die Programmierung des Arduino ist am Tastaturgehäuse angebracht.
Die Pinbelegung ist:
D2 = PS/2-Data;
D3 = PS/2-Clock;
D8 = AC1.PA0;
D9 = AC1.PA1;
D10 = AC1.PA2;
D11 = AC1.PA3;
D4 = AC1.PA4;
D5 = AC1.PA5;
D6 = AC1.PA6;
D7 = AC1.PA7-Status;

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Tastatur.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Nano.jpg)
