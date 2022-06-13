#ifndef ZCL_APP_H
#define ZCL_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */
#include "version.h"
#include "zcl.h"


/*********************************************************************
 * CONSTANTS
 */

// Application Events
#define APP_REPORT_EVT                  0x0001
#define APP_READ_SENSORS_EVT            0x0002


#define AIR_COMPENSATION_FORMULA(ADC)   ((0.179 * (double)ADC + 3500.0))
#define WATER_COMPENSATION_FORMULA(ADC) ((0.146 * (double)ADC + 1900.0))

#ifdef REPORT_0_5HOUR
   #define APP_REPORT_DELAY ((uint32) 1800000) //30 minutes
#endif
#ifdef REPORT_1HOUR
   #define APP_REPORT_DELAY ((uint32) 3600000) //1 hour
#endif
#ifdef REPORT_1_5HOURS
   #define APP_REPORT_DELAY ((uint32) 5400000) //90 minutes
#endif
#ifdef REPORT_2HOURS
   #define APP_REPORT_DELAY ((uint32) 7200000) //2 hours
#endif
#ifdef REPORT_3HOURS
   #define APP_REPORT_DELAY ((uint32) 7200000) //3 hours
#endif



/*********************************************************************
 * MACROS
 */

#define R           ACCESS_CONTROL_READ
#define RR          (R | ACCESS_REPORTABLE)

#define BASIC       ZCL_CLUSTER_ID_GEN_BASIC
#define POWER_CFG   ZCL_CLUSTER_ID_GEN_POWER_CFG

#define TEMP        ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT

#define HUMIDITY    ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY
#define SOIL_HUMIDITY                  0x0408

#define ZCL_UINT8   ZCL_DATATYPE_UINT8
#define ZCL_UINT16  ZCL_DATATYPE_UINT16
#define ZCL_UINT32  ZCL_DATATYPE_UINT32
#define ZCL_INT16   ZCL_DATATYPE_INT16
#define ZCL_INT8    ZCL_DATATYPE_INT8



/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * VARIABLES
 */

extern SimpleDescriptionFormat_t zclApp_Desc;

extern uint8 zclApp_BatteryVoltage;
extern uint8 zclApp_BatteryPercentageRemainig;
extern uint16 zclApp_BatteryVoltageRawAdc;
#ifndef PWS_MINI
extern int16 zclApp_Temperature_Sensor_MeasuredValue;
#endif

extern uint16 zclApp_RelativeHumiditySensor_MeasuredValue;

extern uint16 zclApp_SoilHumiditySensor_MeasuredValue;
extern uint16 zclApp_SoilHumiditySensor_MeasuredValue_old;
extern uint16 zclApp_SoilHumiditySensor_MeasuredValueTr;
extern uint16 zclApp_SoilHumiditySensor_MeasuredValueRawAdc;

// attribute list
extern CONST zclAttrRec_t zclApp_AttrsFirstEP[];
extern CONST uint8 zclApp_NumAttributes;

extern const uint8 zclApp_ManufacturerName[];
extern const uint8 zclApp_ModelId[];
extern const uint8 zclApp_PowerSource;


// APP_TODO: Declare application specific attributes here

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Initialization for the task
 */
extern void zclApp_Init(byte task_id);
extern void zclSampleTemperatureSensor_ResetAttributesToDefaultValues(void);
/*
 *  Event Process for the task
 */
extern UINT16 zclApp_event_loop(byte task_id, UINT16 events);

void user_delay_ms(uint32_t period);

#ifdef __cplusplus
}
#endif

#endif /* ZCL_APP_H */
