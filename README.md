# AC1-Tastatur
Das Programm "AC1-Tastatur.ino" für den Arduino Nano ermöglicht die Nutzung einer PS/2-Tastatur am AC1.

Dabei wird das serielle Signal der PS/2-Tastatur in ein paralleles, AC1-konformes Signal gewandelt.
Die Funktionstasten F1 bis F12 können in der Datei "config.h" mit Steuerkommandos oder ganzen Befehlszeilen belegt werden.
Eine Umschaltung der Kursortasten-Codes für AC1- und CP/M-Programme ist mit der Rollen-Taste möglich. 
Für den Arduino Nano Every ist das Programm "Every.ino" im Unterverzeichnis "Every" zu verwenden. 

Das Kernstück des Programms bildet die Bibliothek "PS2KeyAdvanced" von Paul Carpenter.
Der Quellcode steht hier: https://github.com/techpaul/PS2KeyAdvanced

Als Hardware wird ein Arduino Nano eingesetzt, der auf Grund seiner geringen Größe im Tastaturgehäuse Platz findet. 
Die Stromversorgung erfolgt über das AC1-Anschlusskabel.
Ein 4-poliger Steckverbinder für die Programmierung des Arduino ist am Tastaturgehäuse angebracht.
Über die PINs D13, D12 und A5/D19 lassen sich bei Bedarf die Grafiktaste (D13), AC1-Reset (D12) und AC1-NMI (A5/D19) ansteuern.

Die PINs A0 bis A4 können für einen Atari-Joystick mit 5 Mikrotastern verwendet werden.
Der Joystick nutzt die Routine 0x0EB4 im SCCH-Monitor und kann alternativ Tastencodes emulieren.
Die 5 Taster hoch (A0), runter (A1), links (A2), rechts (A3) und Feuer (A4) werden dazu mit den genannten PINs und
GND verbunden.

Die nachfolgenden Abbildungen zeigen die Umsetzung des Tastatur-Projekts (ohne Joystick):

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Arduino-Nano_Schaltplan.gif)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Arduino-Nano_mit_Text.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/PS2-Tastatur_mit_Text.jpg)

Für den AC1-2010 wurde eine Leiterplatte im Europakarten-Format 100 mm × 160 mm 
mit 64-pol. Steckverbinder erstellt, welche alle Programmfunktionen unterstützt.
Das Projekt ist auf http://www.ac1-info.de/ --> AC1-2010 --> Baugruppen --> PIO_Joystick zu finden.

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Leiterplatte_AC1-PIO-CTC.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Leiterplatte_aufgebaut.jpg)
