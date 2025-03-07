#include "sense.h"
// #include <string>

float seaLevel = 1007;

Adafruit_BMP3XX bmp;
Adafruit_LSM6DSOX sox;
Adafruit_SGP30 sgp;

void airinit() {
	if (!sgp.begin()) {
		Serial.println("SGP Sensor not found :(");
		// while (1); // TODO remove
	}
	Serial.print("Found SGP30 serial #");
	Serial.print(sgp.serialnumber[0], HEX);
	Serial.print(sgp.serialnumber[1], HEX);
	Serial.println(sgp.serialnumber[2], HEX);
}

void testair() {
	if (!sgp.IAQmeasure()) {
		Serial.println("SGP Measurement failed");
		// return;
	}
	Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
	Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

	if (!sgp.IAQmeasureRaw()) {
		Serial.println("Raw Measurement failed");
		// return;
	}
	Serial.print("Raw H2 "); Serial.print(sgp.rawH2); Serial.print(" \t");
	Serial.print("Raw Ethanol "); Serial.print(sgp.rawEthanol); Serial.println("");
}

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
	// approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
	const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
	const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
	return absoluteHumidityScaled;
}


void accinit() {
	if (!sox.begin_I2C(0x6B)) {
		// while (1) {
		// 	Serial.println("acc");
		// } // TODO remove
	}

	sox.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
	Serial.print("Accelerometer range set to: ");
	switch (sox.getAccelRange()) {
		case LSM6DS_ACCEL_RANGE_2_G:
			Serial.println("+-2G");
			break;
		case LSM6DS_ACCEL_RANGE_4_G:
			Serial.println("+-4G");
			break;
		case LSM6DS_ACCEL_RANGE_8_G:
			Serial.println("+-8G");
			break;
		case LSM6DS_ACCEL_RANGE_16_G:
			Serial.println("+-16G");
			break;
	}

	sox.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
	Serial.print("Accelerometer data rate set to: ");
	switch (sox.getAccelDataRate()) {
		case LSM6DS_RATE_SHUTDOWN:
			Serial.println("0 Hz");
			break;
		case LSM6DS_RATE_12_5_HZ:
			Serial.println("12.5 Hz");
			break;
		case LSM6DS_RATE_26_HZ:
			Serial.println("26 Hz");
			break;
		case LSM6DS_RATE_52_HZ:
			Serial.println("52 Hz");
			break;
		case LSM6DS_RATE_104_HZ:
			Serial.println("104 Hz");
			break;
		case LSM6DS_RATE_208_HZ:
			Serial.println("208 Hz");
			break;
		case LSM6DS_RATE_416_HZ:
			Serial.println("416 Hz");
			break;
		case LSM6DS_RATE_833_HZ:
			Serial.println("833 Hz");
			break;
		case LSM6DS_RATE_1_66K_HZ:
			Serial.println("1.66 KHz");
			break;
		case LSM6DS_RATE_3_33K_HZ:
			Serial.println("3.33 KHz");
			break;
		case LSM6DS_RATE_6_66K_HZ:
			Serial.println("6.66 KHz");
			break;
	}

	sox.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
	Serial.print("Gyro data rate set to: ");
	switch (sox.getGyroDataRate()) {
		case LSM6DS_RATE_SHUTDOWN:
			Serial.println("0 Hz");
			break;
		case LSM6DS_RATE_12_5_HZ:
			Serial.println("12.5 Hz");
			break;
		case LSM6DS_RATE_26_HZ:
			Serial.println("26 Hz");
			break;
		case LSM6DS_RATE_52_HZ:
			Serial.println("52 Hz");
			break;
		case LSM6DS_RATE_104_HZ:
			Serial.println("104 Hz");
			break;
		case LSM6DS_RATE_208_HZ:
			Serial.println("208 Hz");
			break;
		case LSM6DS_RATE_416_HZ:
			Serial.println("416 Hz");
			break;
		case LSM6DS_RATE_833_HZ:
			Serial.println("833 Hz");
			break;
		case LSM6DS_RATE_1_66K_HZ:
			Serial.println("1.66 KHz");
			break;
		case LSM6DS_RATE_3_33K_HZ:
			Serial.println("3.33 KHz");
			break;
		case LSM6DS_RATE_6_66K_HZ:
			Serial.println("6.66 KHz");
			break;
	}
}

void testacc() {
	sensors_event_t accel;
	sensors_event_t gyro;
	sensors_event_t temp;
	sox.getEvent(&accel, &gyro, &temp);

	Serial.print("\t\tTemperature ");
	Serial.print(temp.temperature);
	Serial.println(" deg C");

	/* Display the results (acceleration is measured in m/s^2) */
	Serial.print("\t\tAccel X: ");
	Serial.print(accel.acceleration.x);
	Serial.print(" \tY: ");
	Serial.print(accel.acceleration.y);
	Serial.print(" \tZ: ");
	Serial.print(accel.acceleration.z);
	Serial.println(" m/s^2 ");

	/* Display the results (rotation is measured in rad/s) */
	Serial.print("\t\tGyro X: ");
	Serial.print(gyro.gyro.x);
	Serial.print(" \tY: ");
	Serial.print(gyro.gyro.y);
	Serial.print(" \tZ: ");
	Serial.print(gyro.gyro.z);
	Serial.println(" radians/s ");
	Serial.println();
}


void altinit() {
	if (!bmp.begin_I2C()) {
		Serial.println("Could not find a valid BMP3 sensor, check wiring!");
		// while (1); // TODO remove
	}
	bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
	bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
	bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
	bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}


void testalt() {
	if (!bmp.performReading()) {
		Serial.println("ALT Failed to perform reading :(");
		return;
	}
	Serial.print("Temperature = ");
	Serial.print(bmp.temperature);
	Serial.println(" *C");

	Serial.print("Pressure = ");
	Serial.print(bmp.pressure / 100.0);
	Serial.println(" hPa");

	Serial.print("Approx. Altitude = ");
	Serial.print(bmp.readAltitude(seaLevel));
	Serial.println(" m");

	Serial.println();
}

float getPressure() {
	if (!bmp.performReading()) {
		Serial.println("ALT Failed to perform reading :(");
		return 0;
	}
	return (bmp.pressure / 100.0);
}

float getAlt() {

	if (!bmp.performReading()) {
		Serial.println("ALT Failed to perform reading :(");
		return 0;
	}
	// return (bmp.pressure / 100.0);
	return bmp.readAltitude(seaLevel);
}

float getTempBMP() {
	if (!bmp.performReading()) {
		Serial.println("ALT Failed to perform reading :(");
		return 0;
	}
	return (bmp.temperature);
}


std::vector<float> getAccArr() {
	std::vector<float> dat;

	sensors_event_t accel;
	sensors_event_t gyro;
	sensors_event_t temp;
	sox.getEvent(&accel, &gyro, &temp);

	dat.push_back(accel.acceleration.x * 2);
	dat.push_back(accel.acceleration.y * 2);
	dat.push_back(accel.acceleration.z * 2);

	return dat;
}

float norm(std::vector<float> v) {
	float a = 0;
	for (int i = 0; i < v.size(); i++) {
		a += v[i] * v[i];
	}
	return sqrt(a);
}

float getG() {
	std::vector<float> acc = getAccArr();
	return norm(acc) / 9.8;

}

std::string getAcc() {

	sensors_event_t accel;
	sensors_event_t gyro;
	sensors_event_t temp;
	sox.getEvent(&accel, &gyro, &temp);

	std::string x = " X:";

	x.append(std::to_string(accel.acceleration.x * 2));
	x.append(" Y:");
	x.append(std::to_string(accel.acceleration.y * 2));
	x.append(" Z:");
	x.append(std::to_string(accel.acceleration.z * 2));

	return x;
}
