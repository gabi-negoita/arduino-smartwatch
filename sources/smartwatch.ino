#include <LiquidCrystal.h> 
#include "DHT.h"
#include <TimeLib.h>
#include <Time.h>

#define DHTTYPE DHT11
#define DHTPIN 8
#define BUTTON_PIN 10
#define YELLOW_LED_PIN 7
#define RED_LED_PIN 13
#define TIME_HEADER "T"
#define TIME_REQUEST 7
#define LCD_CONTRAST 100

String tempStr, humStr;
int buttonState, screen = 1;
float humid, temp;
String compile_date, compile_time;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();

  lcd.clear();
  pinMode(BUTTON_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  analogWrite(6, LCD_CONTRAST);

  compile_date = __DATE__;
  String monStr = compile_date.substring(0,3);
  String dayStr = compile_date.substring(4,6);
  String yearStr = compile_date.substring(9,11);

  compile_time = __TIME__;
  String hrStr = compile_time.substring(0,2);
  String minStr = compile_time.substring(3,5);
  String secStr = compile_time.substring(6,8);

  setTime(hrStr.toInt(), minStr.toInt(), secStr.toInt(),stringToNumberDay(dayStr),stringToNumberMonth(monStr),yearStr.toInt());
}

void loop()
{ 
  delay(250);
  checkLowTemperature(22);
  checkHighTemperature(28);
  
  buttonState = digitalRead(BUTTON_PIN);
  
  if(buttonState == HIGH) {
      lcd.clear();
      screen = screen + 1;
      Serial.println("Button state changed. Displaying screen: ");
      screen = screen == 4 ? 1 : screen;
      Serial.println(screen);
  }

  switch (screen){
    case 1: {
      displayDateAndTime();
      break;
    }

    case 2: {
      displayTemperatureAndHumidity();
      break;
    }

    case 3: {
      displayThermalComfortIndex();
      break;
    }
  }
}

void displayDateAndTime() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (hour() < 10) {
    lcd.print("Buna dimineata!");
  }
  else if (hour() >= 10 && hour() < 18) {
    lcd.print("Buna ziua!");  
  }
  else {
    lcd.print("Buna seara!");  
  }
  
  lcd.setCursor(0, 1);
  // Lu, 17 Oct 8:02
  String dateToDisplay = intToStringWeekday(weekday()) + ", " + day() + " " + intToStringMonth(month());
  String timeToDisplay = String(hour()) + ":" + ((minute() < 10) ? "0" + String(minute()) : String(minute()));
  lcd.print(dateToDisplay);
  lcd.setCursor(16 - timeToDisplay.length(), 1);
  lcd.print(timeToDisplay);
}

void displayTemperatureAndHumidity() {
  tempStr="Temp:    ";
  humStr="Humid:   ";
  // First line of lcd
  
  // Reading humidity
  humid = dht.readHumidity();
  // Reading temperature (in Celsius)
  temp = dht.readTemperature();  

  if (isnan(humid) || isnan(temp)) {
    lcd.print("ERR");
    return;
  }

  lcd.setCursor(0, 0);
  tempStr=tempStr + temp + " C";
  lcd.print(tempStr);
  displayDegreeSign(14,0);
  lcd.setCursor(0, 1);
  humStr=humStr + humid + " %";
  lcd.print(humStr);
}

void displayThermalComfortIndex() {
  lcd.setCursor(0, 0);
  lcd.print(" Confort Termic ");
  lcd.setCursor(0, 1);
  float ict = (temp * 1.8 + 32) - (0.55 - 0.0055 * humid) * ((temp * 1.8 + 32) - 58);
  lcd.print("****** ");
  lcd.print(int(ict));
  lcd.print(" ******");
  Serial.println("Thermal comfort index: " + String(ict));
}

void checkLowTemperature(float minTemp) {
  temp = dht.readTemperature();
  if (temp < minTemp) {
    Serial.println("The temperature is " + String(temp) + " C, which is below " + String(minTemp) + " C");
    digitalWrite(YELLOW_LED_PIN, HIGH);
    delay(250);
    digitalWrite(YELLOW_LED_PIN, LOW);
  }
}

void checkHighTemperature(float maxTemp) {
  float temp = dht.readTemperature();
  if (temp > maxTemp) {
    Serial.println("The temperature is " + String(temp) + " C, which is above " + String(maxTemp) + " C");
    digitalWrite(RED_LED_PIN, HIGH);
    delay(250);
    digitalWrite(RED_LED_PIN, LOW);
  }
}

void displayDegreeSign(int row, int col){
  byte customChar[8] = {
    0b00011,
    0b00011,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  
  lcd.createChar(0, customChar);
  lcd.setCursor(row, col);
  lcd.write(byte(0));
} 

String intToStringWeekday(int day){
  if(day == 1){
      return "Du";
    }
  if(day == 2){
      return "Lu";
    }
  if(day == 3){
      return "Ma";
    }
  if(day == 4){
      return "Mi";
    }
  if(day == 5){
      return "Jo";
    }
  if(day == 6){
      return "Vi";
    }
  if(day == 7){
      return "Sa";
    }
}

int stringToNumberDay(String day) {
  if(day[0]==' '){
    return day.substring(1,2).toInt();
  }
  return day.toInt();
}

int stringToNumberMonth(String month) {
  if(month == "Jan"){
      return 1;
    }
  if(month == "Feb"){
      return 2;
    }
  if(month == "Mar"){
      return 3;
    }
  if(month == "Apr"){
      return 4;
    }
  if(month == "May"){
      return 5;
    }
  if(month == "Jun"){
      return 6;
    }
  if(month == "Jul"){
      return 7;
    }
  if(month == "Aug"){
      return 8;
    }
  if(month == "Sep"){
      return 9;
    }
  if(month == "Oct"){
      return 10;
    }
  if(month == "Nov"){
      return 11;
    }
  if(month == "Dec"){
      return 12;
    }
}

String intToStringMonth(int month){
  if(month == 1){
      return "Ian";
    }
  if(month == 2){
      return "Feb";
    }
  if(month == 3){
      return "Mar";
    }
  if(month == 4){
      return "Apr";
    }
  if(month == 5){
      return "Mai";
    }
  if(month == 6){
      return "Iun";
    }
  if(month == 7){
      return "Iul";
    }
  if(month == 8){
      return "Aug";
    }
  if(month == 9){
      return "Sep";
    }
  if(month == 10){
      return "Oct";
    }
  if(month == 11){
      return "Nov";
    }
  if(month == 12){
      return "Dec";
    }
}
