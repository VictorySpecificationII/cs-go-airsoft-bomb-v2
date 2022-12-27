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

int Scount = 5; // count seconds
int Mcount = 0; // count minutes
int Hcount = 0; // count hours
int DefuseTimer = 0; // set timer to 0
int Mpenalty = 5; //59 for 1 attempt boom, less for more

long secMillis = 0; // store last time for second add
long interval = 1000; // interval for seconds

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
      print_text_on_screen("Status", 5, 0);
      print_text_on_screen("DISARMED", 4, 1);
      while(state == 1){
        delay(1000); //ghetto debouncing fk yeah
        if (digitalRead(togglePin) == HIGH){
          state = 2;
          break;
        }
      }

    case 2: //armed
      Serial.println("State: 2");
      lcd.clear();
      print_text_on_screen("Status", 5, 0);
      print_text_on_screen("ARMING", 3, 1);
      delay(125);
      print_text_on_screen("ARMING.", 3, 1);
      delay(125);
      print_text_on_screen("ARMING..", 3, 1);
      delay(125);
      print_text_on_screen("ARMING...", 3, 1);
      delay(125);
      lcd.clear();
      print_text_on_screen("Status", 5, 0);
      print_text_on_screen("ARMED", 5, 1);
      delay(2000);
      lcd.clear();

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
      state = 0;
    	break;

    case 5: //defused, round end
    	Serial.println("State: 5");
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

//...................SETUP FUNCTIONS..................

//...................TEXT INPUT AND PRINT FUNCTIONS..................
char return_key_press(){
    char customKey = customKeypad.getKey();
  if (customKey){
    Serial.println(customKey);
    return customKey;
    }
}

void print_char_on_screen(char input, int horizontal, int vertical){
  lcd.setCursor(horizontal,vertical);
  lcd.print(input);
}

void print_text_on_screen(String input, int horizontal, int vertical){
  lcd.setCursor(horizontal,vertical);
  lcd.print(input);  
}

//...................TEXT INPUT AND PRINT FUNCTIONS..................

//...................BUTTON FUNCTIONS..................
int red_button_press(){
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        ledState = !ledState;
        if(ledState == LOW)
        Serial.println("LED Off");
        if(ledState == HIGH)
        Serial.println("LED On");
      }
    }
  }

  digitalWrite(ledPin, ledState);
  lastButtonState = reading;
  return 1;
}

int red_button_defuse(){
	int reading = digitalRead(buttonPin);
	while( reading == 1 ) //while the button is pressed
	{
		//blink
    Serial.println("Defuse Button Pressed");
		digitalWrite(ledPin,HIGH);
		delay(500);
		digitalWrite(ledPin,LOW);
		delay(500);
		reading = digitalRead(buttonPin); //refresh value of variable
	}
}
//...................BUTTON FUNCTIONS..................




//..................TIMING FUNCTIONS...................
void timer(){

  Serial.print(Scount);
  Serial.println();
    
    if (Hcount <= 0){
      
      if ( Mcount < 0 ){
      
        lcd.noCursor();
        lcd.clear();
        lcd.home();
        lcd.print("Goodbye.");
        lcd.setCursor (0,1);
        lcd.print("--------");

        delay(1000);

        lcd.clear();
        lcd.home();
        lcd.print("The bomb");
        lcd.setCursor (0,1);
        lcd.print("has exploded.");
    
        while (Mcount < 0){


          trigger_buzzer(90);
          delay(100); 
          trigger_buzzer(90);
          delay(100); 
          trigger_buzzer(90);
          delay(100); 
          trigger_buzzer(90);
          delay(100); 
          trigger_buzzer(90);
          delay(100); 
          trigger_buzzer(90);
          delay(100);

        }//end while

      }//end if

    }//end if

    lcd.setCursor (0,1); // sets cursor to 2nd line
    lcd.print ("Timer:");

    if (Hcount >= 10){
        lcd.setCursor (7,1);
        lcd.print (Hcount);
    }//end if
    
    if (Hcount < 10){
        lcd.setCursor (7,1);
        lcd.print ("0");
        lcd.setCursor (8,1);
        lcd.print (Hcount);
    }//end if

    lcd.print (":");

    if (Mcount >= 10){
        lcd.setCursor (10,1);
        lcd.print (Mcount);
    }//end if
    
    if (Mcount < 10){
        lcd.setCursor (10,1);
        lcd.print ("0");
        lcd.setCursor (11,1);
        lcd.print (Mcount);
    }//end if
      
    lcd.print (":");

    if (Scount >= 10){
        lcd.setCursor (13,1);
        lcd.print (Scount);
    }//end if

    if (Scount < 10){
        lcd.setCursor (13,1);
        lcd.print ("0");
        lcd.setCursor (14,1);
        lcd.print (Scount);
    }//end if

    if (Hcount <0){
        Hcount = 0; 
    }//end if

    if (Mcount <0){
        Hcount --; 
        Mcount = 59; 
    }//end if

    if (Scount <1){ // if 60 do this operation
        Mcount --; // add 1 to Mcount
        Scount = 59; // reset Scount
    }//end if

    if (Scount > 0){ // do this oper. 59 times
      unsigned long currentMillis = millis();
    
      if(currentMillis - secMillis > interval){
        trigger_buzzer(200);
        trigger_buzzer(300);
            
        secMillis = currentMillis;
        Scount --; // add 1 to Scount
        delay(10); // waits for a second
        delay(10); // waits for a second
        //lcd.clear();

    }//end if

  }//end if

}//end timer

void defuse(){

  timer();
  char key2 = customKeypad.getKey(); // get the key
    
  if (key2 == '*'){

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Code: ");
        
    while (currentLength < 4){

      timer();
      char key2 = customKeypad.getKey();

      if (key2 == '#'){
        currentLength = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Code: ");
      }//end if
          
      else if (key2 != NO_KEY){
                
        lcd.setCursor(currentLength + 7, 0);
        lcd.cursor();
              
        lcd.print(key2);
        entered[currentLength] = key2;
        currentLength++;
        trigger_buzzer(200);
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
      delay(1000000);
    
    }//end if
    
    else{

      lcd.noCursor();
      lcd.clear();
      lcd.home();
      lcd.setCursor (0,0);
      lcd.print("=====WARNING=====");
      lcd.setCursor (0,1);
      lcd.print("-Wrong Password-");
      delay(3000);
      lcd.clear();
      
      if (Hcount > 0){
        Hcount = Hcount - 1;
      }
    
      if (Mcount > 0){
        Mcount = Mcount - Mpenalty;
      }
      
      if (Scount > 0){
        Scount = Scount - Mpenalty;
      }

      delay(1500);
      currentLength = 0;
      
      }//end else

    }//end if

  }//end if

}//end defuse


//..................TIMING FUNCTIONS...................
