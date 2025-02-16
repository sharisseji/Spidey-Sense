#define PERSON_PIN 6  // Match ESP32-CAM GPIO
#define VEHICLE_PIN 7   // Match ESP32-CAM GPIO
#define ANIMAL_PIN 8  // Match ESP32-CAM GPIO
#define OBST_PIN 9   // Match ESP32-CAM GPIO

void setup() {
  Serial.begin(9600);
  
  pinMode(PERSON_PIN, INPUT);
  pinMode(VEHICLE_PIN, INPUT);
  pinMode(ANIMAL_PIN, INPUT);
  pinMode(OBST_PIN, INPUT);
  
  //pinMode(9, OUTPUT);  // Buzzer or speaker
}

void loop() {
  if (digitalRead(PERSON_PIN) == HIGH) {
    // tone(9, 1000, 5000);
    Serial.println("Person Detected");
  } else if (digitalRead(VEHICLE_PIN) == HIGH) {
    // noTone(9);
    Serial.println("Vehicle Detected");
  }
  else if (digitalRead(ANIMAL_PIN) == HIGH) {
    // noTone(9);
    Serial.println("Animal Detected");
  }
  else if (digitalRead(OBST_PIN) == HIGH) {
    // noTone(9);
    Serial.println("Obstruction Detected");    
  }
  // else{
  //   // noTone(9);
  // }
  delay(500);
}
