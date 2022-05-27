#ifndef AIR_QUALITY_H
#define AIR_QUALITY_H

#define AIR_QUALITY_INVALID_RESPONSE 0xFFFF

typedef void (*request_measure_t)(void);
typedef uint16 (*read_t)(void);
typedef void (*set_ABC_t)(bool isEnabled);

typedef struct {
  request_measure_t RequestMeasure;
  read_t Read;
  set_ABC_t SetABC;
} zclAirSensor_t;

#endif //AIR_QUALITY_H