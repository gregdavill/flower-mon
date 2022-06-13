#include "AF.h"
#include "OSAL.h"
#include "ZComDef.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ms.h"
#include "zcl_ha.h"

#include "zcl_app.h"

#include "battery.h"
#include "version.h"
/*********************************************************************
 * CONSTANTS
 */

#define APP_DEVICE_VERSION 21
#define APP_FLAGS 0

#define APP_HWVERSION 21
#define APP_ZCLVERSION 1

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Global attributes
const uint16 zclApp_clusterRevision_all = 0x0001;

int16 zclApp_Temperature_Sensor_MeasuredValue = 0;

uint16 zclApp_RelativeHumiditySensor_MeasuredValue = 0;

uint16 zclApp_SoilHumiditySensor_MeasuredValue = 0;
uint16 zclApp_SoilHumiditySensor_MeasuredValue_old = 0;
uint16 zclApp_SoilHumiditySensor_MeasuredValueTr = 1;
uint16 zclApp_SoilHumiditySensor_MeasuredValueRawAdc = 0;
uint8 zclSampleTemperatureSensor_DeviceEnable;

uint16 zclSampleTemperatureSensor_IdentifyTime = 0;

// Basic Cluster
const uint8 zclApp_HWRevision = APP_HWVERSION;
const uint8 zclApp_ZCLVersion = APP_ZCLVERSION;
const uint8 zclApp_ApplicationVersion = 3;
const uint8 zclApp_StackVersion = 4; 

const uint8 zclApp_GenericDeviceClass = 0;
const uint8 zclApp_GenericDeviceType = 0;

//{lenght, 'd', 'a', 't', 'a'}
const uint8 zclApp_ManufacturerName[] = {14, 'g', 'r', 'e', 'g', 'd', 'a', 'v', 'i', 'l', 'l', '.', 'c', 'o', 'm'};
const uint8 zclApp_ModelId[] = {11, 'f', 'l', 'o', 'w', 'e', 'r', '-', 'm', 'o', 'n'};
const uint8 zclApp_PowerSource = POWER_SOURCE_BATTERY;

/*********************************************************************
 * ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
 */

CONST zclAttrRec_t zclApp_AttrsFirstEP[] = {
    {BASIC, {ATTRID_BASIC_ZCL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ZCLVersion}},
    {BASIC, {ATTRID_BASIC_APPL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ApplicationVersion}},
    {BASIC, {ATTRID_BASIC_STACK_VERSION, ZCL_UINT8, R, (void *)&zclApp_StackVersion}},
    {BASIC, {ATTRID_BASIC_HW_VERSION, ZCL_UINT8, R, (void *)&zclApp_HWRevision}},
    {BASIC, {ATTRID_BASIC_MANUFACTURER_NAME, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ManufacturerName}},
    {BASIC, {ATTRID_BASIC_MODEL_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ModelId}},
    {BASIC, {ATTRID_BASIC_DATE_CODE, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_DateCode}},
    {BASIC, {ATTRID_BASIC_POWER_SOURCE, ZCL_DATATYPE_ENUM8, R, (void *)&zclApp_PowerSource}},
    {BASIC, {ATTRID_BASIC_SW_BUILD_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_DateCode}},
    {BASIC, {ATTRID_CLUSTER_REVISION, ZCL_DATATYPE_UINT16, R, (void *)&zclApp_clusterRevision_all}},
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_VOLTAGE, ZCL_UINT8, RR, (void *)&zclBattery_Voltage}},
/**
 * FYI: calculating battery percentage can be tricky, since this device can be powered from 2xAA or 1xCR2032 batteries
 * */
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING, ZCL_UINT8, RR, (void *)&zclBattery_PercentageRemainig}},
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_VOLTAGE_RAW_ADC, ZCL_UINT16, RR, (void *)&zclBattery_RawAdc}},

    // *** Identify Cluster Attribute ***
    {
        ZCL_CLUSTER_ID_GEN_IDENTIFY,
        { // Attribute record
        ATTRID_IDENTIFY_TIME,
        ZCL_DATATYPE_UINT16,
        (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
        (void *)&zclSampleTemperatureSensor_IdentifyTime
        }
    },
    {
        ZCL_CLUSTER_ID_GEN_IDENTIFY,
        {  // Attribute record
        ATTRID_CLUSTER_REVISION,
        ZCL_DATATYPE_UINT16,
        ACCESS_CONTROL_READ | ACCESS_GLOBAL,
        (void *)&zclApp_clusterRevision_all
        }
    },

    {ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT, {ATTRID_MS_TEMPERATURE_MEASURED_VALUE, ZCL_INT16, RR, (void *)&zclApp_Temperature_Sensor_MeasuredValue}},
    {ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
        {  // Attribute record
        ATTRID_CLUSTER_REVISION,
        ZCL_DATATYPE_UINT16,
        ACCESS_CONTROL_READ,
        (void *)&zclApp_clusterRevision_all
        }
    },
    {HUMIDITY, {ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE, ZCL_UINT16, RR, (void *)&zclApp_RelativeHumiditySensor_MeasuredValue}},
    {SOIL_HUMIDITY, {ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE, ZCL_UINT16, RR, (void *)&zclApp_SoilHumiditySensor_MeasuredValue}}
};


uint8 CONST zclApp_NumAttributes = (sizeof(zclApp_AttrsFirstEP) / sizeof(zclApp_AttrsFirstEP[0]));

const cId_t zclApp_InClusterListFirstEP[] = {
    ZCL_CLUSTER_ID_GEN_BASIC, 
    ZCL_CLUSTER_ID_GEN_POWER_CFG,
    ZCL_CLUSTER_ID_GEN_IDENTIFY, 
    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
    SOIL_HUMIDITY,
    HUMIDITY,
};

#define APP_MAX_INCLUSTERS_FIRST_EP (sizeof(zclApp_InClusterListFirstEP) / sizeof(zclApp_InClusterListFirstEP[0]))

const cId_t zclApp_OutClusterListFirstEP[] = {
    ZCL_CLUSTER_ID_GEN_IDENTIFY
};

#define APP_MAX_OUTCLUSTERS_FIRST_EP (sizeof(zclApp_OutClusterListFirstEP) / sizeof(zclApp_OutClusterListFirstEP[0]))

SimpleDescriptionFormat_t zclApp_Desc = {
    8,                                                  //  int Endpoint;
    ZCL_HA_PROFILE_ID,                                  //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_SIMPLE_SENSOR,                      //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                                 //  int   AppDevVer:4;
    APP_FLAGS,                                          //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_FIRST_EP,                        //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListFirstEP,               //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_FIRST_EP,                       //  byte  AppNumOutClusters;
    (cId_t *)zclApp_OutClusterListFirstEP               //  byte *pAppOutClusterList;  
};


/*********************************************************************
 * @fn      zclSampleLight_ResetAttributesToDefaultValues
 *
 * @brief   Reset all writable attributes to their default values.
 *
 * @param   none
 *
 * @return  none
 */
void zclSampleTemperatureSensor_ResetAttributesToDefaultValues(void)
{
  zclSampleTemperatureSensor_IdentifyTime = 0; 
}