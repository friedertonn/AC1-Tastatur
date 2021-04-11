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

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Tastatur.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Nano.jpg)

![](https://www.ftonn.de/GIT-Projekte/AC1-Tastatur/Schaltbild.gif)
