# NASA SL Challenge - Model Rocket Data Collection System

This project implements a data collection system for model rockets using a Raspberry Pi Pico, encoding telemetry data into audio signals transmitted via the AX.25/APRS protocol. The system is designed to work with affordable amateur radio equipment and can be easily deployed within the constraints of a model rocket.

## Features

- **Data Encoding and Decoding**: Telemetry data is encoded into audio signals using the APRS protocol and decoded by a compatible receiver.
- **Raspberry Pi Pico**: Utilizes the Pico's capabilities for processing and communication while keeping the system compact and cost-effective.
- **Low-Cost, Lightweight Solution**: Designed to be as inexpensive as possible while maintaining reliability, suitable for space-limited applications like model rockets.

## Hardware

- **Raspberry Pi Pico**: A low-cost microcontroller used for data processing and APRS encoding.
- **Amateur Radio Transceiver**: For transmitting and receiving the encoded APRS signals (ex: Baofeng UV-5R).
- **Audio Cable**: To connect the transmitter to the Pico and the receiver to the decoder.
- **Antennas**: Designed for receiving weak signals in various environments.
- **APRS Decoder**: For converting the received signal to readable data (ex: Direwolf)
- **Power Supply**: Adequate power supply for the Raspberry Pi Pico and radio equipment (ex: batteries).

## Software

- **C** and **C++**: Programming languages used for writing and uploading the firmware to the Raspberry Pi Pico.
- **AX.25 Protocol Libraries**: Libraries for encoding and decoding AX.25 signals.
- **Radio Receiver Software**: Compatible software for decoding the AX.25 protocol from the radio receiver (ex: soundcard-based decoders or SDR software).
