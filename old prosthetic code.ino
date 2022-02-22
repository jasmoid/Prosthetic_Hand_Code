// This program works by interpreting an EMG signal and moving a motor in relation to the EMG,
// The program also works to monitor the motor temperature to avoid overheating,
// the joint position to avoid undesired positions and finally uses a push button to recognise pressure.
// Jasmine Usher, 26/03/21

#include <Servo.h>

Servo myservo;

int emgPin = A0;
int emg;

int thermistorPin = A2;
int potPin = A4;
int buttonPin = 2;

int motorpos;

void setup() {

  myservo.attach(9);
  Serial.begin(9600);
}

int tooHot(int x){
  // this function checks the analogue temperature value of the motor and then converts it to 
  // a temperature value. If the temperature is too hot, the motor turns off.
  
  int thermistor = analogRead(thermistorPin); // get analogue temperature value
  float voltage = (thermistor*5.0)/1024; // convert to voltage
  float temp = (voltage-3.6596)/-0.0478; // calibration function
  
  if (temp >= x){ // if voltage temp too high
    Serial.print("Temp: "); 
    Serial.println(temp);
    myservo.detach(); // detach servo
  }
  else{
    Serial.print("Temp: "); 
    Serial.println(temp);
  }
} 

int joints(int pos, int x){
  // This function checks the postition of the joints and if they are in an undesired position, turns off the motor.
  
   if (pos >= x){ // if joint angle is too big
    Serial.print("Joint Postition: ");
    Serial.println(pos);
    myservo.detach(); // detach servo
   }
   else{
     Serial.print("Joint Postition: ");
     Serial.println(pos);
   }
}

void nobutton(){
  // This runs only when the button is not pressed
    
    tooHot(45); // check temp
    
    Serial.println("Button not pressed");
    emg = analogRead(emgPin); // read emg analogue value
    
    int motorpos = map(emg, 0, 1023, 0, 180); // convert emg value to value readable by motor
    
    if (emg > 80){ // if emg is big enough, move the motor
      myservo.write(motorpos); 
      Serial.print("Motor Position: ");
      Serial.println(motorpos);

      int pot = analogRead(potPin); // get analogue joint position
      int jointpos = map(pot, 0, 1023, 0, 300); // convert to angle, this potentiometer turns 300 degrees according to datasheet and inspection

      joints(jointpos, 90); // passing joint position to joint reading function. normal finger joints do not go past 90 degrees.
  }
}
  
void buttonpress(){
  // Runs when button is pressed
  
    tooHot(45);
    
    Serial.println("Button pressed");
    delay(15); // wait
    int newemg = analogRead(emgPin); // read a new emg
    
    if (newemg > emg-10){ // previous emg may be the maximum possible so a tolerance is needed, my maximum is around 80
         int motorpos = map(newemg, 0, 1023, 0, 180); // convert new emg value to one that can be read by the motor
         myservo.write(-motorpos); // move the motor to let go of object, this moves it backwards to origial position
         Serial.print("Motor Position: ");
         Serial.println(motorpos);

         int pot = analogRead(potPin); // get analogue joint position
         int jointpos = map(pot, 0, 1023, 0, 300); // convert to angle, this potentiometer turns 300 degrees according to datasheet and inspection

         joints(jointpos, 90); // passing joint position to joint reading function. normal finger joints do not go past 90 degrees.
    }
}

void loop() {

    int buttonState = digitalRead(buttonPin); // check button state
    if (buttonState != 1){ // if button isn't pressed
      nobutton();
    }
    else{
      buttonpress(); // button is pressed
    }
}
