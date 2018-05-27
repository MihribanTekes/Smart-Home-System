#Mihriban Tekes
#include <LiquidCrystal.h> //http://playground.arduino.cc/Main/LiquidCrystal
#include <Keypad.h> //http://playground.arduino.cc/Code/Keypad
#include <Password.h> //http://playground.arduino.cc/Code/Password
#include <Servo.h>

Servo garaj;
Servo pencere;
String readString;
int passwd_pos = 7;
Password password = Password("1234"); //sifre atamasi

LiquidCrystal lcd (22, 23, 24, 25, 26, 27); 

const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {  //keypadin tanitilmasi
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 11, 10}; //keypad pinleri
byte colPins[COLS] = {9, 8, 7, 6};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); //keypad olusturulmasi

const int pirPin = 5; //pinlerin atanmasi
const int Relay = 39; 
const int redPin = 45;
const int bluePin = 44;
const int buzzer = 38;
const int smokeA0 = A5;
const int bahce_led = 29;
const int ldr = A3;
// Your threshold value
int sensorThres = 200;
const int ledError = 50;

const int timeDelay = 5000; //evi terketme suresi
int alarmStatus = 0;
int alarmActive = 0;

void setup(){
  lcd.begin(16,2);
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  pinMode(30, OUTPUT); 
  pinMode(28, OUTPUT); 
  garaj.attach(2);
  pencere.attach(3);
  pinMode(Relay, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(bahce_led, OUTPUT);
  keypad.addEventListener(keypadEvent);
  lcd.setCursor(0,0);
  lcd.print("Akilli Ev");
  lcd.setCursor(0,1); 
  lcd.print("Guvenlik Alarmi");
  delay(1600);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mihriban");
  lcd.setCursor(0,1); 
  lcd.print("Tekes");
  delay(2000);
  digitalWrite(bluePin, HIGH);
  displayCodeBlock(); 

 
  
}

void loop(){
  bluetooth();
  gazSensoru();
  bahce();
  keypad.getKey(); //keypadi okuyor
  if (alarmActive == 1){ //sifre dogru girilmisse alarmActive 1 oluyor ve pir sensoru high olup alarm sistemini calistiriyor
    if (digitalRead(pirPin) == HIGH){
      alarmTriggered();

    }
  }
}

void keypadEvent(KeypadEvent eKey){ //keypadin okuma kodu
  switch (keypad.getState()){
    case PRESSED:
    if (passwd_pos - 15 >= 5){
      return ;
    }
    lcd.setCursor((passwd_pos++),0);
    switch (eKey){
    case '#':
    passwd_pos  = 7;
    checkPassword(); 
    break;
    case '*':                 
    password.reset(); 
    passwd_pos = 7;
    break;
    default: 
    password.append(eKey);
    lcd.print("*");
   }
  }
}

void alarmTriggered(){ //alarm calisti
  password.reset();
  alarmStatus = 0;
  digitalWrite(Relay, HIGH); //buzzer high yap
  digitalWrite(redPin, HIGH); // kirmizi ledi high yap
  digitalWrite(bluePin, LOW);
  lcd.clear();
    displayCodeUnBlock(); //alarmi kapatmak icin fonks
    StrokeLight();{
    
  }
}   

void StrokeLight(){ //ledler icin
  for (int i=0; i<=10; i++){
    if(i%2){
    digitalWrite(redPin, HIGH);
    }
    else{
    digitalWrite(redPin, LOW);
    }
    delay(ledError);
  }
  for (int i=0; i<=10; i++){
    if(i%2){
      digitalWrite(bluePin, HIGH);
    }
    else{
      digitalWrite(bluePin, LOW);
    }
    delay(ledError);
  }
  digitalWrite(redPin, HIGH);
  delay(100);

}                                       

void checkPassword(){ //sifre dogrulugu
  if (password.evaluate()){
    if(alarmActive == 0 && alarmStatus == 0){ //devreyi aktif hale getirmek icin girilen sifre
      activate();
    } 
    else if( alarmActive == 1 || alarmStatus == 1){ //calan alarmi kapatmak icin girilen sifre
      deactivate();
    }
  } 
  else{
  invalidCode(); //hatali kod
 }
}  

void invalidCode(){
  digitalWrite(bluePin, LOW);
  password.reset();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Gecersiz Sifre");
  ledInvalid();
}

void ledInvalid(){ //hatali sifre de led durumu
  for (int i=0; i<=10; i++){
    if(i%2){
    digitalWrite(redPin, HIGH);
    }
    else{
    digitalWrite(redPin, LOW);
    }
    delay(ledError);
  }
  
  if(alarmActive == 0 && alarmStatus == 0){
    digitalWrite(Relay, LOW);
    digitalWrite(bluePin, HIGH);
    digitalWrite(redPin, LOW);
    delay(200);
    displayCodeBlock();
  } 
  else if( alarmActive == 1 || alarmStatus == 1){ //hareket algilandiysa
    digitalWrite(Relay, HIGH);
    digitalWrite(redPin, HIGH);
    digitalWrite(bluePin, LOW);
    delay(200);
  }
}

void activate(){ //sistemi aktif etmek ixin alarmActive degiskenine 1 atiyoruz
  alarmActive = 1;
  password.reset();
  digitalWrite(bluePin, LOW);
  digitalWrite(redPin, HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sistem aktif"); 
  delay(timeDelay);
}

void deactivate(){ //deaktif etmek icin tum degiskenleri sifirliyoruz
  alarmStatus = 0;
  alarmActive = 0;
  password.reset();
  digitalWrite(Relay, LOW);
  digitalWrite(bluePin, HIGH);
  digitalWrite(redPin, LOW);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Devre Disi");
  delay(1000);
  displayCodeBlock();
}

void displayCodeUnBlock(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sifre:");
  lcd.setCursor(0,1);
  lcd.print("Giriniz");
}

void displayCodeBlock(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sifre:");
  lcd.setCursor(0,1);
  lcd.print("Giriniz");
}

void gazSensoru()
{
  int analogSensor = analogRead(smokeA0);

  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > sensorThres)
  {

    digitalWrite(buzzer,HIGH);
  }
  else
  {

    digitalWrite(buzzer,LOW);
  }
  delay(100);
}

void bahce()
{
 int blinkRate = analogRead(ldr);
 Serial.println(blinkRate);
 delay(10000);
 if(blinkRate < 100)
 {
   digitalWrite(bahce_led, HIGH);
 }
 else
 {
   digitalWrite(bahce_led, LOW);
 }
}
void bluetooth()
{
  while (Serial.available()) {
    delay(3);  
    char c = Serial.read();
    readString += c; 
  }
  if (readString.length() >0) {
    Serial.println(readString);
    
    if (readString == "Oac")     
    {
      digitalWrite(30,HIGH);
    }
        if (readString == "Okapat")     
    {
      digitalWrite(30,LOW);
    }
        if (readString == "Mac")     
    {
      digitalWrite(28,HIGH);
    }
        if (readString == "Mkapat")     
    {
      digitalWrite(28,LOW);
    }
        if (readString == "Pac")     
    {
      pencere.write(179);
    }
        if (readString == "Pkapat")     
    {
      pencere.write(3);
    }
        if (readString == "Gac")     
    {
      garaj.write(179);
    }
        if (readString == "Gkapat")     
    {
      garaj.write(1);
    }
            if (readString == "Kac")     
    {
      
    }
            if (readString == "Kkapat")     
    {
      
    }
    readString="";
  }
}
