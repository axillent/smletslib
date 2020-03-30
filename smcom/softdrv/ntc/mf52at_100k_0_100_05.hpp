#include <avr/io.h>
#include <avr/pgmspace.h>

// Значение температуры, возвращаемое если сумма результатов АЦП больше первого значения таблицы
#define TEMPERATURE_UNDER 0
// Значение температуры, возвращаемое если сумма результатов АЦП меньше последнего значения таблицы
#define TEMPERATURE_OVER 1000
// Значение температуры соответствующее первому значению таблицы
#define TEMPERATURE_TABLE_START 0
// Шаг таблицы 
#define TEMPERATURE_TABLE_STEP 5

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
    50512, 50204, 49892, 49578, 49260, 48940, 48616, 48289,
    47960, 47628, 47293, 46956, 46616, 46274, 45929, 45582,
    45233, 44882, 44529, 44174, 43817, 43459, 43098, 42737,
    42374, 42009, 41644, 41277, 40909, 40540, 40171, 39801,
    39430, 39059, 38687, 38315, 37943, 37571, 37199, 36826,
    36455, 36083, 35712, 35341, 34971, 34602, 34233, 33865,
    33498, 33133, 32768, 32405, 32043, 31682, 31323, 30965,
    30609, 30254, 29902, 29551, 29202, 28855, 28510, 28167,
    27827, 27488, 27152, 26818, 26486, 26157, 25831, 25506,
    25185, 24865, 24549, 24235, 23924, 23615, 23309, 23006,
    22706, 22409, 22114, 21822, 21533, 21247, 20964, 20684,
    20406, 20132, 19860, 19592, 19326, 19063, 18803, 18547,
    18293, 18042, 17793, 17548, 17306, 17066, 16830, 16596,
    16365, 16137, 15912, 15690, 15471, 15254, 15040, 14829,
    14620, 14415, 14212, 14011, 13814, 13619, 13426, 13236,
    13049, 12864, 12682, 12503, 12325, 12151, 11978, 11808,
    11641, 11476, 11313, 11152, 10994, 10838, 10684, 10533,
    10384, 10236, 10091, 9948, 9807, 9668, 9532, 9397,
    9264, 9133, 9004, 8877, 8752, 8628, 8507, 8387,
    8269, 8153, 8039, 7926, 7815, 7705, 7598, 7491,
    7387, 7284, 7182, 7082, 6984, 6887, 6791, 6697,
    6605, 6513, 6424, 6335, 6248, 6162, 6077, 5994,
    5912, 5831, 5752, 5673, 5596, 5520, 5445, 5371,
    5298, 5227, 5156, 5087, 5018, 4951, 4885, 4819,
    4755, 4691, 4629, 4567, 4507, 4447, 4388, 4330,
    4273
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