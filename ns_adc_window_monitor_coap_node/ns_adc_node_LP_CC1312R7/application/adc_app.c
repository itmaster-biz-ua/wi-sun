//*****************************************************************************
//  ADC WINDOW MONITOR FOR LAUNCHPAD
//  Operating system: TI-RTOS
//
//  The Sensor Controller is used to sample a single ADC channel and monitor
//  the value. The Sensor Controller updates a bit-vector that indicates
//  whether the ADC value is:
//  - Below a configurable low threshold
//  - Above a configurable high threshold
//
//  The Sensor Controller notifies the application when the bit-vector changes
//  (triggering scTaskAlertCallback())
//
//  Copyright (C) 2024 ITmaster - https://itmaster.biz.ua/
//
//****************************************************************************/
#include "ex_include_tirtos.h"
#include "adc_app.h"
#include "scif.h"

#ifdef WISUN_NCP_ENABLE
/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "openthread/ncp.h"
#include "platform/system.h"
#elif defined(COAP_SERVICE_ENABLE)
#include "coap_service_api.h"
#include "eventOS_event_timer.h"
#endif

#include "application.h"

#define BV(n)               (1 << (n))

// Display error message if the SCIF driver has been generated with incorrect operating system setting
#if !(defined(SCIF_OSAL_TIRTOS_H) || defined(SCIF_OSAL_TIDPL_H))
    #error "SCIF driver has incorrect operating system configuration for this example. Please change to 'TI-RTOS' or 'TI Driver Porting Layer' in the Sensor Controller Studio project panel and re-generate the driver."
#endif

// Display error message if the SCIF driver has been generated with incorrect target chip package
#if !(defined(SCIF_TARGET_CHIP_PACKAGE_QFN48_7X7_RGZ) || defined(SCIF_TARGET_CHIP_PACKAGE_7X7_MOT))
    #error "SCIF driver has incorrect target chip package configuration for this example. Please change to 'QFN48 7x7 RGZ' in the Sensor Controller Studio project panel and re-generate the driver."
#endif

#define COAP_UPDATE_ADC_WINDOW_STATE_URI "upd_adc_window_state"

// Task data
Task_Struct myTask;
Char myTaskStack[1024];

// Semaphore used to wait for Sensor Controller task ALERT event
static Semaphore_Struct semScTaskAlert;


void scCtrlReadyCallback(void) {

} // scCtrlReadyCallback


void scTaskAlertCallback(void) {

    // Wake up the OS task
    Semaphore_post(Semaphore_handle(&semScTaskAlert));

} // scTaskAlertCallback


void taskFxn(UArg a0, UArg a1) {

    // Initialize the Sensor Controller
    scifOsalInit();
    scifOsalRegisterCtrlReadyCallback(scCtrlReadyCallback);
    scifOsalRegisterTaskAlertCallback(scTaskAlertCallback);
    scifInit(&scifDriverSetup);
    scifStartRtcTicksNow(0x00010000 / 8);

    // Configure and start the Sensor Controller's ADC window monitor task (not to be confused with OS tasks)
    scifTaskData.adcWindowMonitor.cfg.adcWindowHigh = get_adc_corrected_value(800);
    scifTaskData.adcWindowMonitor.cfg.adcWindowLow  = get_adc_corrected_value(400);
    scifStartTasksNbl(BV(SCIF_ADC_WINDOW_MONITOR_TASK_ID));

    // Main loop
    while (1) {
        // Wait for an ALERT callback
        Semaphore_pend(Semaphore_handle(&semScTaskAlert), BIOS_WAIT_FOREVER);

        // Clear the ALERT interrupt source
        scifClearAlertIntSource();

        uint8_t bvWindowState = scifTaskData.adcWindowMonitor.output.bvWindowState;

        // Indicate on LEDs whether the current ADC value is high and/or low
        if (scifTaskData.adcWindowMonitor.output.bvWindowState & SCIF_ADC_WINDOW_MONITOR_BV_ADC_WINDOW_LOW) {
            GPIO_write(CONFIG_GPIO_GLED, 1);
        } else {
            GPIO_write(CONFIG_GPIO_GLED, 0);
        }
        if (scifTaskData.adcWindowMonitor.output.bvWindowState & SCIF_ADC_WINDOW_MONITOR_BV_ADC_WINDOW_HIGH) {
            GPIO_write(CONFIG_GPIO_RLED, 1);
        } else {
            GPIO_write(CONFIG_GPIO_RLED, 0);
        }

        // Acknowledge the alert event
        scifAckAlertEvents();
    }

} // taskFxn


/** \brief ADC raw data correction
  *
  * This function corrects the ADC gain error and ADC offset.
  */
uint16_t get_adc_corrected_value(uint16_t adcValue) {
    int32_t adcOffset = AUXADCGetAdjustmentOffset(AUXADC_REF_FIXED);
    int32_t adcGainError = AUXADCGetAdjustmentGain(AUXADC_REF_FIXED);
    int32_t adcCorrectedValue;

    // ADC raw data correction
    adcCorrectedValue = AUXADCAdjustValueForGainAndOffset((int32_t) adcValue, adcGainError, adcOffset);

    return (uint16_t)adcCorrectedValue;
} // get_adc_corrected_value


void init_adc_tasks(void){
    Task_Params taskParams;

    // Configure the OS task
    Task_Params_init(&taskParams);
    taskParams.stack = myTaskStack;
    taskParams.stackSize = sizeof(myTaskStack);
    taskParams.priority = 3;
    Task_construct(&myTask, taskFxn, &taskParams, NULL);

    // Create the semaphore used to wait for Sensor Controller ALERT events
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&semScTaskAlert, 0, &semParams);


} // init_adc_tasks
