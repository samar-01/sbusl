// #define AIR
#define ACC
#define ACC2
#define ALT
#define CUR
#define RADIO
#define SD
// #define DEBUG

#include <string>
#include <stdio.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
// #include "pico/malloc.h"
#include <malloc.h>
// #include "hardware/rtc.h"
// #include "pico/util/datetime.h"
#include <vector>
#include "hardware/pwm.h"
// #include "pico/mutex.h"
// mutex mtx;

auto_init_mutex(my_mutex);

// #include "hardware/flash.h" // for the flash erasing and writing
// #include "hardware/sync.h" // for the interrupts
// #define FLASH_TARGET_OFFSET (512 * 1024) // choosing to start at 512K

#ifdef RADIO
#include "radio.h"
#endif

#include "sense.h"

#ifdef SD
#include "sd.h"
#endif

std::string data;
std::string sdata;
std::string datareal;
bool ready = false;
bool flying = false;
uint32_t flightStart = 0;
uint32_t landtime0 = 0;
uint32_t landtime = 0;
bool landed = false;
float alt1 = 200; // TODO change
float alt2 = 100;

float minP = 0;
float maxAlt = 0;
uint32_t alttime;
float lastAlt = 0;
float speed = 0;
float minspeed = 0;
float maxspeed = 0;
float landspeed = 0;
float maxG0 = 0;
float maxG1 = 0;
// float co2 = 0;
float charge = 0;

// #ifdef ACC2
std::vector<uint32_t> gtimes;
float mintg = 5;
float maxtg = 30;
float tgsplit = 20;
bool survived = true;
char realdatafr[215];
// #endif

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
void pwm_init_pin(uint8_t pin) {
	gpio_set_function(pin, GPIO_FUNC_PWM);
	uint slice_num = pwm_gpio_to_slice_num(pin);
	pwm_config config = pwm_get_default_config();
	pwm_config_set_clkdiv(&config, 4.f);
	pwm_init(slice_num, &config, true);
}

void error() {
	while (1) {
		pwm_set_gpio_level(BUZZPIN, 255 * 128);
		sleep_ms(100);
		pwm_set_gpio_level(BUZZPIN, 0);
		sleep_ms(100);
	}
}

std::string errorstring;
void senderror() {
	while (1) {
		sendData((char*)errorstring.c_str());
	}
}

void error(std::string s) {
	errorstring = s;
	auto S = (char*)s.c_str();
	multicore_launch_core1(senderror);
	while (1) {
		pwm_set_gpio_level(BUZZPIN, 255 * 128);
		sleep_ms(100);
		pwm_set_gpio_level(BUZZPIN, 0);
		sleep_ms(100);
		Serial.println(S);
	}
}

void mem(){
	struct mallinfo mi = mallinfo();
	printf("aloc %d\n", mi.uordblks);
	printf("free %d\n", mi.fordblks);
}

int main() {
	// sd();
	// while (1) {}
	stdio_init_all();
	pwm_set_gpio_level(BUZZPIN, 0);
	pwm_init_pin(LEDPIN);
	// pwm_set_gpio_level(LEDPIN, 255 * 10);
	pwm_init_pin(BUZZPIN);
	// pwm_set_gpio_level(BUZZPIN, 255 * 128);
	// sleep_ms(100);
	// pwm_set_gpio_level(BUZZPIN, 255 * 0);
	unsigned long a = millis();


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
	float cur = getcur();
	float volt = getv();
	if (cur <= 0 || volt <= 0) { //idk about this one
		// sendData("Current Fail");
		// error();
	}
	#endif

	#ifdef AIR
	testair();
	#endif

	#ifdef ACC
	testacc0();
	std::vector<float> acc0 = getAccArr0();
	float g0 = norm(acc0) / 9.8;
	if (g0 < 0.5 || g0 > 1.5) {
		sendData("ACC 1 Fail");
		// error();
		error("ACC 1 Fail");
	}
	for (int i = 0; i < tgsplit; i++) {
		gtimes.push_back(0);
	}
	#endif

	#ifdef ACC2
	testacc1();
	std::vector<float> acc1 = getAccArr1();
	float g1 = norm(acc1) / 9.8;
	if (g1 < 0.5 || g1 > 1.5) {
		sendData("ACC 2 Fail");
		error("ACC 2 Fail");
	}
	#endif

	#ifdef ALT
	testalt();
	float px = getPressure();
	if (px > 1500 || px < 500) {
		sendData("Alt Fail");
		error("Alt Fail");
	}
	#endif

	// sdtest();
	sd1::setup();
	pwm_set_gpio_level(LEDPIN, 255 * 50);
	pwm_set_gpio_level(BUZZPIN, 255 * 128);
	sleep_ms(300);
	pwm_set_gpio_level(BUZZPIN, 0);
	multicore_launch_core1(core2);
	int i = 0;
	while (true) {
		i++;
		a = millis();
		// rtc_get_datetime(&t);
		// datetime_to_str(datetime_str, sizeof(datetime_buf), &t);


		#ifdef ALT
		float p = getPressure();
		float alt = getAlt();
		if (alttime != 0) {
			speed = 1000 * (alt - lastAlt) / (millis() - alttime);

			if (max(speed, maxspeed) == speed) {
				maxspeed = speed;
			}
			if (min(speed, minspeed) == speed) {
				minspeed = speed;
			}
		}
		alttime = millis();
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
			// flightStart = millis();
		} else if ((flying && alt < alt2) || millis() >= 1000 * 60 * 10) {
			landed = true;
			landtime0 = millis();
		}
		lastAlt = alt;
		#endif

		#ifdef CUR
		float cur = getcur();
		float volt = getv();
		#endif

		#ifdef ACC
		std::vector<float> acc0 = getAccArr0();
		uint32_t acctime = millis();

		float g0 = norm(acc0) / 9.8;

		if (max(g0, maxG0) == g0) {
			maxG0 = g0;
		}
		if (!flying && g0 > 2 && flightStart == 0) {
			flightStart = acctime;
		}
		if (landed && g0 > 2 && landtime == 0) {
			landtime = acctime;
			#ifdef ALT
			landspeed = speed;
			#endif
		}

		if (!flying && flightStart != 0 && acctime - flightStart > 10000) {
			flightStart = 0;
		}


		#endif
		#ifdef ACC2
		std::vector<float> acc1 = getAccArr1();
		float g1 = norm(acc1) / 9.8;
		if (max(g1, maxG1) == g1) {
			maxG1 = g1;
		}

		for (int i = 0; i < tgsplit; i++) {
			// gtimes.push_back(0);
			float gx = (maxtg - mintg) / (tgsplit - 1) * i + mintg;
			if (g1 > gx) {
				if (gtimes.at(i) == 0) {
					gtimes.at(i) = acctime;
				}
				float t = acctime - gtimes.at(i);
				if (t / 1000 > 32 * pow(gx, -0.261)) {
					survived = false;
				}
			} else {
				gtimes.at(i) = 0;
			}
		}
		#endif

		#ifdef AIR
		// testair();
		co2 = getCO2();
		#endif


		ready = false;
		data = "";
		// sdata = "";
		// sdata = sdata.append(std::to_string(millis()));
		// sdata = sdata.append(",");
		data = data.append(std::to_string((landtime - flightStart) / 1000));
		// data = data.append(std::to_string(110));
		// data = data.append(datetime_str);
		// data = data.append(std::to_string(millis()));
		#ifdef ALT
		// sdata = sdata.append(" P:");
		// sdata = sdata.append(tostr(p));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(minP));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(seaLevel));

		// data = data.append(" A:");
		data = data.append(",");
		data = data.append(tostr(alt));
		data = data.append(",");
		data = data.append(tostr(maxAlt));
		data = data.append(",");
		// data = data.append(" V:");
		// data = data.append(tostr(speed));
		// data = data.append(",");
		data = data.append(tostr(landspeed));
		data = data.append(",");
		// data = data.append(tostr(minspeed));
		// data = data.append(",");
		data = data.append(tostr(maxspeed));
		data = data.append(",");

		// sdata = sdata.append(" A:");
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr((int)alt));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(maxAlt));
		// sdata = sdata.append(",");
		// sdata = sdata.append(" V:");
		// sdata = sdata.append(tostr(speed));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(landspeed));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(minspeed));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(maxspeed));
		// sdata = sdata.append(",");
		#endif
		// data = data.append(" ");
		#ifdef ACC
		// data = data.append(getAcc0());
		// data = data.append(" G0:");
		// data = data.append(",");
		data = data.append(tostr(g0));
		data = data.append(",");
		data = data.append(tostr(maxG0));
		// data = data.append(" S:");
		data = data.append(",");

		// sdata = sdata.append(" G0:");
		// sdata = sdata.append(getAccString(acc0));
		// // data = data.append(" DG0:");
		// // data = data.append(getAccString(acc0));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(g0));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(maxG0));
		// sdata = sdata.append(",");


		#endif
		#ifdef ACC2
		// data = data.append(getAcc1());
		// data = data.append(" G1:");
		data = data.append(tostr(g1));
		data = data.append(",");
		data = data.append(tostr(maxG1));
		data = data.append(",");

		// std::string survive = "";
		if (survived) {
			// survive.append("T");
			data = data.append("T");
		} else {
			data = data.append("F");
			// survive.append("F");
		}
		data = data.append(",");


		// sdata = sdata.append(" G1:");
		// std::to_string(acc0);
		// Serial.println(acc1);

		// for (float x: acc1){
		// 	// sdata = sdata.append(tostr(x));
		// 	// sdata = sdata.append(",");
		// float xyz0 = acc1[0];
		// float xyz1 = acc1[1];
		// float xyz2 = acc1[2];
		
		// Serial.println(acc1[0]);
		// Serial.println(acc1[1]);
		// Serial.println(acc1[2]);
		// }
		// sdata = sdata.append(getAccString(acc1));
		// sdata = sdata.append(tostr(xyz0));

		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(g1));
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(maxG1));
		// sdata = sdata.append(",");
		// if (survived) {
		// 	sdata = sdata.append("T");
		// } else {
		// 	sdata = sdata.append("F");
		// }
		// sdata = sdata.append(",");
		//12952	12832
		//3312	3432


		#endif
		#ifdef ALT
		// data = data.append(" T:");
		data = data.append(tostr((int)temp));
		data = data.append(",");
		// sdata = sdata.append(tostr((int)temp));
		// sdata = sdata.append(",");
		#endif

		#ifdef AIR
		// testair();
		// data = data.append(" C:");
		data = data.append(",");
		data = data.append(tostr(co2));
		sdata = sdata.append(",");
		sdata = sdata.append(tostr(co2));
		#endif

		#ifdef CUR
		charge -= (millis() - a) / 1000 * cur;
		// data = data.append(" I:");
		data = data.append(tostr(cur));
		// data = data.append(" C:");
		data = data.append(",");
		data = data.append(tostr(charge / (3600)));
		// data = data.append(" V:");
		data = data.append(",");
		data = data.append(tostr(volt));


		// // data = data.append(" I:");
		// sdata = sdata.append(tostr(cur));
		// // data = data.append(" C:");
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(charge / (3600)));
		// // data = data.append(" V:");
		// sdata = sdata.append(",");
		// sdata = sdata.append(tostr(volt));
		#endif

		// data = data.append("!");
		char* x = (char*)data.c_str();
		// Serial.println(x);
		// Serial.println(x);
		// x[strlen(x)] = 0;

		x[210] = 0;
		// for (int asf = 0; asf < 215; asf++) {
		// 	// realdatafr[asf] = x[asf];
		// 	if (x[asf] == '!') {
		// 		x[asf] = 0;
		// 		break;
		// 	}
		// }


		uint32_t owner_out;
		if (mutex_try_enter(&my_mutex, &owner_out)) {
			// data1 = data1.append(data);
			strcpy(realdatafr, x);
			mutex_exit(&my_mutex);
			// sendData(x);
		} else {
			#ifdef DEBUG
			Serial.println("bruh1");
			#endif
			// sleep_ms(10);
		}

		// if (landed) {
		ready = true;
		// }
		#ifdef DEBUG
		if (flightStart != 0) {
			Serial.print(millis() - flightStart);
		}
		#endif
		// Serial.println(x);
		// mtx.lock();

		// uint32_t owner_out;
		// if (mutex_try_enter(&my_mutex, &owner_out)) {
		// 	datareal = data;
		// 	mutex_exit(&my_mutex);
		// } else {
		// 	// sleep_ms(10);
		// }
		// sleep_ms(500);
		sd1::open(i / 10);
		// sd1::open();
		// if (landed) {
		// 	sd1::write1(std::to_string(millis() - flightStart).append(" ").append(std::to_string(0)).append(data));
		// } else {
		// 	sd1::write1(std::to_string(millis() - flightStart).append(" ").append(std::to_string(landtime - flightStart)).append(data));
		// }

		sdata = data.append(",");
		sdata = sdata.append(std::to_string(millis()));
		sdata = sdata.append(",");
		sdata = sdata.append(tostr(p));
		sdata = sdata.append(",");
		sdata = sdata.append(tostr(minP));
		sdata = sdata.append(",");
		sdata = sdata.append(tostr(seaLevel));
		sdata = sdata.append(",");
		sdata = sdata.append(tostr(speed));
		sdata = sdata.append(",");
		sdata = sdata.append(tostr(minspeed));
		sdata = sdata.append(",");
		sdata = sdata.append(tostr(maxspeed));
		sdata = sdata.append(",");
		// sdata = sdata.append(" G0:");
		sdata = sdata.append(getAccString(acc0));
		sdata = sdata.append(",");
		// sdata = sdata.append(tostr(g0));
		// $data.$i$2:$i$1141
		
		sdata = sdata.append(",");
		// sdata = sdata.append(" G1:");
		sdata = sdata.append(getAccString(acc1));
		sdata = sdata.append(",");
		// sdata = sdata.append(tostr(g1));
		// sdata = sdata.append(",");
		// if (survived) {
		// 	sdata = sdata.append("T");
		// } else {
		// 	sdata = sdata.append("F");
		// }
		// sdata = sdata.append(",");

		#ifdef DEBUG
		// Serial.println((char*)sdata.c_str());
		mem();
		#endif
		Serial.println((char*)sdata.c_str());
		sd1::write1(sdata);
		// Serial.println((char*)data.c_str());
		// sd1::finish();

		#ifdef DEBUG
		Serial.println(millis() - a);
		#endif

		// Serial.println("\nloop");
	}
	sd1::finish();

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

void core2() {
	// sendData("Bootup complete");

	while (true) {
		// Serial.println("asf");
		// std::string data1 = std::to_string(millis());

		// if (millis() > 10 * 1000 && !landed && !flying) {
		if (millis() > 10 * 1000) {
			if (landtime0 == 0 || landtime0 < millis() + 20 * 1000) {
				// sleep_ms(100);
				continue;
			}
		}
		// if (!ready) {
		// 	continue;
		// }
		// for (int i = 0; i < 3; i++) {

		// mtx.lock();

		// char [x = realdatafr;
		// Serial.println((char*)realdatafr);
		// sendData((char*)realdatafr);
		uint32_t owner_out;
		char x[215];
		x[214] = 0;
		if (mutex_try_enter(&my_mutex, &owner_out)) {
			// data1 = data1.append(data);
			strcpy(x, realdatafr);
			mutex_exit(&my_mutex);
			x[214] = 1;

		} else {
			#ifdef DEBUG
			Serial.println("bruh");
			#endif
			continue;
		}
		if (x[214] != 0) {
			std::string sx(x);
			char* x1 = (char*)sx.c_str();

			#ifdef DEBUG
			Serial.println(x1);
			#endif

			// char xsend[215];
			// memcpy(xsend, x1, 215);
			sendData(x1);
			// TODO
		}
		// mtx.unlock();
		sleep_ms(100);
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
