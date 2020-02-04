#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
DS3231 rtc(SDA, SCL);

int P1=5; // Button SET MENU'
int P2=4; // Button +
int P3=3; // Button -
int P4=2; // SWITCH Alarm (needs to be an interrupt pin, 2 or 3)

int P1released =1;
int P4Flag =0;
int alarmStopped =0;

#define LED 13
#define buzzer 10

int hourupg;
int minupg;
int yearupg;
int monthupg;
int dayupg;
int menu =0;
int setAlarm =0;

uint8_t alarmHours = 0, alarmMinutes = 0;

void setup() {
  // put your setup code here, to run once:
  // For UNO, SDA pin is A4, SCL pin is A5
  
  lcd.begin(20,4);//Defining 20 columns and 4 rows of lcd display
  lcd.backlight();//To Power ON the back light
  //lcd.backlight();// To Power OFF the back light
  rtc.begin();
  
  //Manually setting up the time, uncomment to use
  /*
  rtc.setDOW(THURSDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(8, 38, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(26, 12, 2018);   //dd/mm/yyyy
  */
  attachInterrupt(digitalPinToInterrupt(P4), P4ButtonPressed, FALLING); //initialize interrupt
  
  pinMode(P1,INPUT_PULLUP); // https://www.arduino.cc/en/Tutorial/InputPullupSerial
  pinMode(P2,INPUT_PULLUP);
  pinMode(P3,INPUT_PULLUP);
  pinMode(P4,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  pinMode(buzzer, OUTPUT); 

  menu = 0;
  
  //Serial monitor for debugging purposes
  //Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(P1)== LOW && P1released) 
  {
   menu=menu+1;
   P1released = 0;
  }
  if(digitalRead(P1)== HIGH)
  {
   P1released = 1;
  }
  if((digitalRead(P2)== LOW)&&(digitalRead(P3)== LOW))                                                                                                                                                                     
  {
    menu = 7;
  }
 if (menu==0)
    {
     DisplayDateTime();
     Alarm(); // Alarm control
    }
  if (menu==1)
    {
    DisplaySetHour();
    }
  if (menu==2)
    {
    DisplaySetMinute();
    }
  if (menu==3)
    {
    DisplaySetYear();
    }
  if (menu==4)
    {
    DisplaySetMonth();
    }
  if (menu==5)
    {
    DisplaySetDay();
    }
  if (menu==6)
    {
    StoreAgg(); 
    delay(500);
    menu=0;
    }
  //Set alarm
  if (menu==7)
    {
    DisplaySetHourAl();
    }
  if (menu==8)
    {
      DisplaySetMinuteAl();
    }
  if (menu==9)
  {
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("ALARM");
    lcd.setCursor(5,1);
    lcd.print(alarmHours, DEC);
    lcd.print(":");
    if (alarmMinutes <= 9)
    {
     lcd.print("0");
    }
    lcd.print(alarmMinutes, DEC);
    delay(1000);
    lcd.clear();
    menu=0;
  }
}

void Alarm() {
  if(P4Flag == 1)
  {
    P4Flag = 0;
    setAlarm=setAlarm+1; // 0: alarm not set. 1: alarm set
  }
  if (setAlarm==0)
  {
   printAlOff();
   digitalWrite(buzzer, HIGH); //when buzzer is logic low activated
   digitalWrite(LED,LOW);
   alarmStopped = 1;
   delay(300);
  }
  if (setAlarm==1)
  {
   printAlOn();    
   Time now = rtc.getTime();
   if ((now.hour == alarmHours && now.min == alarmMinutes) | !alarmStopped )
   {
    alarmStopped = 0;
    TurkishMarch();
   }
   else{
    digitalWrite(buzzer, HIGH); //when buzzer is logic low activated
    digitalWrite(LED,LOW);
    delay(300);
   }
  }
  if (setAlarm==2)
  {
   setAlarm=0;
  }
}

void printAlOn(){
  //For 4 row LCD
  lcd.setCursor(0,3);
  lcd.print("Alarm: ");
  if (alarmHours <= 9)
  {
    lcd.print("0");
  }
  lcd.print(alarmHours, DEC);
  lcd.print(":");
  if (alarmMinutes <= 9)
  {
   lcd.print("0");
  }
  lcd.print(alarmMinutes, DEC); 
}

void printAlOff() {
  //For 4 row LCD
  lcd.setCursor(0,3);
  
  lcd.print("Alarm: Off  ");  
}

void DisplaySetHourAl()// Setting the alarm hours
{
  lcd.clear();

  if(digitalRead(P2)==LOW)
  {
    if(alarmHours==23)
    {
      alarmHours=0;
    }
    else
    {
      alarmHours=alarmHours+1;
    }
  }
   if(digitalRead(P3)==LOW)
  {
    if(alarmHours==0)
    {
      alarmHours=23;
    }
    else
    {
      alarmHours=alarmHours-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set HOUR Alarm:");
  lcd.setCursor(0,1);
  lcd.print(alarmHours,DEC);
  delay(200);
}

void DisplaySetMinuteAl()// Setting the alarm minutes
 {
  lcd.clear();
  if(digitalRead(P2)==LOW)
  {
    if (alarmMinutes==59)
    {
      alarmMinutes=0;
    }
    else
    {
      alarmMinutes=alarmMinutes+1;
    }
  }
   if(digitalRead(P3)==LOW)
  {
    if (alarmMinutes==0)
    {
      alarmMinutes=59;
    }
    else
    {
      alarmMinutes=alarmMinutes-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set MIN. Alarm:");
  lcd.setCursor(0,1);
  lcd.print(alarmMinutes,DEC);
  delay(200);
}

void DisplaySetHour()
{
// time setting
  lcd.clear();
  if(digitalRead(P2)==LOW)
  {
    if(hourupg==23)
    {
      hourupg=0;
    }
    else
    {
      hourupg=hourupg+1;
    }
  }
   if(digitalRead(P3)==LOW)
  {
    if(hourupg==0)
    {
      hourupg=23;
    }
    else
    {
      hourupg=hourupg-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set hour:");
  lcd.setCursor(0,1);
  lcd.print(hourupg,DEC);
  delay(200);
}

void DisplaySetMinute()
{
// Setting the minutes
  lcd.clear();
  if(digitalRead(P2)==LOW)
  {
    if (minupg==59)
    {
      minupg=0;
    }
    else
    {
      minupg=minupg+1;
    }
  }
   if(digitalRead(P3)==LOW)
  {
    if (minupg==0)
    {
      minupg=59;
    }
    else
    {
      minupg=minupg-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Minutes:");
  lcd.setCursor(0,1);
  lcd.print(minupg,DEC);
  delay(200);
}
  
void DisplaySetYear()
{
// setting the year
  lcd.clear();
  if(digitalRead(P2)==LOW)
  {    
    yearupg=yearupg+1;
  }
   if(digitalRead(P3)==LOW)
  {
    yearupg=yearupg-1;
  }
  lcd.setCursor(0,0);
  lcd.print("Set Year:");
  lcd.setCursor(0,1);
  lcd.print(yearupg,DEC);
  delay(200);
}

void DisplaySetMonth()
{
// Setting the month
  lcd.clear();
  if(digitalRead(P2)==LOW)
  {
    if (monthupg==12)
    {
      monthupg=1;
    }
    else
    {
      monthupg=monthupg+1;
    }
  }
   if(digitalRead(P3)==LOW)
  {
    if (monthupg==1)
    {
      monthupg=12;
    }
    else
    {
      monthupg=monthupg-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Month:");
  lcd.setCursor(0,1);
  lcd.print(monthupg,DEC);
  delay(200);
}

void DisplaySetDay()
{
// Setting the day
  lcd.clear();
  if(digitalRead(P2)==LOW)
  {
    if (dayupg==31)
    {
      dayupg=1;
    }
    else
    {
      dayupg=dayupg+1;
    }
  }
   if(digitalRead(P3)==LOW)
  {
    if (dayupg==1)
    {
      dayupg=31;
    }
    else
    {
      dayupg=dayupg-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Day:");
  lcd.setCursor(0,1);
  lcd.print(dayupg,DEC);
  delay(200);
}

void StoreAgg()
{
// Variable saving
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SAVING IN");
  lcd.setCursor(0,1);
  lcd.print("PROGRESS");
  rtc.setDate(dayupg, monthupg, yearupg);
  rtc.setTime(hourupg, minupg, 0); //seconds set to 0
  rtc.setDOW();
  delay(200);
}

void DisplayDateTime()
{
  //Display first line
  lcd.setCursor(0, 0);
  lcd.print("Dope Alarm Clock");
  
  // We show the current date and time
  Time now = rtc.getTime();

  lcd.setCursor(0, 2);
  
  if (now.hour<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.hour, DEC);
  hourupg=now.hour;
  lcd.print(":");
  if (now.min<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.min, DEC);
  minupg=now.min;
  lcd.print(":");
  if (now.sec<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.sec, DEC);

  
  lcd.setCursor(0, 1);
  if (now.date<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.date, DEC);
  dayupg=now.date;
  lcd.print("/");
  if (now.mon<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.mon, DEC);
  monthupg=now.mon;
  lcd.print("/");
  lcd.print(now.year, DEC);
  lcd.print(" ");
  yearupg=now.year;

  lcd.print(rtc.getDOWStr(FORMAT_LONG)); // if it appears error in the code, enter the code given below
  //lcd.print(DOW[now.dayOfWeek()]);
}

//Customized alarm sound
void TurkishMarch() {
  lcd.noBacklight();
  digitalWrite(LED,HIGH);
  tone(buzzer,988);
  delay (125);
  tone(buzzer,880); 
  delay (125);
  tone(buzzer,831);
  delay (125);
  tone(buzzer,880);
  delay (125);
  lcd.backlight();
  tone(buzzer,1046);
  delay (250);
  noTone(buzzer);
  delay (250);
  lcd.noBacklight();
  tone(buzzer,1175);
  delay (125);
  tone(buzzer,1046); 
  delay (125);
  tone(buzzer,988);
  delay (125);
  tone(buzzer,1046);
  delay (125);
  lcd.backlight();
  tone(buzzer,1319);
  delay (250);
  noTone(buzzer);
  delay (250);
  lcd.noBacklight();
  tone(buzzer,1397);
  delay (125);
  tone(buzzer,1319); 
  delay (125);
  tone(buzzer,1245);
  delay (125);
  tone(buzzer,1319);
  delay (125);
  lcd.backlight();
  tone(buzzer,1976);
  delay (125);
  tone(buzzer,1760); 
  delay (125);
  tone(buzzer,1661);
  delay (125);
  tone(buzzer,1760);
  delay (125);
  lcd.noBacklight();
  tone(buzzer,1976);
  delay (125);
  tone(buzzer,1760); 
  delay (125);
  tone(buzzer,1661);
  delay (125);
  tone(buzzer,1760);
  delay (125);
  lcd.backlight();
  tone(buzzer,2093);
  delay (250);
  noTone(buzzer);
  delay (250);
  lcd.noBacklight();
  tone(buzzer,1760);
  delay (250);
  tone(buzzer,2093);
  delay (250);
  lcd.backlight();
  tone(buzzer,1568);
  delay (25);
  tone(buzzer,1760);
  delay (25);
  tone(buzzer,1976);
  delay (75);
  noTone(buzzer);
  delay (125);
  tone(buzzer,1760);
  delay (125);
  noTone(buzzer);
  delay (125);
  lcd.noBacklight();
  tone(buzzer,1568);
  delay (125);
  noTone(buzzer);
  delay (125);
  tone(buzzer,1760);
  delay (125);
  noTone(buzzer);
  delay (125);
  lcd.backlight();
  tone(buzzer,1976);
  delay (125);
  noTone(buzzer);
  delay (125);
  tone(buzzer,1760);
  delay (125);
  noTone(buzzer);
  delay (125);
  lcd.noBacklight();
  tone(buzzer,1568);
  delay (125);
  noTone(buzzer);
  delay (125);
  tone(buzzer,1760);
  delay (125);
  noTone(buzzer);
  delay (125);
  lcd.backlight();
  tone(buzzer,1976);
  delay (125);
  noTone(buzzer);
  delay (125);
  tone(buzzer,1760);
  delay (125);
  noTone(buzzer);
  delay (125);
  lcd.noBacklight();
  tone(buzzer,1568);
  delay (125);
  noTone(buzzer);
  delay (125);
  tone(buzzer,1480);
  delay (125);
  noTone(buzzer);
  delay (125);
  lcd.backlight();
  tone(buzzer,1319);
  delay (250);
  noTone(buzzer);
  delay (500);
}

//Interrupt Handler
void P4ButtonPressed() {
  P4Flag = 1;
}
