cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(LEDDrive)

include(FetchContent)
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v3.1.1
)

FetchContent_MakeAvailable(cxxopts)

find_library(BCM2835_LIBRARY NAMES bcm2835)

set(SOURCE_FILES
  src/main.cpp
  src/color.cpp
  src/color.h
  src/led.cpp
  src/led.h
  src/pattern.cpp
  src/pattern.h
  src/spidevice-bcm2835.h
  src/spidevice-spidev.cpp
  src/spidevice-spidev.h
  src/spidevice.h
)
add_executable(led-drive ${SOURCE_FILES})
set_property(TARGET led-drive PROPERTY CXX_STANDARD 23)

target_include_directories(led-drive PRIVATE
  ${cxxopts_SOURCE_DIR}/include

)

target_link_libraries(led-drive ${BCM2835_LIBRARY})
