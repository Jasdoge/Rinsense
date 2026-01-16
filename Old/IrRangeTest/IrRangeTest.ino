// Macros to toggle ADC (turn off for lower battery consumption)
#define adc_disable() ADCSRA &= ~ bit(ADEN) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC


// PIN MAPPING
const uint8_t PIN_RED = 1;
const uint8_t PIN_GREEN = 0;

const uint8_t PIN_IR_OUT = 2;
const uint8_t PIN_IR_IN = 3;
#define PIN_IR_IN_A A3
const uint8_t PIN_BUZZER = 4;

void setup(){

	adc_enable();

	// Setup pin defaults
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_BUZZER, OUTPUT);
	pinMode(PIN_IR_OUT, OUTPUT);
	pinMode(PIN_IR_IN, INPUT);

	digitalWrite(PIN_GREEN, LOW);
	digitalWrite(PIN_RED, LOW);
	digitalWrite(PIN_BUZZER, LOW);
	digitalWrite(PIN_IR_OUT, LOW);

	// Diode test (flash green -> yellow -> red)
	digitalWrite(PIN_GREEN, HIGH);
	delay(250);
	digitalWrite(PIN_RED, HIGH);
	delay(250);
	digitalWrite(PIN_GREEN, LOW);
	delay(250);
	digitalWrite(PIN_RED, LOW);
	

}

void loop(){


	delay(1);
	int reading = analogRead(PIN_IR_IN_A);
	digitalWrite(PIN_IR_OUT, HIGH);
	delay(1);
	bool near = analogRead(PIN_IR_IN_A)-reading > 15;
	digitalWrite(PIN_IR_OUT, LOW);

	analogWrite(PIN_RED, near ? 5 : 0);


}

