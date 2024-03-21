#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;
#define PIN_IN1 18
#define PIN_IN2 15
#define PIN_ENA 19
#define Encoder_A 2
#define Encoder_B 3

float position_counter = 0;
int counter_RED;
int counter_GREEN;
int counter_BLUE;

//BELOW IS THE INTERRUPTS ISR TO SEE IF MOTOR IS ALREADY IN THE POSITION ITS SUPPOSED TO BE IN
void IRAM_ATTR position_check() {
  Serial.println("position check working");
  if (counter_RED > 15) {
    if (position_counter == 1000) {
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, LOW);
      position_counter = 0; //STATIONARY
    }
  }
  if (counter_GREEN > 15) {
    if (position_counter == 500) {
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, LOW); //STATIONARY
      position_counter = 0;
    }
  }
  if (counter_BLUE > 15) {
    if (position_counter == 0) {
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, LOW); //STATIONARY
      analogWrite(PIN_ENA, 0); // Stop the motor
      position_counter = 0;
      Serial.println("INTERRUPT WORKING");
    }
  }
}


void position_reader() //THIS FUNCTION FINDS POSITION OF THE MOTOR
{
  int b = digitalRead(Encoder_B);
  if (b > 0) {
    position_counter++;
  }
  else{
    position_counter--;
  }
//
//    if ((PIN_IN1 == HIGH)&&(PIN_IN2 == LOW)){
//      position_counter++;
//      }
//      if((PIN_IN1 == LOW)&&(PIN_IN2 == HIGH)){
//        position_counter--;
//        }
}



void setup() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENA, OUTPUT);
  pinMode(Encoder_A, INPUT);
  pinMode(Encoder_B, INPUT);

  Serial.begin(9600);
  Wire.begin(21, 20);//telling BH1750 (SDA,SCL)
  lightMeter.begin();

  attachInterrupt(digitalPinToInterrupt(Encoder_A), position_reader , RISING);
  //attachInterrupt(digitalPinToInterrupt(Encoder_A), position_check , RISING);

}





void loop() {
  int a = digitalRead(Encoder_A);
  int b = digitalRead(Encoder_B);

  counter_RED = 0;
  counter_GREEN = 0;
  counter_BLUE = 0;

  //BELOW READS THE LIGHTMETER 20 TIMES AND GIVES IT A COLOR DEPENDING ON LIGHT LEVEL
  for (int i = 0; i <= 20 ; i++) {
    float lux = lightMeter.readLightLevel();
    if ( lux < 1000) {
      counter_BLUE++;
      
//      postition_counter--; 
//      Serial.print("Light: ");
//      Serial.print(lux);
      Serial.print("  BLUE  ");
//      Serial.println(counter_BLUE);
      Serial.print("Counter: ");
      Serial.println(position_counter);

    }
    if (( lux > 1000) && (lux < 5000)) {
      counter_RED++;
//      Serial.print("Light: ");
//      Serial.print(lux);
      Serial.print("  RED  ");
//      Serial.println(counter_RED);
      Serial.print("Counter: ");
      Serial.println(position_counter);
    }
    if ( lux > 5000) {
      counter_GREEN++;
//      Serial.print("Light: ");
//      Serial.print(lux);
      Serial.print("  GREEN  ");
//      Serial.println(counter_GREEN);
      Serial.print("Counter: ");
      Serial.println(position_counter);
    }
      delay(1000);

  }

  //BELOW TELLS THE MOTOR TO TURN CC OR CCW OR NOT, DEPENDING ON POSITION_COUNTER
  if (counter_RED > 15) { //Bring all the way down
    neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0); //RED
    if (position_counter < 10000) {
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH); //counterclockwise (down)
      analogWrite(PIN_ENA, 255);
      position_counter++;
    }
    if (position_counter == 10000) {
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, LOW);//stationary
      analogWrite(PIN_ENA, 0);
      position_counter = 0;
    }
  }
  if (counter_GREEN > 15) { // blind half way
    neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0); //GREEN
    if (position_counter > 5000 && position_counter < 10000) {
      digitalWrite(PIN_IN1, HIGH); //clockwise (up)
      digitalWrite(PIN_IN2, LOW);
      analogWrite(PIN_ENA, 255);
      position_counter--;

   
    }
    if (position_counter < 5000) {
      digitalWrite(PIN_IN1, LOW); //counterclockwise (down)
      digitalWrite(PIN_IN2, HIGH);
      analogWrite(PIN_ENA, 255);
      position_counter ++;

    
    }
    if (position_counter == 5000) {
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, LOW); //STATIONARY
      analogWrite(PIN_ENA, 0);
      position_counter = 0;

   
    }
  }
  if (counter_BLUE > 15) { //Bling all the way up
    neopixelWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS); //BLUE
    //    Serial.println(position_counter);
    if (position_counter > 0) {
      digitalWrite(PIN_IN1, HIGH); //clockwise (up)
      digitalWrite(PIN_IN2, LOW);
      analogWrite(PIN_ENA, 255);
      position_counter --;


    }
    else if (position_counter == 0) {
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, LOW);//STATIONARY
      analogWrite(PIN_ENA, 0);
      position_counter = 0;
     
    }
  }
} //END OF LOOP






//esp_sleep_enable_timer_wakeup(5*1000000);//CHANGE FIRST VALUE in SECONDS
//esp_deep_sleep_start();