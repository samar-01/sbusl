// #define AIR
#define ACC
#define ALT
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

#include "sd_card.h"
#include "ff.h"


#ifdef __cplusplus
extern "C" {
	#endif

	size_t sd_get_num();
	sd_card_t* sd_get_by_num(size_t num);

	size_t spi_get_num();
	spi_t* spi_get_by_num(size_t num);

	#ifdef __cplusplus
}
#endif
// extern "C" {
void sd() {


	FRESULT fr;
	FATFS fs;
	FIL fil;
	int ret;
	char buf[100];
	char filename[] = "test02.txt";

	// // Initialize chosen serial port
	stdio_init_all();

	// Wait for user to press 'enter' to continue
	// printf("\r\nSD card test. Press 'enter' to start.\r\n");
	// while (true) {
	// 	buf[0] = getchar();
	// 	if ((buf[0] == '\r') || (buf[0] == '\n')) {
	// 		break;
	// 	}
	// }

	printf("a\n");
	// Initialize SD card
	if (!sd_init_driver()) {
		printf("ERROR: Could not initialize SD card\r\n");
		while (true);
	}

	// Mount drive
	fr = f_mount(&fs, "0:", 1);
	if (fr != FR_OK) {
		printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
		while (true);
	}

	// Open file for writing ()
	fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		printf("ERROR: Could not open file (%d)\r\n", fr);
		while (true);
	}

	// Write something to file
	ret = f_printf(&fil, "This is another test\r\n");
	if (ret < 0) {
		printf("ERROR: Could not write to file (%d)\r\n", ret);
		f_close(&fil);
		while (true);
	}
	ret = f_printf(&fil, "of writing to an SD card.\r\n");
	if (ret < 0) {
		printf("ERROR: Could not write to file (%d)\r\n", ret);
		f_close(&fil);
		while (true);
	}

	// Close file
	fr = f_close(&fil);
	if (fr != FR_OK) {
		printf("ERROR: Could not close file (%d)\r\n", fr);
		while (true);
	}

	// Open file for reading
	fr = f_open(&fil, filename, FA_READ);
	if (fr != FR_OK) {
		printf("ERROR: Could not open file (%d)\r\n", fr);
		while (true);
	}

	// Print every line in file over serial
	printf("Reading from file '%s':\r\n", filename);
	printf("---\r\n");
	while (f_gets(buf, sizeof(buf), &fil)) {
		printf(buf);
	}
	printf("\r\n---\r\n");

	// Close file
	fr = f_close(&fil);
	if (fr != FR_OK) {
		printf("ERROR: Could not close file (%d)\r\n", fr);
		while (true);
	}

	// Unmount drive
	f_unmount("0:");

	// Loop forever doing nothing
	while (true) {
		sleep_ms(1000);
	}
}
// }

int main() {
	// sd();
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
		// unsigned long a = millis();
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
		data = data.append(std::to_string(millis()));
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
		// Serial.println(millis() - a);

		// sleep_ms(100);
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

int incomingByte = 0;
void core2() {
	// sleep_ms(1000);
	// sendData("Bootup complete");
	// sendData("Bootup complete");
	// sendData("Bootup complete");
	// send();
	// send();
	// send();

	while (true) {
		std::string data1 = std::to_string(millis());
		if (millis() > 10 * 1000 && !landed && !flying) {
			continue;
		}
		if (!ready) {
			continue;
		}
		// for (int i = 0; i < 3; i++) {
		data1 = data1.append(data);
		char* x = (char*)data1.c_str();
		Serial.println(x);
		sendData(x);
	}
	// while (true) {
	// 	// std::string data1 = std::to_string(millis());
	// 	// data1 = data1.append(" Test");
	// 	// char* x = (char*)data1.c_str();
	// 	// Serial.println(x);
	// 	// sendData(x);
	// 	// if (ready){
	// 	// 	char* x = (char*)data.c_str();
	// 	// 	sendData(x);
	// 	// }

	// 	//   if (Serial.available() > 0) {
	// 	// 	// char x[100] = "";

	// 	// 	arduino::String data1 = Serial.readString();
	// 	// 	// while (Serial.available() > 0){
	// 	// 	// 	// Serial.read();
	// 	// 	// 	data1.append(std::to_string((char)Serial.read()));
	// 	// 	// }
	// 	// 	char* x = (char*)data1.c_str();

	// 	// 	Serial.println(x);
	// 	// 	sendData(x);
	// 	// 	// // read the incoming byte:
	// 	// 	// incomingByte = Serial.read();

	// 	// 	// // prints the received data
	// 	// 	// Serial.print("I received: ");
	// 	// 	// Serial.println((char)incomingByte);
	// 	// }
	// }
}
