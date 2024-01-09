# Block Escape
`Author: Jari Näser`

`Version: 09.01.2024`

## Periferiche necessarie
Per poter utilizzare questo sistema è necessario disporre dei seguenti componenti:

* 1x Scheda STM32F769I-DISCO
* 1x Joystick 
* 4x Cavi femmina-maschio

## Collegamento delle periferiche
Per collegare il joystick alla board basta collegare i seguenti pin del joystick alla board:

| Pin Board | Pin Joystick |
| -------- | ------- |
| 3.3V | 5V |
| GND | GND |
| AN/PA4    | VRx    |
| AN/PA6 | VRy |

## Codice sorgente e progetto
Il codice sorgente compreso nel progetto è stato fornito assieme alla consegna oppure è reperibile nella seguente repository GitHub: [link](https://github.com/JariNaeser/BlockEscape)

## Utilizzo
1. Se il programma non è ancora stato caricato sulla board farlo tramite il progetto allegato e il software STM32CubeIDE
2. Premere il bottone "user" per cominciare una nuova partita
3. Giocare muovendo il blocchetto centrale tramite il joystick
