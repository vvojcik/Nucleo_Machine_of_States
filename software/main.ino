const int trigPin = 9;
const int echoPin = 8;
const int ledPin = 7;
const int buttonPin = USER_BTN;

enum SystemState {
  STATE_IDLE,
  STATE_MEASURING,
  STATE_ALARM
};

SystemState currentState = STATE_IDLE; //tryb czuwania

unsigned long previousMillis = 0;
const long interval = 500;

int buttonState = HIGH; 
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; 

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  pinMode(buttonPin, INPUT);

  digitalWrite(ledPin, LOW);
  
  Serial.println("System gotowy.");
  Serial.println("Nacisnij NIEBIESKI USER BUTTON, aby rozpoczac pomiary.");
}

void loop() {
  int reading = digitalRead(buttonPin);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {

        if (currentState == STATE_IDLE) {
          currentState = STATE_MEASURING;
          Serial.println(">>> ROZPOCZYNAM POMIARY <<<");
        } 
        else if (currentState == STATE_MEASURING) {
          currentState = STATE_IDLE;
          Serial.println(">>> SYSTEM WSTRZYMANY (IDLE) <<<");
        }
        else if (currentState == STATE_ALARM) {
          currentState = STATE_MEASURING;
          digitalWrite(ledPin, LOW);
          Serial.println(">>> ALARM SKASOWANY! Powrot do pomiarow <<<");
        }
      }
    }
  }
  lastButtonState = reading;

  switch (currentState) {
    
    case STATE_IDLE:
      break;

    case STATE_MEASURING:
      if (millis() - previousMillis >= interval) {
        previousMillis = millis();
        
        float distance = getDistance();
        
        Serial.print("Aktualna odleglosc: ");
        Serial.print(distance);
        Serial.println(" cm");

        if (distance > 0 && distance < 15.0) {
          currentState = STATE_ALARM;
          digitalWrite(ledPin, HIGH); // dioda on
          Serial.println("!!! ALARM !!! Wykryto ruch ponizej 15 cm!");
        }
      }
      break;

    case STATE_ALARM:
      break;
  }
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); 
  
  if (duration == 0) {
    return 0; //poza zasiegiem
  }

  return (duration * 0.0343) / 2.0;
}