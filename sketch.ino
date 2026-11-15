#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
SCREEN_WIDTH,
SCREEN_HEIGHT,
&Wire,
-1
);

// Ultrasonic
#define TRIG 5
#define ECHO 18

// LEDs
#define GREEN_LED 14
#define YELLOW_LED 27
#define RED_LED 26

// Buzzer
#define BUZZER 25

// Emergency Button
#define EMERGENCY_BTN 4

long duration;
float distance;

bool emergencyMode = false;

void setup() {

  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  pinMode(EMERGENCY_BTN, INPUT_PULLUP);

  if(!display.begin(
  SSD1306_SWITCHCAPVCC,
  0x3C))
  {
    Serial.println("OLED Failed");
    while(true);
  }

  display.clearDisplay();
  display.display();
}

float getDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);

  distance = duration * 0.034 / 2;

  return distance;
}

void showFreeRunway()
{
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  noTone(BUZZER);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);
  display.println("RUNWAY STATUS");

  display.setCursor(0,20);
  display.println("RUNWAY FREE");

  display.setCursor(0,40);
  display.println("Next: A101");

  display.display();
}

void showOccupiedRunway()
{
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(RED_LED, HIGH);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);
  display.println("RUNWAY STATUS");

  display.setCursor(0,20);
  display.println("AIRCRAFT LANDING");

  display.setCursor(0,40);
  display.println("ID: A101");

  display.display();
}

void emergencyLanding()
{
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);

  for(int i=0;i<12;i++)
  {
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 2000);

    delay(150);

    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);

    delay(150);
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);
  display.println("!!! EMERGENCY !!!");

  display.setCursor(0,15);
  display.println("AIRCRAFT: E001");

  display.setCursor(0,30);
  display.println("PRIORITY GRANTED");

  display.setCursor(0,45);
  display.println("RUNWAY RESERVED");

  display.display();

  delay(5000);

  emergencyMode = false;
}

void loop()
{

  distance = getDistance();

  if(digitalRead(EMERGENCY_BTN)==LOW)
  {
      emergencyMode = true;
  }

  if(emergencyMode)
  {
      emergencyLanding();
  }
  else
  {
      if(distance < 50)
      {
          showOccupiedRunway();
      }
      else
      {
          showFreeRunway();
      }
  }

  Serial.print("Distance: ");
  Serial.println(distance);

  delay(500);
}