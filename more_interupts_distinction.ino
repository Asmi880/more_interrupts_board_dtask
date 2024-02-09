// Declaring volatile boolean variables to keep track of states that may change within an interrupt service routine
volatile bool state = false;
volatile bool PIRstate = false;

// Declaring a variable for storing the timer1 compare match value
int timer1_compare_match;

// Defining an interrupt service routine for TIMER1 compare match
ISR(TIMER1_COMPA_vect) {
  // Empty ISR function
}

// Defining pin numbers for the PIR sensor and LED pins
const int pirPin = 6;
const int ledPin2 = 2;
const int ledPin3 = 3;
const int ledPin4 = 4;

// Setting motion detection threshold and check delay
const int motionThreshold = 1;
const int motionCheckDelay = 500;

// Variables for temperature measurement and baseline temperature
int celsius = 0;
int baselineTemp = 40;

// Variables for timing control
unsigned long previousMillis = 0;
const long interval = 2000;

void setup() {
  // Configuring the temperature sensor and PIR sensor pins as input
  pinMode(A0, INPUT);
  pinMode(pirPin, INPUT);
  // Initialize serial communication
  Serial.begin(9600);

  // Configuring LED pins as output
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);

  // Disabling interrupts during timer setup
  noInterrupts();
  // Clearing timer1 control registers
  TCCR1A = 0;
  TCCR1B = 0;
  // Setting the timer1 compare match value
  timer1_compare_match = 31249;
  // Initializing timer1 counter
  TCNT1 = timer1_compare_match;
  // Setting prescaler to 256
  TCCR1B |= (1 << CS12);
  // Enabling timer1 compare match interrupt
  TIMSK1 = (1 << OCIE1A);
  // Re-enabling interrupts
  interrupts();
}

void loop() {
  // Getting the current time in milliseconds
  unsigned long currentMillis = millis();

  // Reading the temperature sensor, convert it to Celsius, and print it
  celsius = map(((analogRead(A0) - 20) * 3.04), 0, 1023, -40, 125);
  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.println(" C");

  // Controling the LEDs based on the current temperature
  controlLEDs();

  // Checking for motion using the PIR sensor
  checkMotion();

  // Toggling an LED every 'interval' milliseconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    toggleLED();
  }
}

void controlLEDs() {
  // Initially turn all LEDs off
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  digitalWrite(ledPin4, LOW);

  // Turning on LEDs based on the temperature range
  if (celsius >= baselineTemp && celsius < baselineTemp + 10) {
    digitalWrite(ledPin2, HIGH);
  } else if (celsius >= baselineTemp + 10 && celsius < baselineTemp + 20) {
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
  } else if (celsius >= baselineTemp + 20) {
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(ledPin4, HIGH);
  }
}

void checkMotion() {
  // Reading the state of the PIR sensor
  int currentPirState = digitalRead(pirPin);
  // Waiting for a specified delay to check motion persistence
  delay(motionCheckDelay);
  // Re-checking the PIR sensor state
  int recheckPirState = digitalRead(pirPin);
  
  // If motion is detected in both checks, print a message
  if (currentPirState == HIGH && recheckPirState == HIGH) {
    Serial.println("Motion detected!");
  }
}

void toggleLED() {
  // Toggling the state of the fourth LED
  digitalWrite(ledPin4, !digitalRead(ledPin4));
}

