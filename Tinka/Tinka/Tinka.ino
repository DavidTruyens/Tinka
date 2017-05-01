/*
 Name:		Tinka.ino
 Created:	4/29/2017 10:07:07 AM
 Author:	davidtruyens
*/

// the setup function runs once when you press reset or power the board
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

//accelero
Adafruit_MMA8451 mma = Adafruit_MMA8451();

//led
#define PIN 6
#define NUM_LEDS 7
#define BRIGHTNESS 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

byte neopix_gamma[] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
	1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
	2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
	5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


void setup() {
	//initiate serial
	Serial.begin(9600);
	//while (!Serial);
	//initiate accelero
	if (!mma.begin()) {
		Serial.println("Couldnt start accelero");
		while (1);
	}
	Serial.println("MMA8451 found!");

	mma.setRange(MMA8451_RANGE_2_G);

	Serial.print("Range = "); Serial.print(2 << mma.getRange());
	Serial.println("G");

	//initiate mageneto

	//initiate led
	strip.setBrightness(BRIGHTNESS);
	strip.begin();
	strip.show(); // Initialize all pixels to 'off'

}

//settings
bool LedOn = false;
uint32_t timeout = 1000;
uint32_t timer = 0;
uint8_t previousState = MMA8451_PL_LRB;
uint32_t debugger = 0;
uint32_t debuggertimeout = 100;

//turn on
uint32_t timer1 = 0;
uint8_t State1 = MMA8451_PL_LRB;
uint32_t onTimeout = 2000;

/*
Upright = MMA8451_PL_LRB
Upsidedown = MMA8451_PL_LRF
*/


// the loop function runs over and over again until power down or reset
void loop() {

	//if led off, check accelero
	mma.read();

	/* Get a new sensor event */
	sensors_event_t event;
	mma.getEvent(&event);

	/* Get the orientation of the sensor */
	uint8_t o = mma.getOrientation();

	if (LedOn) {
		switch (o) {
		case MMA8451_PL_PUF:
			Serial.println("Portrait Up Front");
			colorWipe(strip.Color(255, 0, 0), 50); // Red
			colorWipe(strip.Color(0, 255, 0), 50); // Green
			break;
		case MMA8451_PL_PUB:
			Serial.println("Portrait Up Back");
			colorWipe(strip.Color(0, 0, 255), 50); // Blue
			colorWipe(strip.Color(0, 0, 0, 255), 50); // White
			break;
		case MMA8451_PL_PDF:
			Serial.println("Portrait Down Front");
			whiteOverRainbow(20, 75, 5);
			break;
		case MMA8451_PL_PDB:
			Serial.println("Portrait Down Back");
			pulseWhite(5);
			break;
		case MMA8451_PL_LRF:
			Serial.println("Landscape Right Front");
			fullWhite();
			break;
		case MMA8451_PL_LRB:
			Serial.println("Landscape Right Back");
			rainbowCycle(5);
			delay(2000);
			break;
		case MMA8451_PL_LLF:
			Serial.println("Landscape Left Front");
			fullWhite();
			delay(2000);
			break;
		case MMA8451_PL_LLB:
			Serial.println("Landscape Left Back");
			fullWhite();
			delay(2000);
			break;
		}
	}
	
	else {
		if (State1 != o) {
			Serial.println("status change");
			State1=o;
			timer1 = millis();
			if ((millis() - timer1) <= onTimeout) {
				Serial.println("leds on");
				LedOn = true;
			}
		}
	}

	if (o != previousState) {
		previousState = o;
		timer = millis();
	}

	if ((millis()-timer) > timeout) {
		//Serial.println("Black out");
		blackout();
		LedOn = false;
	}



	//if ((millis() - debugger) >= debuggertimeout) {
	//	Serial.print("timer: ");
	//	Serial.println(millis() - timer);
	//}




	//
	
}


// Turn off
void blackout() {
	for (uint16_t i = 0; i < strip.numPixels(); i++) {
		strip.setPixelColor(i, 0, 0, 0, 0);
	}
	strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
	for (uint16_t i = 0; i<strip.numPixels(); i++) {
		strip.setPixelColor(i, c);
		strip.show();
		delay(wait);
	}
}

void pulseWhite(uint8_t wait) {
	for (int j = 0; j < 256; j++) {
		for (uint16_t i = 0; i<strip.numPixels(); i++) {
			strip.setPixelColor(i, strip.Color(0, 0, 0, neopix_gamma[j]));
		}
		delay(wait);
		strip.show();
	}

	for (int j = 255; j >= 0; j--) {
		for (uint16_t i = 0; i<strip.numPixels(); i++) {
			strip.setPixelColor(i, strip.Color(0, 0, 0, neopix_gamma[j]));
		}
		delay(wait);
		strip.show();
	}
}

void rainbowFade2White(uint8_t wait, int rainbowLoops, int whiteLoops) {
	float fadeMax = 100.0;
	int fadeVal = 0;
	uint32_t wheelVal;
	int redVal, greenVal, blueVal;

	for (int k = 0; k < rainbowLoops; k++) {

		for (int j = 0; j<256; j++) { // 5 cycles of all colors on wheel

			for (int i = 0; i< strip.numPixels(); i++) {

				wheelVal = Wheel(((i * 256 / strip.numPixels()) + j) & 255);

				redVal = red(wheelVal) * float(fadeVal / fadeMax);
				greenVal = green(wheelVal) * float(fadeVal / fadeMax);
				blueVal = blue(wheelVal) * float(fadeVal / fadeMax);

				strip.setPixelColor(i, strip.Color(redVal, greenVal, blueVal));

			}

			//First loop, fade in!
			if (k == 0 && fadeVal < fadeMax - 1) {
				fadeVal++;
			}

			//Last loop, fade out!
			else if (k == rainbowLoops - 1 && j > 255 - fadeMax) {
				fadeVal--;
			}

			strip.show();
			delay(wait);
		}

	}



	delay(500);


	for (int k = 0; k < whiteLoops; k++) {

		for (int j = 0; j < 256; j++) {

			for (uint16_t i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i, strip.Color(0, 0, 0, neopix_gamma[j]));
			}
			strip.show();
		}

		delay(2000);
		for (int j = 255; j >= 0; j--) {

			for (uint16_t i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i, strip.Color(0, 0, 0, neopix_gamma[j]));
			}
			strip.show();
		}
	}

	delay(500);


}

void whiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength) {

	if (whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;

	int head = whiteLength - 1;
	int tail = 0;

	int loops = 3;
	int loopNum = 0;

	static unsigned long lastTime = 0;


	while (true) {
		for (int j = 0; j<256; j++) {
			for (uint16_t i = 0; i<strip.numPixels(); i++) {
				if ((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head)) {
					strip.setPixelColor(i, strip.Color(0, 0, 0, 255));
				}
				else {
					strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
				}

			}

			if (millis() - lastTime > whiteSpeed) {
				head++;
				tail++;
				if (head == strip.numPixels()) {
					loopNum++;
				}
				lastTime = millis();
			}

			if (loopNum == loops) return;

			head %= strip.numPixels();
			tail %= strip.numPixels();
			strip.show();
			delay(wait);
		}
	}

}

void fullWhite() {

	for (uint16_t i = 0; i<strip.numPixels(); i++) {
		strip.setPixelColor(i, strip.Color(0, 0, 0, 255));
	}
	strip.show();
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
	uint16_t i, j;

	for (j = 0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
		for (i = 0; i< strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
		}
		strip.show();
		delay(wait);
	}
}

void rainbow(uint8_t wait) {
	uint16_t i, j;

	for (j = 0; j<256; j++) {
		for (i = 0; i<strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel((i + j) & 255));
		}
		strip.show();
		delay(wait);
	}
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85) {
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3, 0);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3, 0);
	}
	WheelPos -= 170;
	return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0, 0);
}
uint8_t red(uint32_t c) {
	return (c >> 16);
}
uint8_t green(uint32_t c) {
	return (c >> 8);
}
uint8_t blue(uint32_t c) {
	return (c);
}

