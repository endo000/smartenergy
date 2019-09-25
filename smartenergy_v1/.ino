#include <LiquidCrystal.h>
#include <TroykaCurrent.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);//8, 9, 10, 11, 12, 13); 
class User{

private:
    int NEEDING_VOLTAGE;
    int priority;
    int pin;
public:
    User(int NEEDING_VOLTAGE,int priority, int pin);
    int getND();
    int getPR();
    int getPN();
    void on();
    void off();
    
};
    User::User(int _NEEDING_VOLTAGE,int _priority, int _pin){
      NEEDING_VOLTAGE = _NEEDING_VOLTAGE;
      priority = _priority;
      pin = _pin;
    }
    int User::getND(){
      return NEEDING_VOLTAGE;
    }
    int User::getPR(){
      return priority;
    }
    int User::getPN(){
      return pin;
    }
    void User::on(){
      digitalWrite(pin,HIGH);
    }
    void User::off(){
      digitalWrite(pin,LOW);
    }

//сила тока
ACS712 producerUpperCurrent(A1);
ACS712 producerLowerCurrent(A0);
int users_volts[] = {1,6,7,8,9};

float delayDebug = 0;

int menuButton = 4;      //кнопка переключения страниц 

/*int userMosfet1 = 44;    //
int userMosfet2 = 5;     
int userMosfet3 = 6;
int userMosfet4 = 47;*/
int producerUpperMosfet = 2;
int producerLowerMosfet = 3;
int batteryMosfet = 7;

User* user1 = new User(0.01,1,2);
User* user2 = new User(5,2,3);
User* user3 = new User(5,3,4);
User* user4 = new User(5,4,5);
User* user5 = new User(5,5,6);

int flag = 0; // проверка дребезга 
int reg = 1;  // текущая страница   
float producerSum; // cумма токов с источников тока (неожиданно)

float k = 5;
float voltageA0;
float voltageA1;
//float A_K = 0.6;
//static float A_y;

// время до обновления монитора
long int delayMenu1; 
long int delayMenu2;
long int delayMenu3;
long int delayMenu4;
long int delayMenu5;

long int delayUsers;
long int delaySerial;
long int delayAnalog1;
long int delayAnalog2;

//нужное кол-во тока
/*float current5Working;
float current4Working = 5;
float current3Working = 5;
float current2Working = 5;
float current1Working = 0.01;*/

float voltage(int pin){
  float volt = analogRead(pin);
  volt = (volt * k)/1023;
  return volt;
}

void pageSwitch(int reg){
  
  switch(reg)
  {
    case 1:
      if(millis() - delayMenu1 > 1000){
            lcd.clear();
            lcd.print("EC1: ");
            lcd.print(voltage(A0)*5);
            if(producerUpperCurrent.readCurrentDC() > 0.5 && digitalRead(producerUpperMosfet) == LOW){
              lcd.print("ON, ");
              lcd.print(producerUpperCurrent.readCurrentDC());
              lcd.print(" Vt");
            }
          else{
            lcd.print("OFF");
          }
              lcd.setCursor(0,1);
              lcd.print("EC2: ");
              lcd.print(voltage(A1) * 5);
            if(producerUpperCurrent.readCurrentDC() > 0.5 && digitalRead(producerLowerMosfet) == LOW){
              lcd.print("ON, ");
              lcd.print(producerLowerCurrent.readCurrentDC());
              lcd.print(" Vt");
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
        lcd.print("AKB: ");
        /*if(A_y > 6 && digitalRead(batteryMosfet) == HIGH){
          lcd.print("ON, ");
          lcd.print(A_y);
          lcd.print(" В");
        }
        else{
          lcd.print("OFF");
        }*/
        delayMenu2 = millis();
      }
      break;
    case 3:
      if(millis() - delayMenu4 > 1000){
        lcd.clear();
        lcd.print("P2:");
        digitalRead(user2->getPN()) == HIGH ? lcd.print("ON ") : lcd.print("OFF ");
        lcd.print("P3:");
        digitalRead(user3->getPN()) == HIGH ? lcd.print("ON ") : lcd.print("OFF ");
        lcd.setCursor(0, 1);
        lcd.print("P4:");
        digitalRead(user4->getPN()) == HIGH ? lcd.print("ON ") : lcd.print("OFF ");
        lcd.print("P5:");
        digitalRead(user5->getPN()) == HIGH ? lcd.print("ON ") : lcd.print("OFF ");
        delayMenu4 = millis();
      }
      break;
    case 4:
      if(millis() - delayMenu5 > 1000){
        lcd.clear();
        lcd.print("B:");
        //lcd.print(voltage);
        delayMenu5 = millis();
      }
      break;
  }
}

void checkUsers(){
  float voltageSum;
  if(user2->getND() < users_volts[1] && user2->getND() < voltage(A0)+voltage(A1)){
    user2->on();
    Serial.print("2 ON!");
    if(user3->getND() < users_volts[2] && user3->getND() < voltage(A0)+voltage(A1)){
     user3->on();
     Serial.print("3 ON!");
     if(user4->getND() < users_volts[3] && user4->getND() < voltage(A0)+voltage(A1)){
      user4->on();
      Serial.print("4 ON!");
      if(user5->getND() < users_volts[4] && user5->getND() < voltage(A0)+voltage(A1)){
       user5->on();
       Serial.print("5 ON!");
      }else{
        Serial.print("5 OFF!");
        user5->off();  
      }
     }else{
       Serial.print("4 OFF!");
       user4->off(); 
     }
    }else{
      Serial.print("3 OFF!");
      user3->off();  
    }
   }else{
    //Serial.println("2 OFF!");
    user2->off(); 
   }
   Serial.println();
}

void checkPress(){
  if(digitalRead(menuButton) == HIGH && flag == 0)
  {
    reg++;
    flag = 1;
    if(reg > 4) reg = 1;
  }
  if(digitalRead(menuButton) == LOW && flag == 1)
  {
    flag = 0;
  }
  if(millis() - delayUsers > 2000)
  {
    user1->on();
    user2->on();
    user3->on();
    user4->on();
    user5->on();
    delayUsers = millis();
  }
}

void setup() {
    // put your setup code here, to run once:
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  
  pinMode(menuButton, INPUT);
  //pinMode(producerButton1, INPUT);
  //pinMode(producerButton2, INPUT);
  //pinMode(batteryButton, INPUT);
  
  pinMode(producerUpperMosfet, OUTPUT);
  pinMode(producerLowerMosfet, OUTPUT);
  pinMode(batteryMosfet, OUTPUT);
  pinMode(user1->getPN(), OUTPUT);
  pinMode(user2->getPN(), OUTPUT);
  pinMode(user3->getPN(), OUTPUT);
  pinMode(user4->getPN(), OUTPUT);
  pinMode(user5->getPN(), OUTPUT);
  
  digitalWrite(producerUpperMosfet, LOW);
  digitalWrite(producerLowerMosfet, LOW);
  digitalWrite(batteryMosfet, HIGH);
  user1->on();
}

void loop() {

  /*if(millis() - delayDebug > 2000){
   user1->off();
   delayDebug = millis(); 
  }*/
  //digitalWrite(producerUpperMosfet, !digitalRead(producerButton1));
  //digitalWrite(producerLowerMosfet, !digitalRead(producerButton2));
  //digitalWrite(batteryMosfet, digitalRead(batteryButton));
   
  //измерение вольтажа
  //voltage = k*4.5f/1024*analogRead(A2);
  //A_y = A_y - A_K * (A_y - voltage);
  voltageA0 = (k * analogRead(A0))/1023;
  //voltageA1 = (k * analogRead(A1))/1023;
  if(millis() - delayAnalog1 > 2000){
  Serial.print(voltageA0);
  delayAnalog1 = millis();
  }

  
  //checkPress();
  producerSum = producerUpperCurrent.readCurrentDC() + producerLowerCurrent.readCurrentDC();

  checkUsers();
  pageSwitch(reg);
}
