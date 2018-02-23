/**
 * HC-SR04 Demo
 * Demonstration of the HC-SR04 Ultrasonic Sensor
 * Date: August 3, 2016
 * 
 * Description:
 *  Connect the ultrasonic sensor to the Arduino as per the
 *  hardware connections below. Run the sketch and open a serial
 *  monitor. The distance read from the sensor will be displayed
 *  in centimeters and inches.
 * 
 * Hardware Connections:
 *  Arduino | HC-SR04 
 *  -------------------
 *    5V    |   VCC     
 *    7     |   Trig     
 *    8     |   Echo     
 *    GND   |   GND
 *  
 * License:
 *  Public Domain
 */

// Pins
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;

// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;

int t1isr, t2isr;

void setup() {

  // The Trigger pin will tell the sensor to range find
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), ISR1, RISING);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), ISR2, FALLING);
  
  // We'll use the serial monitor to view the sensor output
  Serial.begin(9600);
}

void ISR1() {
  TCNT1 = 0;
  t1isr = 0;
}

void ISR2() {
  t2isr = TCNT1;
}

void loop() {

  // Timer/Counter registers
  // https://stackoverflow.com/questions/36254652/arduino-tcnt1-to-count-clock-cycles-between-interrupts
  // https://arduino-info.wikispaces.com/Timers-Arduino

  unsigned long t1, t1_tc;
  unsigned long t2, t2_tc;
  unsigned long pulse_width, pulse_width_tc, pulse_width_isr;
  float cm;
  float inches;

  // Hold the trigger pin high for at least 10 us
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Wait for pulse on echo pin
  while ( digitalRead(ECHO_PIN) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  t1 = micros();
  TCCR1B |= (1 << CS10);
  t1_tc = TCNT1 = 0;
  while ( digitalRead(ECHO_PIN) == 1);
  t2 = micros();
  t2_tc = TCNT1;
  pulse_width = t2 - t1;
  pulse_width_tc = t2_tc - t1_tc;
  pulse_width_isr = t2isr - t1isr;

  // Calculate distance in centimeters and inches. The constants
  // are found in the datasheet, and calculated from the assumed speed 
  //of sound in air at sea level (~340 m/s).
  cm = pulse_width / 58.0;
  inches = pulse_width / 148.0;

  // Print out results
  if ( pulse_width > MAX_DIST ) {
    Serial.println("Out of range");
  } else {
    Serial.print(pulse_width);
    Serial.print(" us \t");
    Serial.print(pulse_width_tc*16.0);
    Serial.print(" us \t");
    Serial.print(pulse_width_isr);
    Serial.print(" a.u. \t");
    Serial.print(cm);
    Serial.print(" cm \t");
    Serial.print(inches);
    Serial.println(" in");
  }
  
  // Wait at least 60ms before next measurement
  delay(60);
}
