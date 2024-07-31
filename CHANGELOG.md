## Update vom 31.07.2024

### Neue Funktionen für Tastaturen ohne Nummernblock

- **AltGr+G** &rarr; Grafiktaste ein- / ausschalten

- **AltGr+T** &rarr; Steuerung von PA7: originale AC1-Tastatur / 40ms-Impulse

### Zusätzliche Tastencodes für ESC, TAB, Backspace

|               | AC1-Modus | CP/M-Modus | mit Shift | mit Ctrl |
| ------------- | --------- | ---------- | --------- | -------- |
| **ESC-Taste** | ^C        | ^&#91;     | ^&#91;    |          |
| **TAB-Taste** | ^O        | ^O         | ^I        |          |
| **Backspace** | 0x7F      | ^H         | 0x7F      | 0x1F     |

### Zusätzliche Tastencodes im Steuerblock

|                  | AC1-Modus | CP/M-Modus | mit Shift | mit Ctrl |
| ---------------- | --------- | ---------- | --------- | -------- |
| **Pos1-Taste**   | ^A        | ^QS        | ^QR       | ^QR      |
| **Ende-Taste**   | ^C        | ^QD        | ^QC       | ^QC      |
| **&larr; Taste** | ^H        | ^S         | ^A        | ^A       |
| **&rarr; Taste** | ^I        | ^D         | ^F        | ^F       |
