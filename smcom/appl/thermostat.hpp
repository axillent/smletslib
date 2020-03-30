#ifndef SMCOM_APPL_THERMOSTAT_H
#define SMCOM_APPL_THERMOSTAT_H

#include "../smcom.hpp"

namespace Smcom { namespace Appl {

    typedef enum { thermostat_heat = 0, thermostat_cool = 1 } thermostat_mode;
    typedef enum { thermostat_avgcurrent_no=0, thermostat_avgcurrent_yes=1 } thermostat_algorithm;

    template <thermostat_mode mode, int16_t off_value=0, int16_t start_value=100, int16_t min_value=0, int16_t max_value=100, uint16_t intervaltomax=0, thermostat_algorithm isavgcurrent=thermostat_avgcurrent_no, uint8_t history_size=4>
    class Thermostat {
    protected:
      int16_t   value;
      int16_t	temp_history[history_size];
      int16_t	value_history[history_size];
    public:
      Thermostat() {}
      int calc(int16_t current, int16_t target);
    };

}}

#include <stdlib.h>

template <Smcom::Appl::thermostat_mode mode, int16_t off_value, int16_t start_value, int16_t min_value, int16_t max_value, uint16_t intervaltomax, Smcom::Appl::thermostat_algorithm isavgcurrent, uint8_t history_size>
int Smcom::Appl::Thermostat<mode, off_value, start_value, min_value, max_value, intervaltomax, isavgcurrent, history_size>::calc(int16_t current, int16_t target) {
	// передвижка статистики
	uint8_t i;
	for (i = history_size - 1; i; i--) temp_history[i - 1] = temp_history[i];
	temp_history[history_size - 1] = current;
	// передвижка статистики по value
	for (i = history_size - 1; i; i--) value_history[i - 1] = value_history[i];
	value_history[history_size - 1] = value;

	if ( isavgcurrent == thermostat_avgcurrent_yes ) {
		int32_t avg = 0;
		for (i = 0; i < history_size; i++) avg += temp_history[i];
		avg /= history_size;
		current = avg;
	}

	// выбор мощности
	int16_t	diff = current - target;											// разница между фактической и целевой температурами
	int16_t	k_1 = (temp_history[3] - temp_history[2]);				            // изменение за последний период
	int16_t	k_2 = (temp_history[3] - temp_history[0]) / 4;			            // изменение за 4 периода усредненно
	int16_t diff_p_t1 = (current + k_1) - target;								// разница по прогноз температуры через период
	int16_t diff_p_t2 = (current + k_2 * 2) - target;							// разница по прогноз погоды через два периода

	if( mode == thermostat_cool ) {
		diff = -diff;
		diff_p_t1 = -diff_p_t1;
		diff_p_t2 = -diff_p_t2;
	}

	int16_t value_new = value;
	int16_t value_return;
	int16_t value_max;

	if (value_new == off_value && diff < -100) {
		// большая текущая разница (10 градусов) температур требует полной мощности
		value_new = max_value;
	} else if (value_new == off_value && diff < -50) {
		value_new = min_value + (max_value - min_value) / 2;
	} else if (diff > 50) {
		// температура достаточно высокая/низкая
		value_new = off_value;
	} else if (abs(diff) > 5) {
		value_new -= diff / 4 + diff_p_t1 / 4 + diff_p_t2 / 4;
	}

	// limit max if needed
    value_max = max_value;
    if ( intervaltomax ) {
      uint16_t intmax = intervaltomax;
      if ( diff < 0 && -diff < intmax ) {
		value_max = max_value - (intmax + diff) * (max_value - min_value) / intmax;
      }
	}

	// apply hard limits
	if (value_new > value_max) value_new = value_max;
	if (value_new < off_value) value_new = off_value;

	// retruning value
	if (value_new > off_value) {
		if (value == off_value) value_return = start_value;
		else value_return = (value_new < min_value) ? min_value : value_new;
	} else value_return = value_new;

	value = value_new;

	return value_return;
}


#endif
