# AC1-Tastatur
Das Programm ermöglicht die Nutzung einer PS/2-Tastatur am AC1.

Mit Hilfe eines Arduino wird das serielle Signal der PS/2-Tastatur in ein paralleles, AC1-konformes Signal gewandelt.
Die Funktionstasten F1 bis F12 können in der Datei "config.h" mit Strings oder Steuerkommandos belegt werden.
Eine Änderung der Kursortasten-Codes für AC1- und CP/M-Programme ist möglich.

Zur Erfassung der Tastencodes wird eine angepasste Bibliothek "PS2Keyboard" von Paul Stoffregen verwendet.
Die Dateien PS2Keyboard.cpp und PS2Keyboard.h wurden modifiziert.
Der Original-Quellcode steht unter: https://github.com/PaulStoffregen/PS2Keyboard

Als Hardware wird ein Arduino Nano Every eingesetzt, der auf Grund seiner geringen Größe im Tastaturgehäuse Platz hat. 
Die Stromversorgung erfolgt über das AC1-Anschlusskabel.
Ein 4-poliger Steckverbinder für die Programmierung des Arduino ist am Tastaturgehäuse angebracht.
Über die PINs D13, D12 und D19 können zusätzlich die Grafiktaste, AC1-Reset und AC1-NMI gesteuert werden.

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/PS2-Tastatur_mit_Text.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Arduino_Nano_mit_Text.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Arduino-Schaltbild.gif)
