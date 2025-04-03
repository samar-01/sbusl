cmake_minimum_required(VERSION 3.15)

include(cmake/pico_sdk_import.cmake)    # File copied from $PICO_SDK_PATH/external
include(cmake/pico_extras_import.cmake) # File copied from $PICO_SDK_PATH/../pico-extras/external

project(raspi-pico-aprs-tnc VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(PICO_BOARD pico_w)
# set(PICO_PLATFORM rp2350-arm-s)
# set(PICO_PLATFORM rp2350)
# set(PICOTOOL_FORCE_FETCH_FROM_GIT true)
set(PICO_SDK_FETCH_FROM_GIT on)
pico_sdk_init()

# Setup of eleccoder's version of Philip Heron's 'ax25beacon' project

# include(FetchContent)
# FetchContent_Declare(ax25_aprs_lib
#     GIT_REPOSITORY    "https://github.com/eleccoder/ax25-aprs-lib.git" 
#     GIT_SHALLOW       ON
# )

# FetchContent_MakeAvailable(ax25_aprs_lib)

# add_library(ax25_aprs_lib::ax25beacon ALIAS ax25beacon)


# Compiler options

# Enable on demand only, since warnings from 3rd-party code will flood the console
# set(APRS_PICO_COMPILE_WARN_OPTIONS -Wundef -Wswitch-enum -Wall -Wextra -pedantic)


# ##### Configure the build of the 'aprs_pico' library #####

set(TARGET_LIB_NAME aprs_pico)

add_library(${TARGET_LIB_NAME}
	src/aprs_pico.c
	src/ax25.c
	src/ax25beacon.c
	# src/fx25_encode.c
	)

target_compile_options(${TARGET_LIB_NAME}
  PRIVATE "${APRS_PICO_COMPILE_WARN_OPTIONS}"
)

target_include_directories(${TARGET_LIB_NAME} PUBLIC
	include
	# src/Reed-Solomon
)

set_target_properties(${TARGET_LIB_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

  
target_link_libraries(${TARGET_LIB_NAME}
	# ax25_aprs_lib::ax25beacon
	pico_audio_pwm        
	pico_stdlib
)


set(RADIO_LIB radio)

add_library(${RADIO_LIB}
	src/radio.cpp
	)

target_compile_options(${RADIO_LIB}
  PRIVATE "${APRS_PICO_COMPILE_WARN_OPTIONS}"
)

target_include_directories(${RADIO_LIB} PUBLIC
	include
)

set_target_properties(${RADIO_LIB}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

  
target_link_libraries(${RADIO_LIB}
	${TARGET_LIB_NAME}
	pico_audio_pwm
	pico_stdlib
)


set(SENSE_LIB sense)

add_library(${SENSE_LIB}
	src/sense.cpp
	)

target_compile_options(${SENSE_LIB}
  PRIVATE "${APRS_PICO_COMPILE_WARN_OPTIONS}"
)

target_include_directories(${SENSE_LIB} PUBLIC
	include
)

set_target_properties(${SENSE_LIB}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)


add_subdirectory(pico-arduino-compat/libs/adafruit-unified-sensor)
add_subdirectory(pico-arduino-compat/libs/adafruit-bmp3xx-library)
add_subdirectory(pico-arduino-compat/libs/adafruit-lsm6ds)
add_subdirectory(pico-arduino-compat/libs/adafruit-sgp30-sensor)
add_subdirectory(pico-arduino-compat/libs/adafruit-ina219)
  
target_link_libraries(${SENSE_LIB}
	pico_stdlib
	pac-adafruit-bmp3xx-library
	pac-adafruit-lsm6ds
	pac-adafruit-unified-sensor
	pac-adafruit-sgp30-sensor
	pac-adafruit-ina219
)


set(SD_LIB sd)

add_library(${SD_LIB}
    src/sd.cpp
	)

target_compile_options(${SD_LIB}
  PRIVATE "${APRS_PICO_COMPILE_WARN_OPTIONS}"
)

target_include_directories(${SD_LIB} PUBLIC
    include
)

add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

set_target_properties(${SD_LIB}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	)
	
target_link_libraries(${SD_LIB}
    pico_stdlib
	FatFs_SPI
)


# ##### Configure the build of the DEMO applications #####

set(TARGET_DEMO_EXE_NAME main)

add_executable(${TARGET_DEMO_EXE_NAME}
	src/main.cpp
	src/hw_config.c
)

target_compile_options(${TARGET_DEMO_EXE_NAME}
  PRIVATE "${APRS_PICO_COMPILE_WARN_OPTIONS}"
)


pico_enable_stdio_usb(${TARGET_DEMO_EXE_NAME} 1)
pico_enable_stdio_uart(${TARGET_DEMO_EXE_NAME} 0)

target_include_directories(${TARGET_DEMO_EXE_NAME} PRIVATE
  include
)


# Create map/bin/hex file etc.
pico_add_extra_outputs(${TARGET_DEMO_EXE_NAME})

target_link_libraries(${TARGET_DEMO_EXE_NAME}
	# ${TARGET_LIB_NAME}
	# ax25_aprs_lib::ax25beacon
	# pico_audio_pwm
	# pico_stdlib
	# hardware_i2c
	hardware_spi
	radio
	sense
	sd
	pico_multicore
	# hardware_rtc
	# hardware_adc
	# hardware_flash
)


# # ##### Configure the build of the TEST application #####

# set(TARGET_TEST_EXE_NAME aprs_pico_tone_test)

# add_executable(${TARGET_TEST_EXE_NAME}
#     src/aprs_pico_tone_test.c
# )

# target_compile_options(${TARGET_TEST_EXE_NAME}
#   PRIVATE "${APRS_PICO_COMPILE_WARN_OPTIONS}"
# )

# target_include_directories(${TARGET_TEST_EXE_NAME} PRIVATE
#   include
# )

# # Create map/bin/hex file etc.
# pico_add_extra_outputs(${TARGET_TEST_EXE_NAME})

# target_link_libraries(${TARGET_TEST_EXE_NAME}
#     ${TARGET_LIB_NAME}
#     pico_audio_pwm        
#     pico_stdlib
# )
