#pragma once
// #include <Wire.h>
#include <string>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_LSM6DSO32.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_INA219.h>
#include <vector>

std::string tostr(float x);

extern Adafruit_BMP3XX bmp;
extern Adafruit_LSM6DSOX sox;
extern Adafruit_SGP30 sgp;
extern Adafruit_INA219 ina219;
void curinit();
float getcur();
float getv();

void airinit();
void acc0init();
void acc1init();
void altinit();

void testair();
void testalt();
void test0acc();
void testacc1();

uint16_t getCO2();
uint32_t getAbsoluteHumidity(float temperature, float humidity);


float getPressure();
float getAlt();
extern float seaLevel;

float norm(std::vector<float> v);

std::vector<float> getAccArr0();
float getG0();
std::string getAcc0();

std::vector<float> getAccArr1();
float getG1();
std::string getAcc1();

float getTempBMP();