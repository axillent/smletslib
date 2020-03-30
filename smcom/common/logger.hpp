#ifndef STAVRP_COMMON_LOGGER_H
#define STAVRP_COMMON_LOGGER_H

#ifdef _STAVR_PLATFORM_STM8S
#elif defined(_STAVR_PLATFORM_STM32F0)
#elif defined(_STAVR_PLATFORM_ESP8266)
#elif defined(_STAVR_PLATFORM_LINUX)
#elif defined(_STAVR_PLATFORM_ARDUINO_ESP8266)
#endif

class LoggerNOP {
public:

};

#endif
