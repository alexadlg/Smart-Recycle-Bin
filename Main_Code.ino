#include <Servo.h>
#include <LiquidCrystal.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_TCS34725.h>


HX711 scale;

float calibration_factor = 1016;                            // this calibration factor is adjusted according to my load cell
float units;
float ounces;
int sensorValue;
float voltage;
int steps;
uint16_t r, g, b, c, colorTemp, lux;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
 
#define dirPin 2
#define stepPin 3
#define dirPin2 4
#define stepPin2 5


//LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
int materialSelector;

void setup() {                                              // put your setup code here, to run once:
  Serial.begin(9600); //initiates serial comunication
  //lcd.begin(16, 2);
  if (tcs.begin()) {
    Serial.println("Found sensor");
  }
  else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
   }
  
  pinMode(dirPin,OUTPUT);
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin2,OUTPUT);
  pinMode(stepPin2,OUTPUT); 

  scale.begin(6, 7);
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  
}

void loop() {                                                // put your main code here, to run repeatedly:

  while (true) {
    //Check Code by uncomenting
  
  // check sensorValue or voltage outputs, change whats inside ()
  Serial.print("Reading: ");
  Serial.print(units); Serial.println(" grams"); 
  //Serial.print(calibration_factor); Serial.print(" calibration_factor: ");
  Serial.print(voltage ); Serial.println(" volts"); 
  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
  

  
    
  sensorValue = analogRead(A0);
    voltage = sensorValue * (12.0 / 1023.0);              //stores the analog signal from the induction sensor into sensor value
  scale.set_scale(calibration_factor); //Adjust to this calibration factor

tcs.getRawData(&r, &g, &b, &c);
colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
lux = tcs.calculateLux(r, g, b);
  
  units = scale.get_units(5), 20;
  if (units < 0)
  {
    units = 0.00;
  }
  ounces = units * 0.035274;
    if (voltage >= 9){                  //aluminum
      materialSelector = 1;
      break;
    }
    if (voltage < 2 ){     // Plastic
      if(units > 4 && units < 10){
      materialSelector = 2;
      break;
    }}
    if ((g >= 1100 && g <= 1300) && (c >= 2900 && c <= 3500) && units > 30 && units < 50 ) {    //brown glass
      materialSelector = 3;
      break;
    }
    if (units > 60 && units < 65) {    //clear glass
      materialSelector = 4;
      break;
    }
    if ((g >= 1350 && g <= 1600) && (c >= 3450 && c <= 3900) && units > 30 && units < 50) {                 //green glass
      materialSelector = 5;
      break;
    }
  }

  switch (materialSelector) {
    case 1:
      // Move the container to aluminum
       /* lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Aluminum Can");
      lcd.setCursor(0, 1);
      lcd.print("Detected Thanks!");*/
      movec(2000); // Move clockwise 2000 steps or position 1 
      delay(1000);
      
      opendoor(250); // Open the door with Stepper Motor
      delay(5000);
      closedoor(250);
      delay (1000);
      
      movecc(2000);// Move the container back to starting position
      Serial.println("case1");
      break;

    case 2:
      // Move the container to green glass
      /* lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Green Glass");
      lcd.setCursor(0, 1);
      lcd.print("Detected Thanks!");*/
      movec(6000); // Move clockwise 6000 steps 
      delay(1000);
      
      opendoor(250);// Open the door with the servo
      delay(5000);
      closedoor(250);
      delay (1000);
      
      movecc(6000); // Move the container back to starting position
      Serial.println("case2");
      break;

      case 3:
      // Move the container to glass brown
      /* lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Brown Glass");
      lcd.setCursor(0, 1);
      lcd.print("Detected Thanks!");*/
      movec(10000); // Move clockwise 100 steps 
      delay(1000);
      
      opendoor(250);// Open the door with the servo
      delay(5000);
      closedoor(250);
      delay (1000);
      
      movecc(10000);// Move the container back to starting position
      Serial.println("case3");
      break;

      case 4:
      // Move the container to glass clear
      /* lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Clear Glass");
      lcd.setCursor(0, 1);
      lcd.print("Detected Thanks!");*/
      movecc(2000); // Move ounter clockwise 20 steps 
      delay(1000);
      
      opendoor(250);// Open the door with the servo
      delay(5000);
      closedoor(250);
      delay (1000);
      
      movec(2000);// Move the container back to starting position
      Serial.println("case4");
      break;

      case 5:
      // Move the container to plastic
      /* lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Plastic Bottle");
      lcd.setCursor(0, 1);
      lcd.print("Detected Thanks!");*/
      movecc(6000); // Move counter clockwise 60 steps 
      delay(1000);
      
      opendoor(250);// Open the door with the servo
      delay(5000);
      closedoor(250);
      delay (1000);
      
      movec(6000);// Move the container back to starting position
      Serial.println("case5");
      break;
}
}
      
// == Custom functions ==

void movec (int steps) {
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for (int x = 0; x < steps; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(600); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(600);
  }
}
void movecc (int steps) {
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for (int x = 0; x < steps; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(600);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(600);
  }
}
void opendoor (int steps) {
  digitalWrite(dirPin2,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for (int x = 0; x < steps; x++) {
    digitalWrite(stepPin2,HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin2,LOW);
    delayMicroseconds(1000);
  }
}
void closedoor (int steps) {
  digitalWrite(dirPin2,HIGH); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for (int x = 0; x < steps; x++) {
    digitalWrite(stepPin2,HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin2,LOW);
    delayMicroseconds(1000);
  }
}  
