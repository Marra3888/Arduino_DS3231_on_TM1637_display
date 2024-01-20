/*
// base sketch from https://brainy-bits.com/tutorials/4-bits-7-segment-led-display-with-arduino/
// changed TM1637 library by niq_ro for degree, r & h letter
// DS3231 clock on TM1637 LED display by niq_ro from http://www.tehnic.go.ro
// & http://arduinotehniq.com/

#include "TM1637Display.h"
#include <Wire.h>
#include <RTClib.h>
 
//{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
//0~9,A,b,C,d,E,F,"-"," ",degree,r,h

#define CLK 5//Pins for TM1637       
#define DIO 4
TM1637Display tm1637(CLK,DIO);

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib

RTC_DS3231 rtc;
//int hh, mm; 
byte RTCC[7] = {};

void setup()
{
//  tm1637.init();
  tm1637.setBrightness(5); 
  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  rtc.begin();
// manual adjust
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
// automatic adjust
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}//end "setup()"
 
void loop(){
DateTime now = rtc.now();
  RTCC[6] = now.year();
  RTCC[5] = now.month();
  RTCC[4] = now.day();
//  RTCC[3] = DateTime.dayOfTheWeek(); //returns 0-6 where 0 = Sunday
  RTCC[2] = now.hour();  //depends on boolean summertime_EU
  RTCC[1] = now.minute();
  RTCC[0] = now.second();
//hh = now.hour(), DEC;
//mm = now.minute(), DEC;
/*
tm1637.point(POINT_OFF);
    tm1637.display(0,hh/10);     // hour
    tm1637.display(1,hh%10);
    tm1637.display(2,mm/10);    // minutes
    tm1637.display(3,mm%10);    // 
delay(500);
tm1637.point(POINT_ON);
    tm1637.display(0,hh/10);     // hour
    tm1637.display(1,hh%10);
    tm1637.display(2,mm/10);    // minutes
    tm1637.display(3,mm%10);    // 
delay(500);

//  for(byte i = 0; i < 2; i++)
//  {
  byte dots = RTCC[0]%2 ? 0x40 : 0;
  tm1637.showNumberDecEx(RTCC[2], dots, true, 2, 0);
  tm1637.showNumberDecEx(RTCC[1], dots, true, 2, 2);
//  }
}// end loop() 
*/

// base sketch from https://brainy-bits.com/tutorials/4-bits-7-segment-led-display-with-arduino/
// changed TM1637 library by niq_ro for degree, r & h letter
// DS3231 clock on TM1637 LED display by niq_ro from http://www.tehnic.go.ro
// & http://arduinotehniq.com/


#include "RotaryEncoder.h" // from http://www.pjrc.com/teensy/td_libs_Encoder.html
//RotaryEncoder knob(4, 5); //encoder connected to pins 2 and 3 (and ground)
RotaryEncoder rotaryEncoder = RotaryEncoder();  // CLK = pin 2, DAT = pin 3, SW = pin A1, CW = count up, CCW = count down


#include <TM1637.h>
#include <TM1637_A.h>
#include <OneButton.h>
// The actions I ca do...
typedef enum 
{
  ACTION_OFF,  // set LED "OFF".
  ACTION_ON,   // set LED "ON"
  ACTION_SLOW, // blink LED "SLOW"
  ACTION_FAST,  // blink LED "FAST"
  ACTION_SUPER_FAST
} MyActions;

// Setup a new OneButton on pin A1.  
OneButton button(A1, true);
MyActions nextAction = ACTION_OFF; // no action when starting

enum Regim
{
  Time,
  MenuHour,
  MenuMinute,
  Light,
  Letter,
  Sound
};


#define CLK (byte)5//Pins for TM1637       
#define DIO (byte)4

TM1637 tm1637(CLK,DIO);
TM1637_A disp(CLK,DIO);

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
//#include <RTClib.h>
#include <DS3231.h>

DS3231 rtc;
RTClib RTC;

byte hh, mm, old_hour, mode = Time, brightness = 4, menu = 0;
byte encoderPos_A; 
bool LongPress = false, Buzzer = false;

#define goarna (byte)7 // buzzer
//-------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  tm1637.init();
  tm1637.set(brightness); 
  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  Wire.begin();
// manual adjust
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
// automatic adjust
//  rtc.adjust(DateTime((__DATE__), (__TIME__)));
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

//   pinMode(buton,INPUT);//push button on encoder connected 
//  digitalWrite(buton,HIGH); //Pull at 1 high
//  rotaryEncoder.setRange(0,23);
//  Serial.begin(9600);
//  Serial.println(F("E02 KY-040 RotaryEncoder Demo"));

  
   pinMode(goarna, OUTPUT);   
   digitalWrite(goarna, LOW); //set in 0 logical level
//   tone(goarna, 4000, 80); 


  // enable the standard led on pin 13.
  pinMode(13, OUTPUT);      // sets the digital pin as output

  // связать функцию myClickFunction, которая будет вызываться при событии щелчка.   
  button.attachClick(myClickFunction);

  // свяжите функцию двойного клика для вызова на событии двойного клика.   
  button.attachDoubleClick(myDoubleClickFunction);

  button.attachDuringLongPress(myDuringLongPress);

  // установить 80 мсек. время подведения итогов По умолчанию 50 мсек.
  button.setDebounceTicks(80);

  READ_TIME();

  disp.clearDisplay();
    
    for (byte j = 0; j < 4; j++) 
    {
        disp.displayByte(j, 0x01);
        delay(100);
        disp.displayByte(j, 0x02);
        delay(100);
        disp.displayByte(j, 0x40);
        delay(100);
        disp.displayByte(j, 0x10);
        delay(100);
        disp.displayByte(j, 0x08);
        delay(100);
        disp.displayByte(j, 0x04);
        delay(100);
        disp.displayByte(j, 0x40);
        delay(100);
        disp.displayByte(j, 0x20);
        delay(100);

        if(j == 0) disp.display(j, hh/10);
        if(j == 1) disp.display(j, hh%10);
        if(j == 2) disp.display(j, mm/10);
        if(j == 3) disp.display(j, mm%10);
    }
}//end "setup()"
//------------------------------------------------------------------------------------------------------------------------------- 
void loop()
{
      switch(mode)
     {
            case Time: 
                        READ_TIME();
                        if ((hh/10) == 0)   To_Display(19, hh%10, mm/10, mm%10);
                        else  To_Display(hh/10, hh%10, mm/10, mm%10);
                        blinking_dots();
            break;
            case MenuHour:
                        
//                        READ_TIME();
                        READ_ROTERYencoder();
                        rtc.setHour(encoderPos_A);
                        tm1637.point(POINT_OFF);
                        To_Display(encoderPos_A/10, encoderPos_A%10, 18, 18);
      
            break;
            case MenuMinute:
                        
//                        READ_TIME();
                        READ_ROTERYencoder();
                        rtc.setMinute(encoderPos_A);
                        tm1637.point(POINT_OFF);
                        To_Display(18, 18, encoderPos_A/10, encoderPos_A%10);
      
            break;
            case Light:
                        READ_ROTERYencoder();
                        brightness = encoderPos_A;
                        tm1637.set(brightness);
                        tm1637.point(POINT_OFF);
                        To_Display(17, 18, 19, encoderPos_A);
            break;
            case Letter:
                        READ_ROTERYencoder();
                        tm1637.point(POINT_OFF);
                        To_Display(17, 28, 19, encoderPos_A);
            break;
            case Sound:
                        READ_ROTERYencoder();
                        tm1637.point(POINT_OFF);
                        Buzzer = encoderPos_A;
                        if(Buzzer)  To_Display(11, 25, 29, 23);
                        if(!Buzzer)  To_Display(11, 25, 29, 15);
            break;
            case 6:
                    
            break;
            default:  
                    mode = 0;
    }
      LED();
}// end loop() 
//-------------------------------------------------------------------------------------------------------------------------------
void To_Display(byte a, byte b, byte c, byte d)
{
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
            encoderPos_A = rotaryEncoder.encodedPos();      // счетчик обновления для набора (values 1..12)
          }
}
//-------------------------------------------------------------------------------------------------------------------------------
void READ_TIME()
{
        DateTime now = RTC.now();
         hh = now.hour();
          mm = now.minute();
           if(hh != old_hour)  
            if(!LongPress)  {if(Buzzer){tone(goarna, 3900, 90);  old_hour = hh;}}
}
//-------------------------------------------------------------------------------------------------------------------------------
void LED()
{
  unsigned long now = millis();

  // продолжайте наблюдать за кнопкой:
  button.tick();

  // Вы можете реализовать другой код здесь или просто подождать некоторое время 

  if (nextAction == ACTION_OFF) 
  {
    // ничего не делать.
    digitalWrite(13, LOW);
  } 
  else if (nextAction == ACTION_ON) 
    // включить светодиод
    digitalWrite(13, HIGH);

  else if (nextAction == ACTION_SLOW) 
  {
    // медленно моргать
    if (now % 1000 < 500)  digitalWrite(13, LOW);
    else digitalWrite(13, HIGH);
  } 
  else if (nextAction == ACTION_FAST) 
  {
    // быстро моргать
    if (now % 200 < 100) digitalWrite(13, LOW);
    else  digitalWrite(13, HIGH);
  } 
  else if (nextAction == ACTION_SUPER_FAST)
  {
    // быстро моргать
    if (now % 100 < 50)  digitalWrite(13, LOW);
    else  digitalWrite(13, HIGH);
  }
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

//-------------------------------------------------------------------------------------------------------------------------------
// this function will be called when the button was pressed 1 time and them some time has passed.
void myClickFunction() 
{
  if(LongPress) 
    {
        menu++;
        if(menu > 4)  
        {
          menu = 0;
          mode = MenuHour;
          rotaryEncoder.setRange(0,23);
        }
        
        if(menu == 1)
        {
            tm1637.clearDisplay();
            mode = MenuMinute;
            rotaryEncoder.setRange(0,59);
      
        }
      
        if(menu == 2)
        {
            tm1637.clearDisplay();
            mode = Light;
            rotaryEncoder.setRange(0,7);
      
        }
      
        if(menu == 3)
        {
            tm1637.clearDisplay();
            mode = Letter;
            rotaryEncoder.setRange(0,29);
        }

        if(menu == 4)
        {
            tm1637.clearDisplay();
            mode = Sound;
            rotaryEncoder.setRange(0,1);
        }
    }

    
  if (nextAction == ACTION_OFF) nextAction = ACTION_ON;//{nextAction = ACTION_ON; mode = MenuMinute; rotaryEncoder.setRange(0,59);}
  else    nextAction = ACTION_OFF;
} // myClickFunction

//-------------------------------------------------------------------------------------------------------------------------------
// this function will be called when the button was pressed 2 times in a short timeframe.
void myDoubleClickFunction() 
{
  if(LongPress) 
  {
    tm1637.clearDisplay();
    tone(goarna, 3900, 90);
    mode = Time;
    LongPress = false;
  }
  
//  if (nextAction == ACTION_OFF) {mode = MenuHour; rotaryEncoder.setRange(0,23);}
  if (nextAction == ACTION_ON) nextAction = ACTION_SLOW;//{nextAction = ACTION_SLOW; mode = Time;}
    else if (nextAction == ACTION_SLOW) nextAction = ACTION_FAST;
       else if (nextAction == ACTION_FAST) nextAction = ACTION_SUPER_FAST;
          else if (nextAction == ACTION_SUPER_FAST) nextAction = ACTION_ON;
       
} // myDoubleClickFunction
//-------------------------------------------------------------------------------------------------------------------------------
void myDuringLongPress()
{
//  tm1637.clearDisplay();
  
  if(button.getPressedTicks() > 20)  
  {
    if(!LongPress)  
    {
        tone(goarna, 3900, 90);
        mode = MenuHour;
        rotaryEncoder.setRange(0,23);
        LongPress = true;
        menu = 0;
    }
  }

}
