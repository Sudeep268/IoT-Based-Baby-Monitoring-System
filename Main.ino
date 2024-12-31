 #include <ESP32Servo.h>  
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
//variables for servo
int pos;

//variables for water sensor 
float S = A0; // S is for water sensor 
float waterValue = 0; //capturing the water sensor value 
int buzzer = 26;

//variables for sound sensor  
float soundpin = 34;
float soundValue;

//variables for message
int flagW =1;
int flagC =1;
int flagB =1;
int music =2;

// Replace with your network credentials
const char* ssid = "Your_SSID";
const char* password ="Your_password";

// Initialize Telegram BOT
#define BOTtoken "your_bot_token"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "Your_Chat_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  Serial.begin(9600);
  myservo.attach(14);
  myservo.write(0);
  pinMode(S, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(soundpin, INPUT);

  lcd.begin(); //initilize the lcd monitor
  lcd.backlight(); // Turn on the blacklight and print a message.
  lcd.print("Welcome to !!!");
  delay(2000);
  lcd.clear();
  lcd.print("IoT based");
  lcd.setCursor (0,1);
  lcd.print("Baby Monitoring");
  delay(1500);
  lcd.clear();
  lcd.print("System");
  delay(1000);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  lcd.clear();
  lcd.print("  Connecting");
  lcd.setCursor (0,1);
  lcd.print("  WiFi.......");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  lcd.clear();
  lcd.print("WiFi connected");
  Serial.println("");
  Serial.println("WiFi connected");
  delay(2000);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {
  
  lcd.clear();
  lcd.print("Baby Status:");
  lcd.setCursor (0,1);
  lcd.print("Normal");
  
  waterValue =  waterSensor();
  //getting the sound sensor value and printing it 
  soundValue = soundSensor();
  if(waterValue >= 300 && soundValue >=60)
  {
    AAA: 
    digitalWrite(buzzer,HIGH);
    lcd.clear();
    lcd.print("Baby Status:");
    lcd.setCursor (0,1);
    lcd.print("Baby is wet ");
    delay(1500);
    lcd.clear();
    lcd.print("& crying");
    both();
    servoRotation();
    soundValue = soundSensor();
    waterValue =  waterSensor();
    if(waterValue >= 300 && soundValue >=60)
        {
          goto AAA;
        }
        else
        {
          digitalWrite(buzzer,LOW);
          lcd.clear();
          lcd.print("Baby Status:");
          lcd.setCursor (0,1);
          lcd.print("Normal");
        }
  }
  flagB = 1;
  if(waterValue >= 300 && soundValue <=60)
    { A: 
        digitalWrite(buzzer,HIGH);
        lcd.clear();
        lcd.print("Baby Status:");
        lcd.setCursor (0,1);
        lcd.print("Baby is wet");
        wet();
        //Cheaking the sound again 
        waterValue = waterSensor();
        if(waterValue >= 300)
        {
          goto A;
        }
        else
        {
          digitalWrite(buzzer,LOW);
          lcd.clear();
          lcd.print("Baby Status:");
          lcd.setCursor (0,1);
          lcd.print("Normal");
        }
    }
   flagW = 1;
 
  //Servo spin 
  if(soundValue >= 60 && waterValue<= 300)
    { AA:
        lcd.clear();
        lcd.print("Baby Status:");
        lcd.setCursor (0,1);
        lcd.print("Crying"); 
        cry();
        servoRotation();
        //Cheaking the sound again 
        soundValue = soundSensor();
        if(soundValue >= 30)
        {
          goto AA;
        }
        else
        {
          lcd.clear();
          lcd.print("Baby Status:");
          lcd.setCursor (0,1);
          lcd.print("Normal");
        }
    }
  flagC =1; 
}


void servoRotation()
{
   int pos;
   for(int i =1; i<=3; i += 1)
   {
     for(pos = 0; pos<=150; pos += 3)
      {
        myservo.write(pos);
        delay(15);
      }
      
      delay(500);
      
      for(pos = 150; pos>=0; pos -= 3)
      {
        myservo.write(pos);
        delay(15);
      }
  }
}

float waterSensor()
{
  float waterValue =0;
  float S = A0;
  //getting the water sensor value and printing it 
  for(int i = 0; i<25; i++)
  {
  waterValue+=  analogRead(S); // Reading the gas value from analog pin
  delay(10);
  }
  waterValue /= 25;
  Serial.print("Water value = ");
  Serial.print(waterValue);
  Serial.print("\n");
  return waterValue;
}

float soundSensor()
{
  float soundValue =0;
  float soundpin = 34;
  for(int j = 0; j<200; j++)
  {
  soundValue+=  analogRead(soundpin); // Reading the gas value from analog pin
  delay(10);
  }
  soundValue /= 200;
  Serial.print("Sound value = ");
  Serial.print(soundValue);
  Serial.print("\n");
  return soundValue;
}

void wet()
{
  if(flagW== 1)
  {
    bot.sendMessage(CHAT_ID, "Baby is wet", "");
    flagW +=1; 
  }
}

void cry()
{
  if(flagC== 1)
  {
    bot.sendMessage(CHAT_ID, "Baby is crying", "");
    flagC +=1; 
  }
}

void both()
{
  if(flagB == 1)
  {
    bot.sendMessage(CHAT_ID, "Baby is wet & crying", "");
    flagB += 1;
  }
}
