
#include "zcl_app.h"

#include <stdio.h>
#include <stdlib.h>

#include "AF.h"
#include "Debug.h"
#include "OSAL.h"
#include "OSAL_Clock.h"
#include "OSAL_PwrMgr.h"
#include "OnBoard.h"
#include "ZComDef.h"
#include "ZDApp.h"
#include "ZDNwkMgr.h"
#include "ZDObject.h"
#include "bdb.h"
#include "bdb_interface.h"
#include "gp_interface.h"
#include "math.h"
#include "nwk_util.h"
#include "zcl.h"
#include "zcl_diagnostic.h"
#include "zcl_general.h"
#include "zcl_ms.h"

/* HAL */
#include "battery.h"
#include "commissioning.h"
#include "factory_reset.h"
#include "hal_adc.h"
#include "hal_drivers.h"
#include "hal_i2c.h"
#include "hal_key.h"
#include "hal_led.h"
#include "inttempsens.h"
#include "utils.h"
#include "version.h"

/*********************************************************************
 * MACROS
 */
#define HAL_KEY_CODE_RELEASE_KEY HAL_KEY_CODE_NOKEY

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

byte zclApp_TaskID;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static uint8 currentSensorsReadingPhase = 0;
int16 temp_old = 0;
int16 tempTr = 1;
int16 startWork = 0;
bool pushBut = false;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclApp_HandleKeys(byte shift, byte keys);
static void zclApp_Report(void);

static void zclApp_ReadSensors(void);
#ifndef PWS_MINI
static void zclApp_IntTempSens(void);
#endif
static void zclApp_ReadSoilHumidity(void);
static bool zclApp_StartReadHumiditiyMeasurment(void);
static void zclApp_ReadHumiditiy(void);
static void zclApp_InitPWM(void);
void zclApp_BatteryReport(void); 

static void zcl_BasicResetCB(void);

static void zclApp_EnableSensorPWR(void){
    LREP("Powering Up Sensors\r\n");
    P1DIR |= BV(1);
    P1 |= BV(1);

    // tri state P0.4 (soil humidity pin)
    P0DIR &= ~BV(4);
    P0INP |= BV(4);
}

static void zclApp_DisableSensorPWR(void){
    LREP("Powering Down Sensors \r\n");
    P1SEL  &= ~BV(7);                    // Disable Rx on P1.
    P1DIR |= BV(0) | BV(1) | BV(2) | BV(3) | BV(5) | BV(6) | BV(7);
    P1 &= ~(BV(0) | BV(1) | BV(2) | BV(3) | BV(5) | BV(6) | BV(7));
    P1INP &= ~(BV(0) | BV(1) | BV(2) | BV(3) | BV(5) | BV(6) | BV(7));
    
    P0DIR |= BV(0) | BV(1) | BV(2) | BV(3) | BV(4) | BV(7);
    
    P0 &= ~(BV(0) | BV(1) | BV(2) | BV(3) | BV(4) | BV(7));
    P0INP |= (BV(0) | BV(1) | BV(2) | BV(3) | BV(4) | BV(7));

    P2DIR |= BV(1) | BV(2);
    P2 &= ~(BV(1) | BV(2));

    APCFG = 0;                          // Disable ADC on all I/O
}



/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclApp_CmdCallbacks = {
    zcl_BasicResetCB,  // Basic Cluster Reset command
    NULL,              // Identify Trigger Effect command
    NULL,              // On/Off cluster commands
    NULL,              // On/Off cluster enhanced command Off with Effect
    NULL,              // On/Off cluster enhanced command On with Recall Global Scene
    NULL,              // On/Off cluster enhanced command On with Timed Off
    NULL,              // RSSI Location command
    NULL               // RSSI Location Response command
};

void zclApp_Init(byte task_id) {
    LREP("Started build %s \r\n", zclApp_DateCodeNT);

    zclApp_EnableSensorPWR();


    zclApp_DisableSensorPWR();


    zclApp_InitPWM();
    HalI2CInit();

    zclApp_TaskID = task_id;
    bdb_RegisterSimpleDescriptor(&zclApp_Desc);

    zclGeneral_RegisterCmdCallbacks(zclApp_Desc.EndPoint, &zclApp_CmdCallbacks);

    /* Run an inital sensor measurement */
    zclSampleTemperatureSensor_ResetAttributesToDefaultValues();
    osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, 5000);
    //user_delay_ms(10);
    
    zcl_registerAttrList(zclApp_Desc.EndPoint, zclApp_NumAttributes, zclApp_AttrsFirstEP);
    zcl_registerForMsg(zclApp_TaskID);

    // Register for all key events - This app will handle all key events
    RegisterForKeys(zclApp_TaskID);


    /* 5m interval */
    osal_start_reload_timer(zclApp_TaskID, APP_REPORT_EVT, ((uint32) (1000UL * 60 * 5)));
}

uint16 zclApp_event_loop(uint8 task_id, uint16 events) {
    afIncomingMSGPacket_t *MSGpkt;

    (void)task_id;  // Intentionally unreferenced parameter
    if (events & SYS_EVENT_MSG) {
        while ((MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(zclApp_TaskID))) {
            switch (MSGpkt->hdr.event) {
                case KEY_CHANGE:
                    zclApp_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys);
                    break;
                case ZCL_INCOMING_MSG:
                    if (((zclIncomingMsg_t *)MSGpkt)->attrCmd) {
                        osal_mem_free(((zclIncomingMsg_t *)MSGpkt)->attrCmd);
                    }
                    break;

                default:
                    break;
            }
            // Release the memory
            osal_msg_deallocate((uint8 *)MSGpkt);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & APP_REPORT_EVT) {
        LREPMaster("APP_REPORT_EVT\r\n");
        zclApp_Report();
        return (events ^ APP_REPORT_EVT);
    }

    if (events & APP_READ_SENSORS_EVT) {
        LREPMaster("APP_READ_SENSORS_EVT\r\n");
        zclApp_ReadSensors();
        return (events ^ APP_READ_SENSORS_EVT);
    }

    // Discard unknown events
    return 0;
}

static void zclApp_HandleKeys(byte portAndAction, byte keyCode) {
    LREP("zclApp_HandleKeys portAndAction=0x%X keyCode=0x%X\r\n", portAndAction, keyCode);
    zclFactoryResetter_HandleKeys(portAndAction, keyCode);
    zclCommissioning_HandleKeys(portAndAction, keyCode);
    if (portAndAction & HAL_KEY_RELEASE) {
        LREPMaster("Key Release\r\n");  
        if (bdbAttributes.bdbNodeIsOnANetwork) {
            osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, 5000);
        } else {
            osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, 5000);
        }
        HalLedSet(HAL_LED_1, HAL_LED_MODE_BLINK);
        pushBut = true;
    }
}
static void zclApp_InitPWM(void) {
    PERCFG &= ~(0x20);  // Select Timer 3 Alternative 1 location
    P2SEL |= 0x20;
    P2DIR |= 0xC0;   // Give priority to Timer 1 channel2-3
    P1SEL |= BV(4);  // Set P1_4 to peripheral, Timer 1,channel 2
    P1DIR |= BV(4);

    T3CTL &= ~BV(4);  // Stop timer 3 (if it was running)
    T3CTL |= BV(2);   // Clear timer 3
    T3CTL &= ~0x08;   // Disable Timer 3 overflow interrupts
    T3CTL |= 0x03;    // Timer 3 mode = 3 - Up/Down

    T3CCTL1 &= ~0x40;  // Disable channel 0 interrupts
    T3CCTL1 |= BV(2);  // Ch0 mode = compare
    T3CCTL1 |= BV(4);  // Ch0 output compare mode = toggle on compare

    T3CTL &= ~(BV(7) | BV(6) | BV(5));  // Clear Prescaler divider value
    T3CC0 = 4;                          // Set ticks
}

static void zclApp_ReadSensors(void) {
    
    uint8 sleepTimeMs = 0;

    switch (currentSensorsReadingPhase++) {
        case 0:
            zclApp_EnableSensorPWR();
            zclBattery_Report();
            T3CTL |= BV(4); /* Start Timer */
            sleepTimeMs = 5;
            break;
        
        case 1:
            zclApp_ReadSoilHumidity();
            
            if(zclApp_StartReadHumiditiyMeasurment()){
                /* Sleep Processor And Then read value whan done */
                sleepTimeMs = 14;
            }else{
                /* Skip over reading if we didn't sucessfully see the i2c device */
                currentSensorsReadingPhase++;
                zclApp_DisableSensorPWR();
            }
            break;

        case 2:
            zclApp_ReadHumiditiy();
            zclApp_DisableSensorPWR();
            /* Fall through */

        case 3:
            /* Transmit all the new params */
            //bdb_RepChangedAttrValue(zclApp_Desc.EndPoint, POWER_CFG, ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING);
            //bdb_RepChangedAttrValue(zclApp_Desc.EndPoint, POWER_CFG, ATTRID_POWER_CFG_BATTERY_VOLTAGE);
            bdb_RepChangedAttrValue(zclApp_Desc.EndPoint, SOIL_HUMIDITY, 0x0000);
            bdb_RepChangedAttrValue(zclApp_Desc.EndPoint, TEMP, ATTRID_MS_TEMPERATURE_MEASURED_VALUE);
            bdb_RepChangedAttrValue(zclApp_Desc.EndPoint, HUMIDITY, ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE);
            
            //LREP("Battery voltageZCL=%d prc=%d voltage=%d\r\n", zclBattery_Voltage, zclBattery_PercentageRemainig, millivolts);
            //LREP("soilHumidityMinRangeAir=%d soilHumidityMaxRangeWater=%d\r\n", soilHumidityMinRangeAir, soilHumidityMaxRangeWater);
            //LREP("ReadSoilHumidity raw=%d mapped=%d\r\n", zclApp_SoilHumiditySensor_MeasuredValueRawAdc, zclApp_SoilHumiditySensor_MeasuredValue);
            //LREP("raw: t=%d h=%d\r\n", rawt, rawh);
            //LREP("cal: t=%d h=%dC\r\n", hum, tem);
    
        
        default:
            currentSensorsReadingPhase = 0;
            sleepTimeMs = 0;

            if(pushBut){
                // Possible to force update of attrs?
            }
            pushBut = false;
            osal_pwrmgr_task_state(zcl_TaskID, PWRMGR_CONSERVE);
            break;
    }
    
    if (sleepTimeMs) {
        osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, sleepTimeMs);
    }
}

static void zclApp_ReadSoilHumidity(void) {
    
    zclApp_SoilHumiditySensor_MeasuredValueRawAdc = adcReadSampled(SOIL_MOISTURE_PIN, HAL_ADC_RESOLUTION_14, HAL_ADC_REF_AVDD, 5);

    T3CTL &= ~BV(4); /* Stop Timer */
    T3CTL |= BV(2); /* Clear counter */

    // FYI: https://docs.google.com/spreadsheets/d/1qrFdMTo0ZrqtlGUoafeB3hplhU3GzDnVWuUK4M9OgNo/edit?usp=sharing
    uint16 soilHumidityMinRangeAir = (uint16)AIR_COMPENSATION_FORMULA(zclBattery_RawAdc);
    uint16 soilHumidityMaxRangeWater = (uint16)WATER_COMPENSATION_FORMULA(zclBattery_RawAdc);
    zclApp_SoilHumiditySensor_MeasuredValue =
        (uint16)mapRange(soilHumidityMinRangeAir, soilHumidityMaxRangeWater, 0.0, 10000.0, zclApp_SoilHumiditySensor_MeasuredValueRawAdc);
    
    //LREP("soilHumidityMinRangeAir=%d soilHumidityMaxRangeWater=%d\r\n", soilHumidityMinRangeAir, soilHumidityMaxRangeWater);
    //LREP("ReadSoilHumidity raw=%d mapped=%d\r\n", zclApp_SoilHumiditySensor_MeasuredValueRawAdc, zclApp_SoilHumiditySensor_MeasuredValue);
    //bdb_RepChangedAttrValue(zclApp_Desc.EndPoint, SOIL_HUMIDITY, 0x0000);
}


void zclApp_BatteryReport(void) {
    uint16 millivolts = getBatteryVoltage();
    zclBattery_Voltage = getBatteryVoltageZCL(millivolts);
    zclBattery_PercentageRemainig = getBatteryRemainingPercentageZCLCR2032(millivolts);

    //LREP("Battery voltageZCL=%d prc=%d voltage=%d\r\n", zclBattery_Voltage, zclBattery_PercentageRemainig, millivolts);
    //bdb_RepChangedAttrValue(8, POWER_CFG, ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING);
    //bdb_RepChangedAttrValue(8, POWER_CFG, ATTRID_POWER_CFG_BATTERY_VOLTAGE);
}


static bool zclApp_StartReadHumiditiyMeasurment(void){
    uint8 regs[8];
    int8 ret;

    /* Need 5ms after power on before COMs */

    /* Start Measurement, 1x Avg Humidity, 1x Temp */
    regs[0] = (0x00 << 3) | (0x00 << 2) | 0x01;
    ret = I2C_WriteMultByte(0x7f, 1, &regs[0], 1);
    if(ret != I2C_SUCCESS){
        LREP("I2C_ReadMultByte ret=%d \r\n", ret);
        return 0;
    }

    return 1;
}

static void zclApp_ReadHumiditiy(void) {
    uint8 regs[8];
    int8 ret;
    
    /* Max conversion time 1x time 14ms, For 4x / 8x, need ~75ms */

    uint8 timeout = 5;
    do{
        ret = I2C_ReadMultByte(0x7f, 1, regs, 1);
        if(ret != I2C_SUCCESS){
            LREP("I2C_ReadMultByte ret=%d \r\n", ret);
            return;
        }

        if(!(regs[0] & 0x01)){
            ret = I2C_ReadMultByte(0x7f, 3, regs, 5);
            if(ret != I2C_SUCCESS){
                LREP("I2C_ReadMultByte ret=%d \r\n", ret);
                return;
            }

            if(regs[0] & 0x01){
                LREP("Error Occurred?\r\n", ret);
                return;
            }

            uint16 rawt = regs[3] | (regs[4] << 8);
            uint16 rawh = regs[1] | (regs[2] << 8);

            zclApp_RelativeHumiditySensor_MeasuredValue = (uint16)mapRange(0, 100.0, 0.0, 10000.0, ((100.0f * rawh) / (1024.0)));
            zclApp_Temperature_Sensor_MeasuredValue = (uint16)( (rawt - ((1 << 10) - (25.0f/0.1f) ) ) * 10.0f);

            
            //bdb_RepChangedAttrValue(zclApp_Desc.EndPoint, TEMP, ATTRID_MS_TEMPERATURE_MEASURED_VALUE);
            //bdb_RepChangedAttrValue(zclApp_Desc.EndPoint, HUMIDITY, ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE);

            //LREP("raw: t=%d h=%d\r\n", rawt, rawh);
            //LREP("cal: t=%d h=%dC\r\n", hum, tem);
            break;
        }
    }while(timeout--);
}

static void _delay_us(uint16 microSecs) {
    while (microSecs--) {
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
    }
}

void user_delay_ms(uint32_t period) { _delay_us(1000 * period); }

static void zclApp_Report(void) { 
    osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, 5); 
}

/*********************************************************************
 * @fn      zclSampleTemperatureSensor_BasicResetCB
 *
 * @brief   Callback from the ZCL General Cluster Library
 *          to set all the Basic Cluster attributes to default values.
 *
 * @param   none
 *
 * @return  none
 */
static void zcl_BasicResetCB(void) {
    zclSampleTemperatureSensor_ResetAttributesToDefaultValues();
}

/****************************************************************************
****************************************************************************/
