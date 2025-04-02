// #define AIR
#define ACC
// #define ACC2
#define ALT
#define CUR
#define RADIO

#include <string>
#include <stdio.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "hardware/pwm.h"

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
float maxG0 = 0;
float maxG1 = 0;
float co2 = 0;

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


	gpio_init(13);
	gpio_set_dir(13, GPIO_OUT);
	gpio_put(13, HIGH);

	sleep_ms(1000);

	printf("a\n");
	// Initialize SD card
	if (!sd_init_driver()) {
		printf("ERROR: Could not initialize SD card\r\n");
		while (true);
	}
	printf("inited\n");

	// Mount drive
	fr = f_mount(&fs, "0:", 1);
	if (fr != FR_OK) {
		printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
		while (true);
	}
	printf("mounted\n");

	// Open file for writing ()
	fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		printf("ERROR: Could not open file (%d)\r\n", fr);
		while (true);
	}
	printf("opened\n");


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
	printf("wrote\n");

	// Close file
	fr = f_close(&fil);
	if (fr != FR_OK) {
		printf("ERROR: Could not close file (%d)\r\n", fr);
		while (true);
	}
	printf("closed\n");

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

	gpio_init(LEDPIN);
	gpio_set_dir(LEDPIN, GPIO_OUT);
	gpio_put(LEDPIN, HIGH);
	sd();
	while (1) {}
	stdio_init_all();
	multicore_launch_core1(core2);

	sleep_ms(100);

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
	acc0init();
	#endif
	#ifdef ACC2
	acc1init();
	#endif

	#ifdef ALT
	altinit();
	sleep_ms(100);
	testalt();
	sleep_ms(100);
	seaLevel = getPressure();
	minP = seaLevel;
	#endif

	#ifdef CUR
	curinit();
	#endif

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


		#ifdef ALT
		float p = getPressure();
		float alt = getAlt();
		float temp = getTempBMP();

		if (max(p, seaLevel) == p) {
			seaLevel = p;
		}
		if (min(p, minP) == p) {
			minP = p;
		}
		if (max(alt, maxAlt) == alt) {
			maxAlt = alt;
		}
		// setMax(p,seaLevel);
		// setMin(p,minP);
		// setMax(alt,maxAlt);
		// setMax(g,maxG); // TODO get working
		if (alt > alt1 && !flying) {
			Serial.println();
			flying = true;
			flightStart = millis();
		} else if ((flying && alt < alt2) || millis() >= 1000 * 60 * 10) {
			landed = true;
		}

		#endif

		#ifdef CUR
		float cur = getcur();
		float volt = getv();
		#endif

		#ifdef ACC
		std::vector<float> acc0 = getAccArr0();
		float g0 = norm(acc0) / 9.8;

		if (max(g0, maxG0) == g0) {
			maxG0 = g0;
		}
		#endif
		#ifdef ACC2
		std::vector<float> acc1 = getAccArr1();
		float g1 = norm(acc1) / 9.8;
		if (max(g1, maxG1) == g1) {
			maxG1 = g1;
		}
		#endif

		#ifdef AIR
		// testair();
		co2 = getCO2();
		#endif


		ready = false;
		data = "";
		// data = data.append(datetime_str);
		// data = data.append(std::to_string(millis()));
		#ifdef ALT
		data = data.append(" P:");
		data = data.append(tostr(p));
		data = data.append(",");
		data = data.append(tostr(minP));
		data = data.append(",");
		data = data.append(tostr(seaLevel));
		data = data.append(" A:");
		data = data.append(tostr(alt));
		data = data.append(",");
		data = data.append(tostr(maxAlt));
		#endif
		// data = data.append(" ");
		#ifdef ACC
		data = data.append(getAcc0());
		data = data.append(" G0:");
		data = data.append(tostr(g0));
		data = data.append(",");
		data = data.append(tostr(maxG0));
		#endif
		#ifdef ACC2
		data = data.append(getAcc1());
		data = data.append(" G1:");
		data = data.append(tostr(g1));
		data = data.append(",");
		data = data.append(tostr(maxG1));
		#endif
		#ifdef ALT
		data = data.append(" T:");
		data = data.append(tostr(temp));
		#endif

		#ifdef AIR
		// testair();
		data = data.append(" C:");
		data = data.append(tostr(co2));
		#endif

		#ifdef CUR
		data = data.append(" I:");
		data = data.append(tostr(cur));
		data = data.append(" V:");
		data = data.append(tostr(volt));
		#endif

		char* x = (char*)data.c_str();

		// if (landed) {
		ready = true;
		// }
		Serial.println(x);
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
bool led = false;
void pwm_init_pin(uint8_t pin) {
	gpio_set_function(pin, GPIO_FUNC_PWM);
	uint slice_num = pwm_gpio_to_slice_num(pin);
	pwm_config config = pwm_get_default_config();
	pwm_config_set_clkdiv(&config, 4.f);
	pwm_init(slice_num, &config, true);
}


void core2() {

	gpio_init(LEDPIN);
	gpio_set_dir(LEDPIN, GPIO_OUT);
	gpio_put(LEDPIN, HIGH);

	// gpio_init(BUZZPIN);
	// gpio_set_dir(BUZZPIN, GPIO_OUT);
	// gpio_set_dir(BUZZPIN, GPIO_FUNC_PWM);
	// gpio_put(BUZZPIN, HIGH);
	pwm_init_pin(BUZZPIN);
	pwm_set_gpio_level(BUZZPIN, 255 * 128);


	// for (int i = 0; i < 255; i++) {
	// 	pwm_set_gpio_level(BUZZPIN, i*i);

	// 	sleep_ms(10);
	// 	if (i == 250){
	// 		i = 0;
	// 	}
	// }

	// uint slice_num = pwm_gpio_to_slice_num(BUZZPIN);
	// pwm_set_wrap(slice_num, 3);
	// pwm_set_chan_level(slice_num, PWM_CHAN_A, 2);
	// pwm_set_chan_level(slice_num, PWM_CHAN_B, 2);
	// pwm_set_enabled(slice_num, true);


	while (1) {
	}

	// sleep_ms(1000);
	sendData("Bootup complete");
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
		// Serial.println(x);
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
