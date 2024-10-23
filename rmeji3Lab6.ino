/*
// Rafael Mejia, 679124181 rmeji3
// TA - 
// Lab 6
// Description -
create a clock. the user can select a date and time for the clock to start but it must be valid. take in the users input using Serial.read()
// References - https://forum.arduino.cc/t/sprintf-conversion-specifications-n/954134/5
https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay/
https://github.com/PaulStoffregen/Time
https://www.pjrc.com/teensy/td_libs_Time.html
https://www.arduino.cc/reference/tr/language/functions/communication/serial/readbytesuntil/
https://docs.arduino.cc/learn/electronics/lcd-displays/
https://cplusplus.com/reference/cstdlib/atoi/
*/

#include <LiquidCrystal.h>
#include <TimeLib.h>

unsigned long previousMillis = 0; 

const long interval = 1000;  // interval

int hoursInt = -1;
int minsInt = -1;
int secsInt = -1;
int dayInt = -1;
int monthInt = -1;
int yearInt = -1;
bool isValid = true;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup() {
  setTime(0,0,0,1,1,0); //default/start time 
  lcd.begin(16, 2);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}
//split the incoming buffer to date and time string
void splitBuffer(char *buffer, char *date, char *time) {
    int i = 1;
  	int x = 0;
 
    while (buffer[i] != '@' && buffer[i] != '\0') {
  
      date[x] = buffer[i];
      i++;
      x++;
    }
    date[x] = '\0'; 


    if (buffer[i] == '@') {
        i+=2;
    }

    int j = 0;
    while (buffer[i] != ')' && j < 16) {
        time[j] = buffer[i];
        i++;
        j++;
    }
    time[j] = '\0';
}
//split the times into individual strings
void splitTime(char *timeBuf, char* hours, char* mins, char* secs){
  int i = 0;
  int x = 0;
  while(timeBuf[i]!= ':'){
   hours[x] = timeBuf[i];
   i++;
   x++;
  }
  
 
  hours[x] = '\0';
  x = 0;
  i++;
  
  while(timeBuf[i]!= ':'){
   mins[x] = timeBuf[i];
   i++;
   x++;
  }
  
  mins[x] = '\0';
  x = 0;
  i++;
  
  while(timeBuf[i]!= '\0'){
   secs[x] = timeBuf[i];
   i++;
   x++;
  }
  secs[x] = '\0';
  x = 0;
  
}
//split the dates into individual strings
void splitDate(char *dateBuf, char* month, char* day, char* year){
  int i = 0;
  int x = 0;
  while(dateBuf[i]!= '/'){
   month[x] = dateBuf[i];
   i++;
   x++;
  }
  
 
  month[x] = '\0';
  x = 0;
  i++;
  
  while(dateBuf[i]!= '/'){
   day[x] = dateBuf[i];
   i++;
   x++;
  }
  
  day[x] = '\0';
  x = 0;
  i++;
  
  while(dateBuf[i]!= '\0'){
   year[x] = dateBuf[i];
   i++;
   x++;
  }
  year[x] = '\0';
  x = 0;
  
}
//check for leap year
bool checkLeapYear(int year){
  return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}
//make sure the date entered is valid
bool validDate(int month, int day, int year){
  bool validDay = false;
  bool validMonth = false;
  bool validYear = false;
  validYear = year >= 0;

  validMonth = month >= 1 && month <= 12;

  if(day <= 0){
    validDay = false;
  }else{
    switch(month){
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      validDay = day <= 31;
      break;
    case 2:
      validDay = (checkLeapYear(year) && day <= 29) || (!checkLeapYear(year)  && day <= 28);
      break;
    default:
      validDay = day <= 30;
      break;
    }
  }

  

  return validDay && validMonth && validYear;


}
//make sure the time entered is valid
bool validTime(int hour, int min, int sec){
  bool validHour = false;
  bool validMin = false;
  bool validSec = false;

  validHour = hour <= 23 && hour >= 0;
  validMin = min <= 59 && min >= 0;
  validSec = sec <= 59 && sec >= 0;

  return validHour && validMin && validSec;

}

void loop() {
  // only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
 	char buffer[37]; // for incoming serial data
    Serial.readBytesUntil('\0', buffer, 37);
    buffer[36] = '\0';
    char date[17];
    char time[17];
 
    splitBuffer(buffer, date, time);
    
    
    char hours[17];
    char mins[17];
    char secs[17];
    
    splitTime(time, hours, mins, secs);
    
    char month[17];
    char day[17];
    char year[17];
    
 
    
    splitDate(date, month, day, year);

  //convert string to int
    monthInt = atoi(month);
    dayInt = atoi(day);
    yearInt = atoi(year);
   
    hoursInt = atoi(hours);
    minsInt = atoi(mins);
    secsInt = atoi(secs);
    
    
    //check id date and time are valid
    if(validDate(monthInt, dayInt, yearInt) && validTime(hoursInt, minsInt, secsInt))
    {
      setTime(hoursInt,minsInt,secsInt,dayInt,monthInt,yearInt);
    }else{
      Serial.println("invalid time/date!");
    }

    
  }
  
  //wait every 1 seconds to update lcd
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    char timeBuff[17];
    timeBuff[16] = '\0';
    char dateBuff[17];
    dateBuff[16] = '\0';

    sprintf(timeBuff,"%02d:%02d:%02d", hour(), minute(), second());
     
    sprintf(dateBuff,"%02d/%02d/%d", month(), day(), year());
    Serial.println(dateBuff);
    Serial.println(timeBuff);
    Serial.println("\n");
  
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print(dateBuff);
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print(timeBuff);
  }

  /*
  Inputs that you should be testing:
    ● valid dates and times (12/04/03 @ 12:06:22)
    ● invalid dates and times (20/20/-3 @ -1:200:333)
    ● times right before the end of a day/month (10/31/24 @ 23:59:45) to show change of day/month
    ● times right before the end of a year (12/31/24 @ 23:59:45) to show change of day/year
    ● times right before leap day (2/28/24 @ 23:59:45) to show inclusion of leap day

  */
}