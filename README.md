# cs-go-airsoft-bomb-v2

A second design iteration on a previous project of mine (https://github.com/VictorySpecificationII/cs-go-airsoft-bomb). In contrast to the older design, this iteration introduces a FSM (finite state machine) which greatly improves the overall flow of the program, and allows for greater flexibility and improved functionality. Moreover, using the SIM900 shield, the prop can send messages to the game organizers/referees to indicate the current state of the game (i.e unarmed/armed/planted/defused/exploded). I have reused some of the code from the previous project, and put everything in separate functions so the FSM looks as tidy as possible.

## Libraries Used
 
 To install, open your Arduino IDE and in the Libraries tab, search for the following terms:
  - LiquidCrystal I2C by Frank de Brabander, version 1.1.1
  - Keypad by Mark Stanley, Alexander Brevig, version 3.1.1

## Usage

Clone the repository, open the .ino file and upload to your Nano board! Simple as that. You can tweak various parameters in the GAME SECTION commented portion of the code.

## Bill Of Materials

 - 1x Arduino Nano
 - 1x Generic 4x4 keypad
 - 1x 16x2 LCD with an I2C interface
 - 1x Toggle Switch
 - 1x Pushbutton (not yet utilized)
 - 1x SIM900 Shield (not yet utilized)
 
 ## Wiring
  - Keypad (Left to Right) -> Arduino
     - Pin 1 -> D5
     - Pin 2 -> D6
     - Pin 3 -> D7
     - Pin 4 -> D8
     - Pin 5 -> D9
     - Pin 6 -> D10
     - Pin 7 -> D11
     - Pin 8 -> D12
  
  - 16x2 LCD (i2C Interface) -> Arduino
     - GND -> GND
     - VCC -> 5V
     - SDA -> A4
     - SCL -> A5

  - Toggle Switch -> Arduino
     - Left Pole -> D3
     - Middle Pole -> GND
     
  - Push Button -> Arduino
     - GND -> GND
     - VCC -> 5V
     - SIG -> D4
 
  - SIM900
    - Onboard Shield
      - J11 to J12
        - D0 Pin to Adjacent Pin in the middle
        - D1 Pin to Adjacent Pin in the middle
    - Onboard Shield Breakout Jumper -> Arduino
      - J17 to Arduino
        - TX -> D1
        - GND -> GND
      - J18 to Arduino
        - RX -> D0
