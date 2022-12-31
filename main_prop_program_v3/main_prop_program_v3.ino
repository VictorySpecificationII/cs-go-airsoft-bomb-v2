//https://www.instructables.com/Finite-State-Machine-on-an-Arduino/

#include "Arduino.h"

//.............................FSM SECTION......................................
static unsigned int state;

//.............................FSM SECTION......................................

//............................GAME SECTION......................................
char password[4]; // number of characters in our password
int currentLength = 0; //defines which number we are currently writing
int i = 0; 
char entered[4];

long hour = 23;
long minute = 59;
long second = 59;

long countdown_time = (hour*3600) + (minute * 60) + second;

//............................GAME SECTION......................................

//...........................KEYPAD SECTION.....................................
#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {5, 6, 7, 8}; 
byte colPins[COLS] = {9, 10, 11, 12}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//...........................KEYPAD SECTION.....................................


//............................LCD SECTION.......................................
#include <LiquidCrystal_I2C.h>

//define I2C address......
LiquidCrystal_I2C lcd(0x27,16,2);
//............................LCD SECTION.......................................

//..........................BUTTONS SECTION......................................

const int buttonPin = 4;  // the number of the pushbutton pin
const int togglePin = 3; // the number of the toggle switch pin
const int buzzerPin = 2; // the number of the buzzer pin
const int ledPin = 13;    // the number of the LED pin

int ledState = HIGH;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

//..........................BUTTONS SECTION........................................




void setup(){
    serial_comm_init();
    set_pin_modes();
    lcd_init();  
    motd();
    trigger_buzzer(125);
    misc_setup();
    Serial.println("State: Setup");
    state = 0;
}

void loop()
{
    switch (state){

    case 0: //round start
      Serial.println("State: 0");
      state = 1;
      break;      

    case 1: //disarmed
      Serial.println("State: 1");
      lcd.clear();
      trigger_buzzer(200);

      lcd.setCursor(5,0);
      lcd.print("Status");

      lcd.setCursor(4,1);
      lcd.print("DISARMED");  
      
      while(state == 1){
        delay(1000); //ghetto debouncing fk yeah
        if (digitalRead(togglePin) == HIGH){
          state = 2;
          break;
        }
      }

    case 2: //armed
      Serial.println("State: 2");

      print_arming_msg();

      while(state == 2){
        if(digitalRead(togglePin) == LOW){
          state = 1;
          break;
        }
        
        lcd.setCursor(0,0);
        lcd.print("Enter Sequence: ");

        while (currentLength < 4)
        {

          if(digitalRead(togglePin) == LOW){
            state = 1;
            break;
          }

          lcd.setCursor(currentLength + 6, 1);
          lcd.cursor();
          char key = customKeypad.getKey();
          key == NO_KEY;
          if (key != NO_KEY)
          {
            if ((key != '*')&&(key != '#'))
            { 
            lcd.print(key);
            Serial.println(key);
            password[currentLength] = key;
            currentLength++;
            }
          }
        }

        if (currentLength == 4)
        {
          delay(500);
          lcd.noCursor();
          lcd.clear();
          lcd.home();
          lcd.print("Defuse Code: ");
          lcd.setCursor(6,1);
          lcd.print(password[0]);
          lcd.print(password[1]);
          lcd.print(password[2]);
          lcd.print(password[3]);
  
          delay(3000);
          lcd.clear();
          currentLength = 0;
         
          state = 3;
          break;
        }


      }
      
    case 3: //deployed
      Serial.println("State: 3");
      while(state == 3){
        timer();
        defuse();
      }    	
      state = 4;
    	state = 5;
    	break;

    case 4: //exploded, round end
    	Serial.println("State: 4");
      print_explosion_msg();
      state = 0;
    	break;

    case 5: //defused, round end
    	Serial.println("State: 5");
      print_defuse_msg();
      state = 0;
    	break;

    }
}

//...................SETUP FUNCTIONS..................
void serial_comm_init(){
  Serial.begin(9600);
}

void set_pin_modes(){
    pinMode(buzzerPin, OUTPUT);//buzzer
    pinMode(buttonPin, INPUT);//button
    pinMode(ledPin, OUTPUT);//led
    pinMode(togglePin, INPUT_PULLUP);//toggle
}

void lcd_init(){
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void motd(){
  lcd.setCursor(2,0);
  lcd.print("Hello World!");

  lcd.setCursor(1,1);
  lcd.print("Bomb Prop v0.2");

  delay(1000);
  lcd.clear();
}

void trigger_buzzer(int beep_delay){
  digitalWrite(buzzerPin, HIGH); // sets the digital pin 13 on
  delay(beep_delay);            // waits for a second
  digitalWrite(buzzerPin, LOW);  // sets the digital pin 13 off
}

void misc_setup(){
  // set initial LED state
  digitalWrite(ledPin, ledState);
}

void print_arming_msg(){

  lcd.clear();

  lcd.setCursor(5,0);
  lcd.print("Status");

  lcd.setCursor(3,1);
  lcd.print("ARMING");
  delay(125);


  lcd.setCursor(5,0);
  lcd.print("Status");

  lcd.setCursor(3,1);
  lcd.print("ARMING.");
  delay(125);


  lcd.setCursor(5,0);
  lcd.print("Status");

  lcd.setCursor(3,1);
  lcd.print("ARMING..");
  delay(125);


  lcd.setCursor(5,0);
  lcd.print("Status");

  lcd.setCursor(3,1);
  lcd.print("ARMING...");
  delay(125);

  lcd.clear();
  trigger_buzzer(100);
  delay(100);
  trigger_buzzer(100);
  delay(100);
  trigger_buzzer(100);

  lcd.setCursor(5,0);
  lcd.print("Status");
  lcd.setCursor(5,1);
  lcd.print("ARMED");
  delay(125);

  delay(2000);
  lcd.clear();
}

void print_defuse_msg(){

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Bomb defused.");

  delay(3000);

  lcd.clear();
  trigger_buzzer(100);
  delay(100);
  trigger_buzzer(100);
  delay(100);
  trigger_buzzer(100);

  lcd.setCursor(5,0);
  lcd.print("CT's");
  lcd.setCursor(5,1);
  lcd.print("Win.");
  delay(125);
}

void print_explosion_msg(){

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Goodbye.");

  delay(3000);

  lcd.clear();
  trigger_buzzer(100);
  delay(100);
  trigger_buzzer(100);
  delay(100);
  trigger_buzzer(100);

  lcd.setCursor(5,0);
  lcd.print("T's");
  lcd.setCursor(5,1);
  lcd.print("Win.");
  delay(125);
}
//...................SETUP FUNCTIONS..................






//..................TIMING FUNCTIONS...................
int timer(){

  long countdowntime_seconds = countdown_time - (millis() / 1000);
  if (countdowntime_seconds >= 0) {
    long countdown_hour = countdowntime_seconds / 3600;
    long countdown_minute = ((countdowntime_seconds / 60)%60);
    long countdown_sec = countdowntime_seconds % 60;
    lcd.setCursor(4, 1);
    if (countdown_hour < 10) {
      lcd.print("0");
    }
    lcd.print(countdown_hour);
    lcd.print(":");
    if (countdown_minute < 10) {
      lcd.print("0");
    }
    lcd.print(countdown_minute);
    lcd.print(":");
    if (countdown_sec < 10) {
      lcd.print("0");
    }
    lcd.print(countdown_sec);
  }
  delay(500);

}//end timer

int defuse(){

  //timer();
  char currentKey = customKeypad.getKey(); // get the key
    
  if (currentKey == '*'){

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Code: ");
        
    while (currentLength < 4){
        
      timer();
      char currentKey = customKeypad.getKey();

      if (currentKey == '#'){
        currentLength = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Code: ");
      }//end if
          
      else if (currentKey != NO_KEY){
                
        lcd.setCursor(currentLength + 7, 0);
        lcd.cursor();
              
        lcd.print(currentKey);
        entered[currentLength] = currentKey;
        currentLength++;
        delay(100);
        lcd.noCursor();
        lcd.setCursor(currentLength + 6, 0);
        lcd.print("*");
        lcd.setCursor(currentLength + 7, 0);
        lcd.cursor();
    
      }//end if
    
    }//end while

  if (currentLength == 4){
    
    if (entered[0] == password[0] && entered[1] == password[1] && entered[2] == password[2] && entered[3] == password[3]){
    
      lcd.noCursor();
      lcd.clear();
      lcd.home();
      lcd.setCursor (0,0);
      lcd.print("The bomb has");
      lcd.setCursor (0,1);
      lcd.print("been defused.");

      currentLength = 0;
      delay(2500);
      lcd.setCursor(0,1);
      lcd.print("");
      delay(10000);
      state = 0;
    
    }//end if
    
    else{

      lcd.noCursor();
      lcd.clear();
      lcd.home();
      lcd.setCursor (0,0);
      lcd.print("=====WARNING=====");
      lcd.setCursor (0,1);
      lcd.print("-Wrong Password-");
      delay(1500);
      lcd.clear();
      
      currentLength = 0;
      
      }//end else

    }//end if

  }//end if

}//end defuse


//..................TIMING FUNCTIONS...................
