#include <avr/io.h>
#include <avr/pgmspace.h>

// Значение температуры, возвращаемое если сумма результатов АЦП больше первого значения таблицы
#define TEMPERATURE_UNDER 0
// Значение температуры, возвращаемое если сумма результатов АЦП меньше последнего значения таблицы
#define TEMPERATURE_OVER 1000
// Значение температуры соответствующее первому значению таблицы
#define TEMPERATURE_TABLE_START 0
// Шаг таблицы 
#define TEMPERATURE_TABLE_STEP 10

// Тип каждого элемента в таблице, если сумма выходит в пределах 16 бит - uint16_t, иначе - uint32_t
typedef uint16_t temperature_table_entry_type;
// Тип индекса таблицы. Если в таблице больше 256 элементов, то uint16_t, иначе - uint8_t
typedef uint8_t temperature_table_index_type;
// Метод доступа к элементу таблицы, должна соответствовать temperature_table_entry_type
#define TEMPERATURE_TABLE_READ(i) pgm_read_word(&termo_table[i])

/* Таблица суммарного значения АЦП в зависимости от температуры. От большего значения к меньшему
   Для построения таблицы использованы следующие парамертры:
     R1(T1): 100кОм(25°С)
     B25/50: 3950
     Схема включения: A
     Ra: 100кОм
     Напряжения U0/Uref: 5В/5В
*/
const temperature_table_entry_type termo_table[] PROGMEM = {
    50512, 49892, 49260, 48616, 47960, 47293, 46616, 45929,
    45233, 44529, 43817, 43098, 42374, 41644, 40909, 40171,
    39430, 38687, 37943, 37199, 36455, 35712, 34971, 34233,
    33498, 32768, 32043, 31323, 30609, 29902, 29202, 28510,
    27827, 27152, 26486, 25831, 25185, 24549, 23924, 23309,
    22706, 22114, 21533, 20964, 20406, 19860, 19326, 18803,
    18293, 17793, 17306, 16830, 16365, 15912, 15471, 15040,
    14620, 14212, 13814, 13426, 13049, 12682, 12325, 11978,
    11641, 11313, 10994, 10684, 10384, 10091, 9807, 9532,
    9264, 9004, 8752, 8507, 8269, 8039, 7815, 7598,
    7387, 7182, 6984, 6791, 6605, 6424, 6248, 6077,
    5912, 5752, 5596, 5445, 5298, 5156, 5018, 4885,
    4755, 4629, 4507, 4388, 4273
};

// Функция вычисляет значение температуры в десятых долях градусов Цельсия
// в зависимости от суммарного значения АЦП.
int16_t calc_temperature(temperature_table_entry_type adcsum) {
  temperature_table_index_type l = 0;
  temperature_table_index_type r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
  temperature_table_entry_type thigh = TEMPERATURE_TABLE_READ(r);
  
  // Проверка выхода за пределы и граничных значений
  if (adcsum <= thigh) {
    #ifdef TEMPERATURE_UNDER
      if (adcsum < thigh) 
        return TEMPERATURE_UNDER;
    #endif
    return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type tlow = TEMPERATURE_TABLE_READ(0);
  if (adcsum >= tlow) {
    #ifdef TEMPERATURE_OVER
      if (adcsum > tlow)
        return TEMPERATURE_OVER;
    #endif
    return TEMPERATURE_TABLE_START;
  }

  // Двоичный поиск по таблице
  while ((r - l) > 1) {
    temperature_table_index_type m = (l + r) >> 1;
    temperature_table_entry_type mid = TEMPERATURE_TABLE_READ(m);
    if (adcsum > mid) {
      r = m;
    } else {
      l = m;
    }
  }
  temperature_table_entry_type vl = TEMPERATURE_TABLE_READ(l);
  if (adcsum >= vl) {
    return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type vr = TEMPERATURE_TABLE_READ(r);
  temperature_table_entry_type vd = vl - vr;
  int16_t res = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP; 
  if (vd) {
    // Линейная интерполяция
    res -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
  }
  return res;
}