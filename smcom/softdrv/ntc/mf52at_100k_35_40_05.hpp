#include <avr/io.h>
#include <avr/pgmspace.h>

// Значение температуры, возвращаемое если сумма результатов АЦП больше первого значения таблицы
#define TEMPERATURE_UNDER -350
// Значение температуры, возвращаемое если сумма результатов АЦП меньше последнего значения таблицы
#define TEMPERATURE_OVER 400
// Значение температуры соответствующее первому значению таблицы
#define TEMPERATURE_TABLE_START -350
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
    63288, 63212, 63133, 63052, 62968, 62883, 62795, 62704,
    62612, 62516, 62418, 62318, 62215, 62109, 62000, 61889,
    61775, 61658, 61538, 61415, 61289, 61160, 61028, 60893,
    60755, 60613, 60469, 60321, 60169, 60014, 59856, 59695,
    59530, 59361, 59189, 59013, 58834, 58651, 58464, 58274,
    58080, 57882, 57681, 57476, 57267, 57054, 56837, 56617,
    56393, 56165, 55933, 55697, 55458, 55215, 54967, 54717,
    54462, 54203, 53941, 53675, 53406, 53132, 52855, 52575,
    52290, 52003, 51711, 51417, 51118, 50817, 50512, 50204,
    49892, 49578, 49260, 48940, 48616, 48289, 47960, 47628,
    47293, 46956, 46616, 46274, 45929, 45582, 45233, 44882,
    44529, 44174, 43817, 43459, 43098, 42737, 42374, 42009,
    41644, 41277, 40909, 40540, 40171, 39801, 39430, 39059,
    38687, 38315, 37943, 37571, 37199, 36826, 36455, 36083,
    35712, 35341, 34971, 34602, 34233, 33865, 33498, 33133,
    32768, 32405, 32043, 31682, 31323, 30965, 30609, 30254,
    29902, 29551, 29202, 28855, 28510, 28167, 27827, 27488,
    27152, 26818, 26486, 26157, 25831, 25506, 25185, 24865,
    24549, 24235, 23924, 23615, 23309, 23006, 22706
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