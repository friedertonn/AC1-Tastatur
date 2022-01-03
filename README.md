# AC1-Tastatur
Die Software ermöglicht die Nutzung einer PS/2-Tastatur am AC1.

Mit Hilfe eines Arduino Nano wird das serielle Signal der PS/2-Tastatur in ein paralleles, AC1-konformes Signal gewandelt.
Die Funktionstasten F1 bis F12 können in der Datei "config.h" mit Steuerkommandos oder ganzen Befehlszeilen belegt werden.
Eine Umschaltung der Kursortasten-Codes für AC1- und CP/M-Programme ist mit der Rollen-Taste möglich.

Das Kernstück des Programms bildet die Bibliothek "PS2KeyAdvanced" von Paul Carpenter (techpaul).
Der Quellcode steht hier: https://github.com/techpaul/PS2KeyAdvanced

Als Hardware wird ein Arduino Nano eingesetzt, der auf Grund seiner geringen Größe im Tastaturgehäuse Platz findet. 
Die Stromversorgung erfolgt über das AC1-Anschlusskabel.
Ein 4-poliger Steckverbinder für die Programmierung des Arduino ist am Tastaturgehäuse angebracht.
Über die PINs D13, D12 und A5/D19 lassen sich zusätzlich die Grafiktaste (D13), AC1-Reset (D12) und AC1-NMI (A5/D19) ansteuern.

Die PINs A0 bis A4 können für einen Atari-Joystick mit 5 Mikrotastern verwendet werden.
Der Joystick nutzt die Routine 0x0EB4 im SCCH-Monitor und kann alternativ Tastencodes emulieren.
Die 5 Taster hoch (A0), runter (A1), links (A2), rechts (A3) und Feuer (A4) werden dazu mit den genannten PINs und
GND verbunden.

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/PS2-Tastatur_mit_Text.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Arduino-Nano_mit_Text.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Arduino-Nano_Schaltbild.gif)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Leiterplatte_AC1-PIO-CTC.jpg)
