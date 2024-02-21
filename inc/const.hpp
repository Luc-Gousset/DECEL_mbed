#ifndef __CONST_HPP__
#define __CONST_HPP__

#include <chrono>
#include <mutex>
using namespace std::literals::chrono_literals;

constexpr int DEFAULT_BAUD = 57600;

constexpr auto GPIO_POOLING_MS = std::chrono::milliseconds(100ms);
constexpr auto ANALOG_IN_POOLING_MS = std::chrono::milliseconds(1000ms);


constexpr uint8_t SOM = 0xab;
constexpr uint8_t EOM = 0xba;

typedef enum {
    DECEL_PIN_OUTPUT = 0,
    DECEL_PIN_INPUT = 1,
    DECEL_PIN_DISABLE = 2,
} PinDirection_DECEL;


constexpr uint8_t GPIO_SIZE = 16;
constexpr uint8_t ANALOG_IN_SIZE = 4;



#endif