#include <LiquidCrystalRus.h>
#include <TroykaCurrent.h>

LiquidCrystalRus lcd(13, 12, 11, 10, 9, 8);//8, 9, 10, 11, 12, 13); 

ACS712 producerUpperCurrent(A1);
ACS712 producerLowerCurrent(A0);
ACS712 userCurrent(A3);

int menuButton = 4;      //кнопка переключения страниц 
int producerButton1 = 46;//тумблер 1
int producerButton2 = 48;//тумблер 2
int batteryButton = 49;  //тумблер 3

int userMosfet1 = 44;    //
int userMosfet2 = 5;     
int userMosfet3 = 6;
int userMosfet4 = 47;
int producerUpperMosfet = 2;
int producerLowerMosfet = 3;
int batteryMosfet = 7;

int flag = 0; // проверка дребезга 
int reg = 1;  // текущая страница   
float producerSum; // cумма токов с источников тока (неожиданно)

float k = 2.8;
float voltage;
float A_K = 0.6;
static float A_y;

// время до обновления монитора
long int delayMenu1; 
long int delayMenu2;
long int delayMenu3;
long int delayMenu4;
//long int delayMenu5;
long int delayUsers;
long int delaySerial;
long int delayAnalog1;
long int delayAnalog2;

//нужное кол-во тока
float current5Working;
float current4Working = 5;
float current3Working = 5;
float current2Working = 5;
float current1Working = 0.01;

void page_switch(int reg){
  
  switch(reg)
  {
    case 1:
      if(millis() - delayMenu1 > 1000){
            lcd.clear();
            lcd.print("ЭС1: ");
            if(producerUpperCurrent.readCurrentDC() > 0.5 && digitalRead(producerUpperMosfet) == LOW){
              lcd.print("ON, ");
              lcd.print(producerUpperCurrent.readCurrentDC());
              lcd.print(" Вт");
            }
          else{
            lcd.print("OFF");
          }
              lcd.setCursor(0,1);
              lcd.print("ЭС2: ");
            if(producerUpperCurrent.readCurrentDC() > 0.5 && digitalRead(producerLowerMosfet) == LOW){
              lcd.print("ON, ");
              lcd.print(producerLowerCurrent.readCurrentDC());
              lcd.print(" Вт");
            }
       else{
       lcd.print("OFF");
       }
        delayMenu1 = millis();
     }
      break;
    case 2:
      if(millis() - delayMenu2 > 1000){
        lcd.clear();
        lcd.print("АКБ: ");
        if(A_y > 6 && digitalRead(batteryMosfet) == HIGH){
          lcd.print("ON, ");
          lcd.print(A_y);
          lcd.print(" В");
        }
        else{
          lcd.print("OFF");
        }
        delayMenu2 = millis();
      }
      break;
    case 3:
      if(millis() - delayMenu4 > 1000){
        lcd.clear();
        lcd.print("П1:");
        digitalRead(userMosfet1) == HIGH ? lcd.print("ON ") : lcd.print("OFF ");
        lcd.print("П2:");
        digitalRead(userMosfet2) == HIGH ? lcd.print("ON ") : lcd.print("OFF ");
        lcd.setCursor(0, 1);
        lcd.print("П3:");
        digitalRead(userMosfet3) == HIGH ? lcd.print("ON ") : lcd.print("OFF ");
        lcd.print("П4:");
        digitalRead(userMosfet4) == HIGH ? lcd.print("ON ") : lcd.print("OFF ");
        delayMenu4 = millis();
      }
      break;
  }
}

void setup() {
    // put your setup code here, to run once:
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  
  pinMode(menuButton, INPUT);
  pinMode(producerButton1, INPUT);
  pinMode(producerButton2, INPUT);
  pinMode(batteryButton, INPUT);
  
  pinMode(producerUpperMosfet, OUTPUT);
  pinMode(producerLowerMosfet, OUTPUT);
  pinMode(batteryMosfet, OUTPUT);
  pinMode(userMosfet1, OUTPUT);
  pinMode(userMosfet2, OUTPUT);
  pinMode(userMosfet3, OUTPUT);
  pinMode(userMosfet4, OUTPUT);
  
  digitalWrite(producerUpperMosfet, LOW);
  digitalWrite(producerLowerMosfet, LOW);
  digitalWrite(batteryMosfet, HIGH);
  digitalWrite(userMosfet1, LOW);
  digitalWrite(userMosfet2, LOW);
  digitalWrite(userMosfet3, LOW);
  digitalWrite(userMosfet4, LOW);
}

void loop() {
    
  digitalWrite(producerUpperMosfet, !digitalRead(producerButton1));
  digitalWrite(producerLowerMosfet, !digitalRead(producerButton2));
  digitalWrite(batteryMosfet, digitalRead(batteryButton));
  
  //измерение вольтажа
  voltage = k*4.5f/1024*analogRead(A2);
  A_y = A_y - A_K * (A_y - voltage);
  
  if(digitalRead(menuButton) == HIGH && flag == 0)
  {
    reg++;
    flag = 1;
    if(reg > 3) reg = 1;
  }
  if(digitalRead(menuButton) == LOW && flag == 1)
  {
    flag = 0;
  }
  if(millis() - delayUsers > 2000)
  {
    digitalWrite(userMosfet1, HIGH);
    digitalWrite(userMosfet2, HIGH);
    digitalWrite(userMosfet3, HIGH);
    digitalWrite(userMosfet4, HIGH);
    delayUsers = millis();
  }
  producerSum = producerUpperCurrent.readCurrentDC() + producerLowerCurrent.readCurrentDC();
  if(producerSum > current4Working)
  {
    digitalWrite(userMosfet1, HIGH);
    digitalWrite(userMosfet2, HIGH);
    digitalWrite(userMosfet3, HIGH);
    digitalWrite(userMosfet4, HIGH);
  }
  else if(producerSum < current4Working && producerSum > current3Working)
  {
    digitalWrite(userMosfet1, HIGH);
    digitalWrite(userMosfet2, HIGH);
    digitalWrite(userMosfet3, HIGH);
    digitalWrite(userMosfet4, LOW);
  }
  else if(producerSum < current3Working && producerSum > current2Working)
  {
    digitalWrite(userMosfet1, HIGH);
    digitalWrite(userMosfet2, HIGH);
    digitalWrite(userMosfet3, LOW);
    digitalWrite(userMosfet4, LOW);
  }
  else if(producerSum < current2Working && producerSum > current1Working)
  {
    digitalWrite(userMosfet1, HIGH);
    digitalWrite(userMosfet2, LOW);
    digitalWrite(userMosfet3, LOW);
    digitalWrite(userMosfet4, LOW);
  }
  else
  {
    digitalWrite(userMosfet1, LOW);
    digitalWrite(userMosfet2, LOW);
    digitalWrite(userMosfet3, LOW);
    digitalWrite(userMosfet4, LOW);
  }
  page_switch(reg);
}
