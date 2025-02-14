#pragma once
// #include <Wire.h>
#include <string>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_LSM6DSO32.h>
#include <Adafruit_SGP30.h>
#include <vector>

extern Adafruit_BMP3XX bmp;
extern Adafruit_LSM6DSOX sox;
extern Adafruit_SGP30 sgp;

void airinit();
void accinit();
void altinit();

void testair();
void testalt();
void testacc();

uint32_t getAbsoluteHumidity(float temperature, float humidity);


float getPressure();
float getAlt();
extern float seaLevel;
std::vector<float> getAccArr();
float norm(std::vector<float> v);
float getG();
std::string getAcc();
float getTempBMP();