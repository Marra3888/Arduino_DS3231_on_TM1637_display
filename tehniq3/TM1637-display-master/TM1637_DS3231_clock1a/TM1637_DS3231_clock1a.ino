// base sketch from https://brainy-bits.com/tutorials/4-bits-7-segment-led-display-with-arduino/
// changed TM1637 library by niq_ro for degree, r & h letter
// DS3231 clock on TM1637 LED display by niq_ro from http://www.tehnic.go.ro
// & http://arduinotehniq.com/


#include "RotaryEncoder.h" // from http://www.pjrc.com/teensy/td_libs_Encoder.html
//RotaryEncoder knob(4, 5); //encoder connected to pins 2 and 3 (and ground)
RotaryEncoder rotaryEncoder = RotaryEncoder();  // CLK = pin 2, DAT = pin 3, SW = pin A1, CW = count up, CCW = count down


#include <TM1637.h>
#include <OneButton.h>
// The actions I ca do...
typedef enum 
{
  ACTION_OFF,  // set LED "OFF".
  ACTION_ON,   // set LED "ON"
  ACTION_SLOW, // blink LED "SLOW"
  ACTION_FAST  // blink LED "FAST"
} MyActions;

// Setup a new OneButton on pin A1.  
OneButton button(A1, true);
MyActions nextAction = ACTION_OFF; // no action when starting

enum Regim
{
  Time,
  MenuHour,
  MenuMinute
};

//{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
//0~9,A,b,C,d,E,F,"-"," ",degree,r,h

#define CLK 5//Pins for TM1637       
#define DIO 4
TM1637 tm1637(CLK,DIO);

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <RTClib.h>
//RTC_DS3231 rtc;
RTC_DS3231 rtc;

byte hh, mm, old_hour, mode = Time; 
/*
// Эти переменные предназначены для процедуры нажатия кнопки
int buttonstate = 0; //флаг, чтобы увидеть, была ли нажата кнопка, используется только внутри подпрограммы
int pushlengthset = 2500; // значение для длительного нажатия в мс
int pushlength = pushlengthset; // установить длину по умолчанию
int pushstart = 0;// устанавливает значение толчка по умолчанию для кнопки, идущей в низкий уровень
int pushstop = 0;// устанавливает значение по умолчанию, когда кнопка возвращается в высокое положение

int knobval; // значение для поворота ручки
boolean buttonflag = false; // значение по умолчанию для флага кнопки
int sethourstemp;
int setminstemp;
*/
#define goarna 7 // buzzer
//-------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  tm1637.init();
  tm1637.set(4); 
  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  rtc.begin();
// manual adjust
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
// automatic adjust
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

//   pinMode(buton,INPUT);//push button on encoder connected 
//  digitalWrite(buton,HIGH); //Pull at 1 high
  rotaryEncoder.setRange(0,255);
  Serial.begin(9600);
  Serial.println(F("E02 KY-040 RotaryEncoder Demo"));

  
   pinMode(goarna, OUTPUT);   
   digitalWrite(goarna, LOW); //set in 0 logical level
//   tone(goarna, 4000, 80); 


  // enable the standard led on pin 13.
  pinMode(13, OUTPUT);      // sets the digital pin as output

  // связать функцию myClickFunction, которая будет вызываться при событии щелчка.   
  button.attachClick(myClickFunction);

  // свяжите функцию двойного клика для вызова на событии двойного клика.   
  button.attachDoubleClick(myDoubleClickFunction);

  // установить 80 мсек. время подведения итогов По умолчанию 50 мсек.
  button.setDebounceTicks(80);
}//end "setup()"
//------------------------------------------------------------------------------------------------------------------------------- 
void loop()
{

      switch(mode)
     {
            case Time: 
                        READ_TIME();
                        if ((hh/10) == 0)   To_Display(17, hh%10, mm/10, mm%10);
                        else  To_Display(hh/10, hh%10, mm/10, mm%10);
                        blinking_dots();
      
            break;
            case MenuHour:
                        READ_TIME();
                        To_Display(hh/10, hh%10, 16, 16);
      
            break;
            case MenuMinute:
                        READ_TIME();
                        To_Display(16, 16, mm/10, mm%10);
      
            break;
            case 3:
      
            break;
            case 4:
      
            break;
            case 5:
      
            break;
            case 6:
                    
            break;
            default:  
                    mode = 0;
    }
      LED();
                                              
                                              //tm1637.point(POINT_ON);
                                              //now.second()%0.5 ? tm1637.point(POINT_ON) : tm1637.point(POINT_OFF);
      

                                              //delay(500);
                                            /*
                                             pushlength = pushlengthset;
                                                pushlength = getpushlength ();
                                                delay (10);
                                                
                                                if (pushlength > pushlengthset)
                                                {
                                                tone(goarna, 2000,50);
                                                //   ShortPush ();   
                                                }
                                                  
                                                   //This runs the setclock routine if the knob is pushed for a long time
                                                   if (pushlength < pushlengthset) {
                                                    tone(goarna, 1000,50);
                                                   //  delay(500);
                                                     DateTime now = rtc.now();
                                                     sethourstemp=now.hour(),DEC;
                                                     setminstemp=now.minute(),DEC;       
                                                     setclock();
                                                     pushlength = pushlengthset;
                                                   };
                                            */
                                            /*
                                                tm1637.point(POINT_OFF);
                                            if ((hh/10) == 0) tm1637.display(0,17);
                                            else
                                                tm1637.display(0,hh/10);     // hour
                                                tm1637.display(1,hh%10);
                                                tm1637.display(2,mm/10);    // minutes
                                                tm1637.display(3,mm%10);    // 
                                            //delay(500);
                                            */
}// end loop() 
//-------------------------------------------------------------------------------------------------------------------------------
void To_Display(byte a, byte b, byte c, byte d)
{
/*            if ((hh/10) == 0) tm1637.display(0,17);
             else
             {
              tm1637.display(0,hh/10);     // hour
              tm1637.display(1,hh%10);
                                          //now.second()%2 ? tm1637.display(1,17) : tm1637.display(1,hh%10);
              tm1637.display(2,mm/10);    // minutes
              tm1637.display(3,mm%10);
             }*/
              tm1637.display(0,a);     // hour
              tm1637.display(1,b);
                                          //now.second()%2 ? tm1637.display(1,17) : tm1637.display(1,hh%10);
              tm1637.display(2,c);    // minutes
              tm1637.display(3,d);
}
//-------------------------------------------------------------------------------------------------------------------------------
void READ_ROTERYencoder()
{
      // Получить статус Rotary Encoder
      rotaryEncoder.startDebouncer();    // должен быть внутри цикла, чтобы позволить Антидребезг
      //  rotaryEncoder.updateClearButton(); // включить кнопку, чтобы очистить недавнюю позицию датчика, если нажата, установлен на 0
      //  if(rotaryEncoder.hasButtonPushed()) Serial.println(F("Button ON"));

      if (rotaryEncoder.hasChanged()) // проверить, если положение датчика изменилось
          {
            volatile byte encoderPos = rotaryEncoder.encodedPos();      // счетчик обновления для набора (values 1..12)
            Serial.print(F("Position: "));
            Serial.println(encoderPos, DEC);
          }

}
//-------------------------------------------------------------------------------------------------------------------------------
void READ_TIME()
{
        DateTime now = rtc.now();
         hh = now.hour();
          mm = now.minute();
           if(hh != old_hour)  {tone(goarna, 3900, 90);  old_hour = hh;}
}
//-------------------------------------------------------------------------------------------------------------------------------
void LED()
{
  unsigned long now = millis();

  // продолжайте наблюдать за кнопкой:
  button.tick();

  // Вы можете реализовать другой код здесь или просто подождать некоторое время 

  if (nextAction == ACTION_OFF) {
    // ничего не делать.
    digitalWrite(13, LOW);

  } else if (nextAction == ACTION_ON) {
    // включить светодиод
    digitalWrite(13, HIGH);

  } else if (nextAction == ACTION_SLOW) {
    // медленно моргать
    if (now % 1000 < 500) {
      digitalWrite(13, LOW);
    } else {
      digitalWrite(13, HIGH);
    } // if

  } else if (nextAction == ACTION_FAST) {
    // быстро моргать
    if (now % 200 < 100) {
      digitalWrite(13, LOW);
    } else {
      digitalWrite(13, HIGH);
    } // if
  } // if
}
//-------------------------------------------------------------------------------------------------------------------------------
void blinking_dots(void)
{
  static byte i = 0;
  if(i < 100) tm1637.point(POINT_ON);
  else tm1637.point(POINT_OFF);
  i++;
  if (i > 200) i = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------

/*
// subroutine to return the length of the button push.
int getpushlength () {
  buttonstate = digitalRead(buton);  
       if(buttonstate == LOW && buttonflag==false) {     
              pushstart = millis();
              buttonflag = true;
          };
          
       if (buttonstate == HIGH && buttonflag==true) {
         pushstop = millis ();
         pushlength = pushstop - pushstart;
         buttonflag = false;
       };
       return pushlength;
}
//-------------------------------------------------------------------------------------------------------------------------------
int sethours () 
{
  if ((sethourstemp/10) == 0) tm1637.display(0,17);
  else
    tm1637.display(0,sethourstemp/10);     // hour
    tm1637.display(1,sethourstemp%10);
    tm1637.display(2,16);    // minutes
    tm1637.display(3,16);    // 
/*    
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return sethourstemp;
    }
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    
    sethourstemp=sethourstemp + knobval;
    if (sethourstemp < 0) {
      sethourstemp = 23;
    }
    if (sethourstemp > 23) {
      sethourstemp = 0;
    }
    sethours();
}
//-------------------------------------------------------------------------------------------------------------------------------
int setmins () {
    tm1637.display(0,16);     // hour
    tm1637.display(1,16);
    tm1637.display(2,setminstemp/10);    // minutes
    tm1637.display(3,setminstemp%10);    // 
/*    
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setminstemp;
    }
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
   
    setminstemp=setminstemp + knobval;
    if (setminstemp < 0) {
      setminstemp = 59;
    }
    if (setminstemp > 59) {
      setminstemp = 0;
    }
    setmins();
}
//-------------------------------------------------------------------------------------------------------------------------------
//sets the clock
void setclock (){
   sethours ();
   delay(500);
   setmins ();
   delay(500);
   rtc.adjust(DateTime(2017,4,1,sethourstemp,setminstemp,0));
   delay (500); 
   hh = sethourstemp;
   mm = setminstemp;
}
*/
//-------------------------------------------------------------------------------------------------------------------------------
// this function will be called when the button was pressed 1 time and them some time has passed.
void myClickFunction() 
{
  if (nextAction == ACTION_OFF) {nextAction = ACTION_ON; mode = MenuMinute;}
  else    nextAction = ACTION_OFF;
} // myClickFunction

//-------------------------------------------------------------------------------------------------------------------------------
// this function will be called when the button was pressed 2 times in a short timeframe.
void myDoubleClickFunction() 
{
  if (nextAction == ACTION_OFF) mode = MenuHour;
  if (nextAction == ACTION_ON) {nextAction = ACTION_SLOW; mode = Time;}
  else if (nextAction == ACTION_SLOW) nextAction = ACTION_FAST;
       else if (nextAction == ACTION_FAST) nextAction = ACTION_ON;
       
} // myDoubleClickFunction
