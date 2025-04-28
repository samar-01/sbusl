#include "sd.h"
// #include "pico/stdlib.h"
#include <format>
#include "stdio.h"

std::string tostr(int x) {
	// fmt::format
	return std::format("{}", x);
}


void sdtest() {
	FRESULT fr;
	FATFS fs;
	FIL fil;
	int ret;
	char buf[100];
	char filename[] = "res.txt";

	// // Initialize chosen serial port

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
	printf("inited\n");

	// Mount drive
	fr = f_mount(&fs, "0:", 1);
	if (fr != FR_OK) {
		printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
		while (true);
	}
	printf("mounted\n");

	// // Open file for writing ()
	// fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
	// if (fr != FR_OK) {
	// 	printf("ERROR: Could not open file (%d)\r\n", fr);
	// 	while (true);
	// }
	// printf("opened\n");


	// // Write something to file
	// ret = f_printf(&fil, "This is another test\r\n");
	// if (ret < 0) {
	// 	printf("ERROR: Could not write to file (%d)\r\n", ret);
	// 	f_close(&fil);
	// 	while (true);
	// }
	// ret = f_printf(&fil, "of writing to an SD card.\r\n");
	// if (ret < 0) {
	// 	printf("ERROR: Could not write to file (%d)\r\n", ret);
	// 	f_close(&fil);
	// 	while (true);
	// }
	// printf("wrote\n");

	// // Close file
	// fr = f_close(&fil);
	// if (fr != FR_OK) {
	// 	printf("ERROR: Could not close file (%d)\r\n", fr);
	// 	while (true);
	// }
	// printf("closed\n");

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
	printf("closed");

	// Unmount drive
	// f_unmount("0:");

	// Loop forever doing nothing
	// while (true) {
	// 	sleep_ms(1000);
	// }
}
// }

namespace sd1 {
	FRESULT fr;
	FATFS fs;
	FIL fil;
	int ret;
	char buf[100];
	char filename[] = "res.txt";
	int lasti = -1;
	void setup() {
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

	}
	void open() {
		// Open file for writing ()
		fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
		if (fr != FR_OK) {
			printf("ERROR: Could not open file (%d)\r\n", fr);
			// while (true);
		}
		// printf("opened\n");
	}

	void open(int i) {
		// Open file for writing ()
		if (i == lasti) {
			return;
		} else {
			if (lasti != -1) {
				sd1::finish();
			}
			lasti = i;

			std::string s = "a";
			s = s.append(std::to_string(i)).append(".txt");
			char* x = (char*)s.c_str();
			// printf(x);
			fr = f_open(&fil, x, FA_WRITE | FA_CREATE_ALWAYS);
			if (fr != FR_OK) {
				printf("ERROR: Could not open file (open) (%d)\r\n", fr);
				// while (true);
			}
			// printf("opened\n");
		}
	}
	void write1(std::string a) {
		ret = f_printf(&fil, (char*)a.append("\n").c_str());
		if (ret < 0) {
			printf("ERROR: Could not write to file (%d)\r\n", ret);
			f_close(&fil);
			// while (true);
		}
	}
	void finish() {
		// Close file
		fr = f_close(&fil);
		if (fr != FR_OK) {
			printf("ERROR: Could not close file (finish) (%d)\r\n", fr);
			// while (true);
		}
		// printf("closed");
	}
}
