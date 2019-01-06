
#include <EEPROM.h>                                                   //Library used to keep memory stored in internal storage when the board is turned off 
#include <LiquidCrystal.h>                                            //Library for 1602A-4 LCD screen 

#define RSPin 6                                                       //Register Select (RS)        //(yellow wire) Pin 4 on LCD -> Pin 6 on Mega
#define EnablePin 7                                                   //Clock (Enable pin)          //(orange wire) Pin 6 on LCD -> Pin 7 on Mega
#define DS4 8                                                         //Bit 4                       //(green wire)  Pin 11 on LCD -> Pin 8 on Mega
#define DS5 9                                                         //Bit 5                       //(blue wire)   Pin 12 on LCD -> Pin 9 on Mega
#define DS6 11                                                        //Bit 6                       //(white wire)  Pin 13 on LCD -> Pin 10 on Mega
#define DS7 12                                                        //Bit 7                       //(yellow wire) Pin 14 on LCD -> Pin11 on Mega
#define LCDColumns 16                                                 //Number of columns on screen
#define LCDRows 2                                                     //Number of rows on screen

LiquidCrystal lcd (RSPin, EnablePin, DS4, DS5, DS6, DS7);             //Specify all pins used in the liquidCrystal library                                                        

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// TOTAL INPUTS AND OUTPUTS 
#define Total_LEDS       4                                            //number of leds total (red, white, green, blue)
#define Total_PBS        4                                            //number of PS's total (red, white, green, blue)

//OUTPUTS:
// LED Configuration
#define Red_LED          40                                           // LED'S connected to digital pin's 40, 41, 42, 43 of mega
#define White_LED        41     
#define Green_LED        42      
#define Blue_LED         43      

//INPUTS:
// Push Button Configuration
#define Red_PB           44                                           // LED'S connected to digital pin's 44, 45, 46, 47 of mega
#define White_PB         45     
#define Green_PB         46      
#define Blue_PB          47      

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//boolean values are either TRUE, FALSE, or an ERROR has occured
typedef enum                                                          //storage class
   {
   TRUE,                                                              //type specifier
   FALSE,
   ERROR
   } 
   eBoolean ;                                                         //declarator
   
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Checking if a player presses multiple buttons
// Checking if a button is triggered & released (de-bouncing)
#define Multiple_PB_Triggered       FALSE                             //TRUE reading when multiple PB triggered
#define PB_Triggered_Check          FALSE                             //check logic
char print_output[256] ;                                              //debugging in serial monitor

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//First output computer sequence , Second Checks Player sequence
#define Computer_Sequence_Output    FALSE                             //TRUE when computer output is called
#define Player_Sequence_Input       FALSE                             //TRUE when player is entering sequence

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// DELAYS
// easy = 400 ms, medium = 200 ms, hard = 100ms 
#define Easy_Delay          400                                       // LED ON/OFF timing
#define Medium_Delay        200                                       
#define Hard_Delay          100    
#define ROUND_PAUSE         200                                       // pause between rounds

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//SETUP 
#define Random_Seed         25001                                     // Random number 
#define Max_Game_Level      20                                        // Max number of levels (one flash added per level)
#define Debounce_Passes     20                                        // number of passes before registering the push

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// GAME ARRAYS
int LED_Array [Total_LEDS] ;                                          // LED sequence (4 outputs total)
int PB_Array [Total_PBS] ;                                            // PB sequence  (4 inputs total)
int Game_Level_Array [ Max_Game_Level ] ;                             // The sequence of LEDS to light each level (20 levels total) 
int Current_Level ;                                                   // The current level player is on 
int Current_Level_Display ;                                           // Serial monitor display
int Stored_High_Score =0 ;                                            // Highest level reached kept in internal storage
byte High_Score;                                                      //Value of Stored_High_Score in internal storage

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {                                                        // put your setup code here, to run once:

    Serial.begin(9600);                                               //Initialize the serial communications:
    
    lcd.begin(LCDColumns, LCDRows);                                   //Configure the LCD
    lcd.setCursor(1,0);                                               //Start on first row, position " 1 "
    lcd.print("  SIMON SAYS  ");
    lcd.setCursor(0,1);                                               //Next, go to second row, position " 0 "
    lcd.print("By:Ashley Mooney ");
    delay (3000);                                                     //Pause so user sees authour and game title 
    
    LED_Array [0] = Red_LED;                                          //Config LED array positions 
    LED_Array [1] = White_LED;
    LED_Array [2] = Green_LED;
    LED_Array [3] = Blue_LED;
    
      for (int A = 0 ; A < Total_LEDS ; A++)                          //for all LED's starting at zero (with a total of 4) , 0,1,2,3 in array set too:
      {
        pinMode (LED_Array [A], OUTPUT);                              //Sets all LEDS pins as OUTPUTS
      }

    PB_Array [0] = Red_PB;                                            //Configure PB array positions
    PB_Array [1] = White_PB;
    PB_Array [2] = Green_PB;
    PB_Array [3] = Blue_PB;
    
      for (int B = 0 ; B < Total_LEDS ; B++)                          //for all PB's starting at zero (with a total of 4) , 0,1,2,3 in array set too:
      {
        pinMode (PB_Array [B], INPUT);                                //Sets all PB pins as INPUTS
      }
  
     for (int C = 0 ; C < Total_LEDS; C++)                            
     {
        digitalWrite(LED_Array[C], HIGH) ;                            // flashes the LEDs TO TEST
        digitalWrite(LED_Array[C], LOW) ;
     }   
   
   srand(Random_Seed) ;                                              //Initialize the pseudo-random number generator by passing the argument seed
                                                                     //Maybe add in future : function time , as input for seed to randomize 
                                                                     
   New_Game() ;                                                      // Jump to new game routine

            }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
 
  void New_Game( void )                                               //Starts up a new game of SIMON.
  {
     int D ; 
     
     for (D=0 ; D < Max_Game_Level ; D++)                             //Game array of LED's starts at lowest value, remains under 20, increments 
     {
        Game_Level_Array[D] = (rand() % Total_PBS) ;                  //Number result (between 0 and (Total_PBS- 1) is the position in the led_array to light on that turn.
        //  Serial.print(Game_Level_Array[D]);                        //Fill the game array with a new sequence ofLEDs to light.
     }
    
     Current_Level = 0 ;                                              //The current_round is reset to zero
  }
  
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Computer_Output( void )                                          //Calls / Displays the LED array at each level
{
   
int Computer_Step ;                                                   //Integer for actual step in array the computer is outputting

   if ( TRUE == Computer_Step )                                     
   {
   }
   
   for ( Computer_Step=0 ;                                            
         ((Computer_Step < Max_Game_Level) &&                        //If under 20 levels, and computer hasnt executed current round LED pattern
         ( Computer_Step <= Current_Level)) ;            
         Computer_Step++)                                            //Add to computers current Level
   {
      if ( TRUE == Computer_Step )                                   //Output sequence generating logic, goes TRUE when loop is running
      {
      }
      Computer_Output_Level_Speed( Game_Level_Array [Computer_Step]);//flashes the LED array up to the computers current level
   }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Computer_Output_Level_Speed (int LED_Output)
{
   
int Yellow;                                                           //Three wires for three way level switch
int Blue;
int Green;
int (Current_Level_Display = (Current_Level+1));                      //Since array starting at 0, Display must start at 1

  //Serial.print(LED_Output);                                         //cheat to make it to higher levels via serial display
  
  Yellow = analogRead(A1);                                            // read the input on analog pin 1:
  float Yellow_Voltage1 = Yellow  * (5.0 / 1023.0);                   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
//  Serial.print("Yellow Wire: ");Serial.println(Yellow_Voltage1);    // print out the value you read (in Volts):
  delay(50);                                                          //Wait a second between displaying voltages on serial monitor.


  Blue  = analogRead(A2);                                             // read the input on analog pin 2:
  float Blue_Voltage2 = (Blue * (5.0 / 1023.0))-1;                    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
//  Serial.print("Blue Wire:    ");Serial.println(Blue_Voltage2);     // print out the value you read (in Volts):
  delay(50);                                                          //Wait a second between displaying voltages on serial monitor.


  Green = analogRead(A3);                                             // read the input on analog pin 3:
  float Green_Voltage3 = Green * (5.0 / 1023.0);                      // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
//  Serial.print("Green Wire:    ");Serial.println(Green_Voltage3);   // print out the value you read (in Volts):
  delay(50);                                                          //Wait a second between displaying voltages on serial monitor.

//HARD MODE!!!!!
    if 
        (Blue_Voltage2 >= 3.90)
        {
          lcd.begin(LCDColumns, LCDRows);                             //Refresh LCD screen (used for if difficuly level changed)
          lcd.setCursor(0,1);                                         //Start cursor on second row, column " 0 "
          lcd.print("HARD");                                          //Specify Level = HARD
          lcd.setCursor(7,1);                                         //Next cursor go to second row, column " 7 "
          lcd.print("LEVEL: ");
          lcd.print(Current_Level_Display);                           //Display current level being played
          lcd.setCursor(0,0);                                         //Starts cursor on first row, column " 0 "
          lcd.print("HIGHSCORE:");                                    //Print overall highscore
          lcd.setCursor(11,0);                                        //Next, cursor go to first row, column " 11 "
          lcd.print (EEPROM.read(Stored_High_Score));                 //Displays high score saved in memory

          digitalWrite(LED_Array[LED_Output], HIGH) ;                 // sets the LED in current round on for 100ms
          delay(Hard_Delay) ;                                                 
          digitalWrite(LED_Array[LED_Output], LOW) ;                  // sets the LED in current round off for 100 ms
          delay(Hard_Delay) ;                                            
          }

//MEDIUM MODE!!!!!
    else if 
     //   ((Yellow_Voltage1 == 5.00) && ( (Blue_Voltage2 >= 2.00)     //Yellow = 5v , Blue= 2-4v , Green = 0v
      //  || (4.00 <= Blue_Voltage2) ) && (Green_Voltage3 ==0.00) ) { 
        ( ( Blue_Voltage2 >= 1.50 ) && (Blue_Voltage2 <= 3.80) ) 
        { 
          lcd.begin(LCDColumns, LCDRows);                             //Refresh LCD screen (used for if difficuly level changed)
          lcd.setCursor(0,1);                                         //Start cursor on second row, column " 0 "
          lcd.print("MEDIUM");                                        //Specify Level = MEDIUM
          lcd.setCursor(7,1);                                         //Next cursor go to second row, column " 7 "
          lcd.print("LEVEL: ");
          lcd.print(Current_Level_Display);                           //Display current level being played
          lcd.setCursor(0,0);                                         //Starts cursor on first row, column " 0 "
          lcd.print("HIGHSCORE:");                                    //Print overall highscore
          lcd.setCursor(11,0);                                        //Next, cursor go to first row, column " 11 "
          lcd.print (EEPROM.read(Stored_High_Score));                 //Displays high score saved in memory
      
          digitalWrite(LED_Array[LED_Output], HIGH) ;                 // sets the LED in current round ON for 200 ms
          delay(Medium_Delay) ;                                                     
          digitalWrite(LED_Array[LED_Output], LOW) ;                  // sets the LED in current round OFF for 200 ms
          delay(Medium_Delay) ;
         }

//EASY MODE!!!!!
    else{
          lcd.begin(LCDColumns, LCDRows);                             //Refresh LCD screen (used for if difficuly level changed)
          lcd.setCursor(0,1);                                         //Start cursor on second row, column " 0 "
          lcd.print("EASY");                                          //Specify Level = EASY
          lcd.setCursor(7,1);                                         //Next cursor go to second row, column " 7 "
          lcd.print("LEVEL: ");
          lcd.print(Current_Level_Display);                           //Display current level being played
          lcd.setCursor(0,0);                                         //Starts cursor on first row, column " 0 "
          lcd.print("HIGHSCORE:");                                    //Print overall highscore
          lcd.setCursor(11,0);                                        //Next, cursor go to first row, column " 11 "
          lcd.print (EEPROM.read(Stored_High_Score));                 //Displays high score saved in memory
              
          digitalWrite(LED_Array[LED_Output], HIGH) ;                 // sets the LED in current round ON for 400 ms
          delay(Easy_Delay) ;                                               
          digitalWrite(LED_Array[LED_Output], LOW) ;                  // sets the LED in current round OFF for 400 ms
          delay(Easy_Delay) ;  
    }

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

int Check_Multiple_PB_Triggered                                       // Checks for human error
                                    ( int Red_PB_State,               //initialize each PB's state 
                                      int White_PB_State,
                                      int Green_PB_State,
                                      int Blue_PB_State )
{
                                                                      
   int Multiple_Buttons_Pressed = TRUE ;                              //TRUE  = Multiple buttons were pressed  FALSE = one or 0 buttons were pressed.
   int Total_PBS_Held = 0 ;                                           //Set to zero because PB normally opened contacts, energized when pressed

   if ( TRUE == Multiple_PB_Triggered )
   {

   }
   
                                                                      // Check if any PB are pressed
   if ( Red_PB_State == HIGH )                                        // If any of the PB's are energized, add one too Total_PBS_Held integer 
   {
      Total_PBS_Held++ ;
   }
   if ( White_PB_State == HIGH )
   {
      Total_PBS_Held++ ;
   }
   if ( Green_PB_State == HIGH )
   {
      Total_PBS_Held++ ;
   }
   if ( Blue_PB_State == HIGH )
   {
      Total_PBS_Held++ ;
   }
                                                                     
   if (  Total_PBS_Held > 1 )                                         // TRUE if more than 1 button is active at once
   {
       Multiple_Buttons_Pressed = TRUE ;
   }
   else
   {
      Multiple_Buttons_Pressed = FALSE ;                              //if not more than 1 button is active, return false. (normal operation)
   }
   return Multiple_Buttons_Pressed ;                                  //returns T or F
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

int Player_Input( void )
{
   
   int PB = 0 ;                                                       //Notes: Pressed push button == HIGH, released button == LOW 
   int Player_Input_Array_Step = 0 ;                                  //Always start the player at step one of input array
   int Multiple_Buttons_Not_Active = -1 ;                                                                        

   while ( (Player_Input_Array_Step <= Current_Level) &&              // when player level is below the index of the current level and there is no errors
           (Multiple_Buttons_Not_Active != FALSE) )                   //must be -1 to continue while statement //First: a PB press must be registered
   {

      PB = Get_PB_Press() ;                                           //jump to routine and retrieve current PB triggered 
      
      if ( -1 == PB )                                                 // if -1 error has occured
      {
         return FALSE ;         
      }
      digitalWrite( LED_Array[(PB-1)], LOW ) ;                        //if no error turn on appropriate LED
      digitalWrite( LED_Array[(PB-1)], HIGH ) ;
      Serial.print (PB);
      

//Second: then the PB release must be registered
      PB = Get_PB_Release(PB) ;                                       //jump to routine and retrieve when current PB released "Last_pb_Pressed"
      if ( -1 == PB )
      {
        
      digitalWrite( LED_Array[(PB-1)], HIGH ) ;                       //turn on appropriate LED
      digitalWrite( LED_Array[(PB-1)], LOW ) ;                        //turn off appropriate LED
         return FALSE ;       
      }

      
      digitalWrite( LED_Array[(PB-1)], HIGH ) ;                       //turn on appropriate LED
      digitalWrite( LED_Array[(PB-1)], LOW ) ;                        //turn off appropriate LED

//Third: the button that was just pressed and released must be checked against the expected answer. 
      if ( TRUE == Check_PB_Entered(PB, Player_Input_Array_Step))                //TRUE player input is correct for the present sequence index
      {
         Player_Input_Array_Step++ ;                                             //if player passes the check testing loop ... level up
      }
      else
      {
          Multiple_Buttons_Not_Active= FALSE ;                                       //if player doesnt pass the check testing loop ... game over
      }
   }                                                                  // end loop

   if ((Player_Input_Array_Step - 1) == Current_Level)           
   {
      Multiple_Buttons_Not_Active = TRUE ;                           
   }
   else
   {
      Multiple_Buttons_Not_Active = FALSE ;                          
   }
   
   return  Multiple_Buttons_Not_Active ;                                             // If the player input the worng answer or pressed more than one pushbutton, then the playersturn ends and the game is over.
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

int Get_PB_Press( void )                                              //Loops until a press passes the debounce cycle
{

// Variables will change:
   int Debounce = 0 ;
   int Current_Button_Pressed = 0 ;                                   // The current reading from the input pins
   int Last_Button_Pressed = 0 ;                                      // The previous reading from the input pins
   int Red_Button_1 = LOW ;                                           // All buttons are set too LOW (When not pressed) because they are normally open contacts
   int White_Button_2 =LOW ;
   int Green_Button_3 = LOW  ;
   int Blue_Button_4 = LOW  ;
   int Multi_Press = FALSE ;                                          //True if "Check_Multiple_PB_Triggered" routine returned a true
   
   while (Debounce != Debounce_Passes)                                //Does 20 debounces passes as a check
   {
      Red_Button_1 =    digitalRead ( PB_Array[0] ) ;                 //reads push button signals, positions of PB array: 0 is red, 1 is white, 2 is green, 3 is blue
      White_Button_2 =  digitalRead ( PB_Array[1] ) ;
      Green_Button_3 =  digitalRead ( PB_Array[2] ) ;
      Blue_Button_4 =   digitalRead ( PB_Array[3] ) ;

      Multi_Press = Check_Multiple_PB_Triggered   ( Red_Button_1,
                                                    White_Button_2,
                                                    Green_Button_3,
                                                    Blue_Button_4 ) ;

      if ( TRUE == Multi_Press)
      {
         return -1 ;                                                  // -1 = player error (most likely multiple keys pressed)
      }
      
      if (HIGH == Red_Button_1)                                       //If a button is pressed, it stores 0,1,2, or 3 into the PB array for red,white,green,blue respectively
      {
         Current_Button_Pressed= 1 ;
         Serial.print("R") ;                                          //For debugging / watching de-bounce when buttons pressed from serial monitor
      }
      else if (HIGH == White_Button_2)
      {
         Current_Button_Pressed = 2 ;
         Serial.print("W") ;
      }
      else if (HIGH== Green_Button_3)
      {
         Current_Button_Pressed= 3 ;
         Serial.print("G") ;
      }
      else if (HIGH== Blue_Button_4)
      {
         Current_Button_Pressed = 4 ;
         Serial.print("B") ;
      }
      else                                                             // no button pressed
      {
         Current_Button_Pressed = 0 ;
      }

      if ( (Last_Button_Pressed  == Current_Button_Pressed )           //current button being pressed should always be zero, if the last button was between 1-4
            && (Current_Button_Pressed != 0 ) )                        //So if current and last button  values are both = 1 (red) it didnt go back to zero 
                                                                       //debounce 
      {
         Debounce++ ;
      }
      else
      {
         Last_Button_Pressed = Current_Button_Pressed ;                //If the last button is ur current button, reset your debounce check to 0
         Debounce = 0 ;
      }
   }

   return Last_Button_Pressed ;                                        // after it checked 5 times, return the button reading 
                                                                       //a return between 1 annd 4 inclusive is given to signal the button
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

int Get_PB_Release(int Original_PB)                                    //This routine processes loops until a key release passes the debounce period.
{
   int Multiple_Buttons_Not_Active = Original_PB ;
   int Released_PB = 0 ;
   int Debounce = 0 ;                                                  //Does 5 debounce checks/ passovers
   
   int Red_Button_1 = HIGH ;                                           //All buttons are set too HIGH (When being pressed) because they are normally open contacts
   int White_Button_2 = HIGH ;
   int Green_Button_3 = HIGH;
   int Blue_Button_4 = HIGH;
   int Multi_Press = TRUE ;                                            //Only goes true if multiple PB pressed
   int Read_PB = 0 ;
     
   while ( Debounce != Debounce_Passes )                               //Does 5 debounces passes as a check
   {
      Red_Button_1 =    digitalRead ( Red_PB  ) ;                      //Reads push button signals (1-4 number of the button pressed.)
      White_Button_2 =  digitalRead ( White_PB  ) ;
      Green_Button_3 =  digitalRead ( Green_PB  ) ;
      Blue_Button_4 =   digitalRead ( Blue_PB  ) ;
      
      Multi_Press = Check_Multiple_PB_Triggered     ( Red_Button_1,    //Checks for Multiple PB Triggered 
                                                    White_Button_2,
                                                    Green_Button_3,
                                                     Blue_Button_4) ;

      if ( TRUE == Multi_Press)                                        // if there was multiple pb's pressed return a "-1"
      {
         return -1 ;                                                   // -1 = player error (most likely multiple keys pressed)
      }
        
      switch (Original_PB)
      {
         case 1:
            Read_PB = Red_Button_1 ;                                   // Red PB released 
            break ;
         case 2:
            Read_PB  = White_Button_2 ;
            break ;
         case 3:
            Read_PB = Green_Button_3 ;
            break ;
         case 4:
            Read_PB  = Blue_Button_4 ;
            break ;
         default:

            return -1 ;
            break ;
      }
      
      if (Read_PB == LOW)                                             //if a LOW value comes from a PB being triggered, debounce
      {
         Debounce++ ;
      }     
   } 
   
   return Multiple_Buttons_Not_Active ;                               //returns either a TRUE or a FALSE
                                                                      //Otherwise a return between 1 annd 4 inclusive is given to signal the button
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

int Check_PB_Entered(int Recieved_PB, int Player_Input_Array_Step)    //checks the physical input received from the player against the LED that flashed in each step
{
   int Press_Value = TRUE ;                                           

   if ( (Recieved_PB -1) == Game_Level_Array[Player_Input_Array_Step])// (-1 to PB number because in the code arrays start at 0)
                                                                      //Checks step player inputed against what is in the array at that step
   {
      Press_Value = TRUE ;                                            //TRUE player input is correct for the present sequence index
   }
   else
   {
      Press_Value = FALSE ;                                           //FALSE player inputed the incorrect answer
   }
      
   return Press_Value ;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop()                                                           //Code runs repeatedly
{
   int i ; 
   int Player_WON_Level = FALSE ;                                     // Only turns TRUE when round is beat
   int Mistake = FALSE ;                                              // Only turns TRUE if mistake is made
sprintf( print_output, "\r\nStarting Round %d\r\n", Current_Level) ;  //writes " starting round " and current round on serial monitor
//   Serial.print(print_output) ; 
   // lcd.begin(LCDColumns, LCDRows);   
  //  lcd.setCursor(1,0);                                             //Start on first row, position " 0 "
  //  lcd.print("  SIMON SAYS  ");                                      
  //  lcd.setCursor(0,1);                                             //Next, go to second row, position " 0 "
  //  lcd.print("Starting Round");
  // delay(50);
// read a byte from the current address of the EEPROM
  High_Score= EEPROM.read(Stored_High_Score);

  Serial.print(Stored_High_Score);
  Serial.print("\t");
  Serial.print(High_Score, DEC);                                        //Prints Highest Level reached in decimal format
  Serial.println();

if (EEPROM.read(Stored_High_Score) < Current_Level)
{
  EEPROM.write(Stored_High_Score, Current_Level);
     
}

  
   Computer_Output() ;                                                //First Computer output loop is called
 Serial.print(print_output);
 // delay( ROUND_PAUSE ) ;


   Player_WON_Level = Player_Input() ;                                //Player input returns a true value if player gets sequence right
   if ( Player_WON_Level == TRUE )                                  
   {
      Mistake = FALSE ;     
                                            
       delay( ROUND_PAUSE ) ;
   }
   else                                                               //if ( FALSE == player_passed_round )
   {
      Mistake = TRUE ;
   }
    
     if  ( Current_Level == Max_Game_Level-1)  //FIXXEDDDDDDD                     //if at level 20, then won the game
 
  {
   
    Serial.print("winning loop");
 
      for (i =0 ; i < 5 ; i++) //number of led cycles
      {
         for (int j = 0 ; j < Total_LEDS ; j++)                       //blink LEDS if win
         {
            digitalWrite(LED_Array[j], HIGH) ;
            delay(100) ; //500
           //  if ( (Current_Level = Max_Game_Level) && ( Mistake = FALSE) )
              lcd.setCursor(0,1);                                               //Next, go to second row, position " 0 "
              lcd.print("WINNER YAYYY");
              
         }
         for (int j = 0 ; j < Total_LEDS ; j++)
         {
            digitalWrite(LED_Array[j], LOW) ;
            delay(100) ; //500
         }
      }
      Current_Level++;
      Mistake = FALSE ;                                               // if no mistakes made, but at round 20, start new game
     
      New_Game() ;
   }
   else if ( Mistake == TRUE )                                        // If the player makes an error, diplay "game over" on LCD screen
   {
        
 //   lcd.begin(LCDColumns, LCDRows);                                 //Configure the LCD
   lcd.begin(LCDColumns, LCDRows);                                    //Refresh LCD screen (used for if difficuly level changed)
    lcd.setCursor(1,0);                                               //Start on first row, position " 0 "
    lcd.print("  SIMON SAYS  ");
    lcd.setCursor(3,1);                                               //Next, go to second row, position " 0 "
    lcd.print("Game Over!  ");
    
   //  EEPROM.write(Stored_High_Score, Current_Level);
     
    for (i =0 ; i < 10 ; i++)                                         //Run losing LED pattern "LOSE_Sequence" 10 cycles
      {
         LOSE_Sequence() ;
      }
      Mistake = FALSE ;
      New_Game() ;                                                    // Start a new game if game is over
   }
 
   else
   {
      Current_Level++ ;                                               //if no mistakes made, and not at level 20, keep leveling up
          
       delay(1000) ;
   }
  
 delay(100) ;  
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LOSE_Sequence( void )                                            //Visual LED pattern when Players lose 
{
    for (int E = 0 ; E < Total_LEDS ; E++)                             
   {
      digitalWrite( LED_Array[E], HIGH ) ;                            //Trig ON each LED output in array 
   }
   delay( 100 ) ;                                                     //delay LEDs stay ON (100ms) 
    
   for (int E = 0 ; E < Total_LEDS ; E++)                              
   {
      digitalWrite( LED_Array[E], LOW ) ;                             //Trig OFF each LED output in array
   }
   delay ( 100 ) ;                                                    //delay LEDs stay OFF (100ms) 
}
