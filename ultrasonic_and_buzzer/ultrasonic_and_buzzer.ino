// Right buzzer pins
const int buzzLeft = 2; 
const int buzzRight = 12;

// Ultrasonic sensor pins 
const int trigPin1 = 3; // left
const int echoPin1 = 4; 
const int trigPin2 = 10; // right
const int echoPin2 = 11;
const int trigPin3 = 5; // centre
const int echoPin3 = 6; 

// Distance threshold
const int DISTANCE_THRESHOLD = 200; //centimetres

int duration1, duration2, duration3;
float left_distance, right_distance, middle_distance;

void setup() {
  pinMode(buzzLeft, OUTPUT);
  pinMode(buzzRight, OUTPUT);

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);

  Serial.begin(9600); //begins serial communication with computer
}

void loop() {
  checkProximity();
  delay(200); // checks again every 2s
}

// function that checks distance to wall and acts accordingly.
void checkProximity() {
  
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);

  duration1 = pulseIn(echoPin1, HIGH);
  left_distance = (duration1*0.034)/2;

  Serial.print("Left Distance: ");
  Serial.println(left_distance);

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);

  duration2 = pulseIn(echoPin2, HIGH);
  right_distance = (duration2*0.034)/2;

  Serial.print("Right Distance: ");
  Serial.println(right_distance);

  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);

  duration3 = pulseIn(echoPin3, HIGH);
  middle_distance = (duration3*0.034)/2;

  Serial.print("Middle Distance: ");
  Serial.println(middle_distance);

  digitalWrite(buzzLeft, LOW);
  digitalWrite(buzzRight, HIGH);

  // Case 1: LC
  if ((left_distance <= DISTANCE_THRESHOLD) && (middle_distance <= DISTANCE_THRESHOLD) && (right_distance >= DISTANCE_THRESHOLD)) {
    Serial.println("Obstacle detected on left side and center.");
    digitalWrite(buzzLeft, HIGH);
    delay(20);
    digitalWrite(buzzLeft, LOW);
  // Case 2: RC
  } else if ((left_distance >= DISTANCE_THRESHOLD) && (middle_distance <= DISTANCE_THRESHOLD) && (right_distance <= DISTANCE_THRESHOLD)) {
    Serial.println("Obstacle detected on right side and center.");
    digitalWrite(buzzRight, LOW);
    delay(20);
    digitalWrite(buzzRight, HIGH);

  // Case 3: C 
  } else if ((left_distance >= DISTANCE_THRESHOLD) && (middle_distance <= DISTANCE_THRESHOLD) && (right_distance >= DISTANCE_THRESHOLD)) {
    Serial.println("Obstacle detected on center.");
    digitalWrite(buzzLeft, HIGH);
    delay(20);
    digitalWrite(buzzLeft, LOW);

  // Case 4: NOTHING
  } else { 
    Serial.println("All clear.");
    digitalWrite(buzzLeft, LOW);
    digitalWrite(buzzRight, HIGH);
  }
}




  