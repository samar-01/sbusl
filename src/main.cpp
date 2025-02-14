// #define AIR
// #define ACC
// #define ALT
#define RADIO

#include <string>
#include <stdio.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

#include "pico/stdlib.h"
// #include "hardware/flash.h" // for the flash erasing and writing
// #include "hardware/sync.h" // for the interrupts
// #define FLASH_TARGET_OFFSET (512 * 1024) // choosing to start at 512K

#ifdef RADIO
#include "radio.h"
#endif

#include "sense.h"

std::string data;
bool ready = false;
bool flying = false;
uint32_t flightStart;
bool landed = false;
float alt1 = 200; // TODO change
float alt2 = 100;

float minP = 0;
float maxAlt = 0;
float maxG = 0;

void core2();

template <typename T>
T max(T x, T y) {
	return (x > y) ? x : y;
}
template <typename T>
T min(T x, T y) {
	return (x > y) ? y : x;
}


template <typename T>
void setMin(T x, T current) {
	if (x < current) {
		current = x;
	}
}

template <typename T>
void setMax(T x, T current) {
	if (x > current) {
		current = x;
	}
}


int main() {
	stdio_init_all();

	// sleep_ms(5000);

	// char datetime_buf[256];
	// char* datetime_str = &datetime_buf[0];
	// // Start on Friday 5th of June 2020 15:45:00
	// datetime_t t = {
	// 		.year = 2000,
	// 		.month = 0,
	// 		.day = 0,
	// 		.dotw = 0,
	// 		.hour = 0,
	// 		.min = 0,
	// 		.sec = 0
	// };
	// rtc_init();
	// rtc_set_datetime(&t);
	Serial.println("startup");

	#if PTT_ENABLE == true
	gpio_init(PTT_GPXX_PIN);
	gpio_set_dir(PTT_GPXX_PIN, GPIO_OUT);
	#endif // PTT_ENABLE

	#ifdef RADIO
	radioinit();
	#endif

	#ifdef AIR
	airinit();
	#endif

	#ifdef ACC
	accinit();
	#endif

	#ifdef ALT
	altinit();
	sleep_ms(100);
	testalt();
	sleep_ms(100);
	seaLevel = getPressure();
	minP = seaLevel;
	#endif

	multicore_launch_core1(core2);
	while (true) {
		#ifdef AIR
		// testair();
		#endif

		#ifdef ACC
		// testacc();
		#endif

		#ifdef ALT
		// testalt();
		#endif

		// rtc_get_datetime(&t);
		// datetime_to_str(datetime_str, sizeof(datetime_buf), &t);


		#ifdef ACC
		#ifdef ALT
		float p = getPressure();
		float alt = getAlt();
		float temp = getTempBMP();

		std::vector<float> acc = getAccArr();
		float g = norm(acc) / 9.8;

		// setMax(p,seaLevel);
		// setMin(p,minP);
		// setMax(alt,maxAlt);
		// setMax(g,maxG); // TODO get working

		if (max(p, seaLevel) == p) {
			seaLevel = p;
		}
		if (min(p, minP) == p) {
			minP = p;
		}
		if (max(alt, maxAlt) == alt) {
			maxAlt = alt;
		}
		if (max(g, maxG) == g) {
			maxG = g;
		}
		if (alt > alt1 && !flying) {
			Serial.println();
			flying = true;
			flightStart = millis();
		} else if ((flying && alt < alt2) || millis() >= 1000 * 60 * 10) {
			landed = true;
		}

		ready = false;
		data = "";
		// data = data.append(datetime_str);
		// data = data.append(std::to_string(millis()));
		data = data.append(" P:");
		data = data.append(std::to_string(p));
		data = data.append(",");
		data = data.append(std::to_string(minP));
		data = data.append(",");
		data = data.append(std::to_string(seaLevel));
		data = data.append(" A:");
		data = data.append(std::to_string(alt));
		data = data.append(",");
		data = data.append(std::to_string(maxAlt));
		// data = data.append(" ");
		data = data.append(getAcc());
		data = data.append(" G:");
		data = data.append(std::to_string(g));
		data = data.append(",");
		data = data.append(std::to_string(maxG));
		data = data.append(" T:");
		data = data.append(std::to_string(temp));
		char* x = (char*)data.c_str();

		#endif
		#endif
		// if (landed) {
		ready = true;
		// }
		// Serial.println(x);

		sleep_ms(100);
		// Serial.println("\nloop");
	}

	return 0;
}

// void send(){
// 	std::string data1 = std::to_string(millis());
// 	data1 = data1.append(data);
// 	char* x = (char*)data1.c_str();
// 	sendData(x);
// }

int incomingByte=0;
void core2() {
	// sleep_ms(1000);
	// sendData("Bootup complete");
	// sendData("Bootup complete");
	// sendData("Bootup complete");
	// send();
	// send();
	// send();

	// while (true) {
	// 	std::string data1 = std::to_string(millis());
	// 	if (millis() > 10 * 1000 && !landed) {
	// 		continue;
	// 	}
	// 	if (!ready) {
	// 		continue;
	// 	}
	// 	// for (int i = 0; i < 3; i++) {
	// 	data1 = data1.append(data);
	// 	char* x = (char*)data1.c_str();
	// 	Serial.println(x);
	// 	sendData(x);
	// }
	while (true) {
		// std::string data1 = std::to_string(millis());
		// data1 = data1.append(" Test");
		// char* x = (char*)data1.c_str();
		// Serial.println(x);
		// sendData(x);

		  if (Serial.available() > 0) {
			// char x[100] = "";

			arduino::String data1 = Serial.readString();
			// while (Serial.available() > 0){
			// 	// Serial.read();
			// 	data1.append(std::to_string((char)Serial.read()));
			// }
			char* x = (char*)data1.c_str();
			
			Serial.println(x);
			sendData(x);
			// // read the incoming byte:
			// incomingByte = Serial.read();

			// // prints the received data
			// Serial.print("I received: ");
			// Serial.println((char)incomingByte);
		}
	}
}
