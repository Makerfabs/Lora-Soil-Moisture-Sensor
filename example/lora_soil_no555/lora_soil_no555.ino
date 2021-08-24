

int sensorPin = A2;  // select the input pin for the potentiometer
int sensorValue = 0; // variable to store the value coming from the sensor
int sensorPowerCtrlPin = 5;
int pwm_pin = 9;

void setup()
{
    Serial.begin(115200);

    pinMode(pwm_pin, OUTPUT);
    pinMode(sensorPowerCtrlPin, OUTPUT);
    digitalWrite(sensorPowerCtrlPin, HIGH); //Sensor power on

    // set up Timer 1
    TCCR1A = bit(COM1A0);            // toggle OC1A on Compare Match
    TCCR1B = bit(WGM12) | bit(CS10); // CTC, scale to clock
    OCR1A = 6;                       // compare A register value (5000 * clock speed / 1024)

    delay(100);
}

void loop()
{
    delay(100);
    sensorValue = analogRead(sensorPin);

    Serial.print(F("Moisture ADC : "));
    Serial.println(sensorValue);

    delay(1000);
}
