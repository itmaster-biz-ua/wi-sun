/// \addtogroup module_scif_driver_setup
//@{
#include "scif.h"
#include "scif_framework.h"
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_aon_event.h)
#include DeviceFamily_constructPath(inc/hw_aon_rtc.h)
#include DeviceFamily_constructPath(inc/hw_aon_pmctl.h)
#include DeviceFamily_constructPath(inc/hw_aux_sce.h)
#include DeviceFamily_constructPath(inc/hw_aux_smph.h)
#include DeviceFamily_constructPath(inc/hw_aux_spim.h)
#include DeviceFamily_constructPath(inc/hw_aux_evctl.h)
#include DeviceFamily_constructPath(inc/hw_aux_aiodio.h)
#include DeviceFamily_constructPath(inc/hw_aux_timer01.h)
#include DeviceFamily_constructPath(inc/hw_aux_sysif.h)
#include DeviceFamily_constructPath(inc/hw_event.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_ioc.h)
#include <string.h>
#if defined(__IAR_SYSTEMS_ICC__)
    #include <intrinsics.h>
#endif


// OSAL function prototypes
uint32_t scifOsalEnterCriticalSection(void);
void scifOsalLeaveCriticalSection(uint32_t key);




/// Firmware image to be uploaded to the AUX RAM
static const uint16_t pAuxRamImage[] = {
    /*0x0000*/ 0x140E, 0x0417, 0x140E, 0x0440, 0x140E, 0x044A, 0x140E, 0x0467, 0x140E, 0x0470, 0x140E, 0x0479, 0x140E, 0x0482, 0x8953, 0x9954, 
    /*0x0020*/ 0x8D29, 0xBEFD, 0x4553, 0x2554, 0xAEFE, 0x445C, 0xADB7, 0x745B, 0x545B, 0x7000, 0x7CAC, 0x68B5, 0x00A7, 0x1439, 0x68B6, 0x00A8, 
    /*0x0040*/ 0x1439, 0x68B7, 0x00A9, 0x1439, 0x78AC, 0xF801, 0xFA01, 0xBEF2, 0x78B3, 0x68B5, 0xFD0E, 0x68B7, 0xED92, 0xFD06, 0x7CB3, 0x78B2, 
    /*0x0060*/ 0xFA01, 0xBE05, 0x7002, 0x7CB2, 0x78B2, 0xFA00, 0xBEFD, 0x6440, 0x0487, 0x78AC, 0x8F1F, 0xED8F, 0xEC01, 0xBE01, 0xADB7, 0x8DB7, 
    /*0x0080*/ 0x755B, 0x555B, 0x78B1, 0x60BF, 0xEF27, 0xE240, 0xEF27, 0x7000, 0x7CB1, 0x0487, 0x6477, 0x0000, 0x18B3, 0x9D88, 0x9C01, 0xB60E, 
    /*0x00A0*/ 0x10A6, 0xAF19, 0xAA00, 0xB60A, 0xA8FF, 0xAF39, 0xBE07, 0x0CAC, 0x8600, 0x88A8, 0x8F08, 0xFD47, 0x9DB7, 0x08AC, 0x8801, 0x8A01, 
    /*0x00C0*/ 0xBEEB, 0x254F, 0xAEFE, 0x645B, 0x445B, 0x4477, 0x0487, 0x5656, 0x655B, 0x455B, 0x0000, 0x0CAC, 0x0001, 0x0CAD, 0x1416, 0x0487, 
    /*0x00E0*/ 0x5657, 0x665B, 0x465B, 0x0000, 0x0CAC, 0x0002, 0x0CAD, 0x1416, 0x0487, 0x5658, 0x675B, 0x475B, 0x0000, 0x0CAC, 0x0004, 0x0CAD, 
    /*0x0100*/ 0x1416, 0x0487, 0x765B, 0x565B, 0x86FF, 0x03FF, 0x0CAF, 0x645C, 0x78AE, 0x68AF, 0xED37, 0xB605, 0x0000, 0x0CAE, 0x7CB4, 0x6540, 
    /*0x0120*/ 0x0CAF, 0x78AF, 0x68B0, 0xFD0E, 0xF801, 0xE95A, 0xFD0E, 0xBE01, 0x6553, 0xBDB7, 0x700B, 0xFB96, 0x4453, 0x2454, 0xAEFE, 0xADB7, 
    /*0x0140*/ 0x6453, 0x2454, 0xA6FE, 0x7000, 0xFB96, 0xADB7, 0x0000, 0x00BC, 0x00C1, 0x00F1, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 
    /*0x0160*/ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x701A, 0x14F2, 0x0001, 0x0CA6, 
    /*0x0180*/ 0xADB7, 0x0020, 0x8B49, 0x8DB1, 0x0041, 0x8B9F, 0x863F, 0x0301, 0x6467, 0x2567, 0xA6FE, 0x8B8E, 0x0078, 0x8BA6, 0x0018, 0x8B9E, 
    /*0x01A0*/ 0x0003, 0x8B9E, 0x8B9E, 0x8B9E, 0x8B9E, 0x6491, 0xEDB1, 0x8990, 0x0CBA, 0x14FD, 0x7000, 0x18B9, 0x8D29, 0xA601, 0xF201, 0x18B8, 
    /*0x01C0*/ 0x8D29, 0x9E01, 0xF202, 0x08BB, 0xFD28, 0xB604, 0x7CBB, 0x08AE, 0x8201, 0x0CAE, 0x0001, 0x0CA6, 0x0004, 0x18AB, 0x8D01, 0x0CAA, 
    /*0x01E0*/ 0xADB7, 0xADB7, 0xF8ED, 0xF007, 0x86FF, 0x63F8, 0xEBA3, 0x8680, 0x6000, 0xED8F, 0xEB9B, 0xEB9B, 0xADB7, 0x7079, 0xFBA7, 0x71FB, 
    /*0x0200*/ 0xFBA6, 0xFBA6, 0x4467, 0x448E, 0xADB7
};


/// Look-up table that converts from AUX I/O index to MCU IOCFG offset
static const uint8_t pAuxIoIndexToMcuIocfgOffsetLut[] = {
    0, 68, 64, 60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 16, 12, 8, 4, 0, 120, 116, 112, 108, 104, 100, 96, 92, 88, 84, 80, 76, 72
};


/** \brief Look-up table of data structure information for each task
  *
  * There is one entry per data structure (\c cfg, \c input, \c output and \c state) per task:
  * - [31:20] Data structure size (number of 16-bit words)
  * - [19:12] Buffer count (when 2+, first data structure is preceded by buffering control variables)
  * - [11:0] Address of the first data structure
  */
static const uint32_t pScifTaskDataStructInfoLut[] = {
//  cfg         input       output      state       
    0x00201170, 0x00000000, 0x00201174, 0x00000000  // ADC Window Monitor
};




/// Run-time logging signatures (CRC-16) for each data structure for each task
static const uint16_t pRtlTaskStructSignatures[] = {
    0xC2ED, 0x0000, 0x0A7D, 0x0000
};




// No task-specific initialization functions




// No task-specific uninitialization functions




/** \brief Performs driver setup dependent hardware initialization
  *
  * This function is called by the internal driver initialization function, \ref scifInit().
  */
static void scifDriverSetupInit(void) {

    // Select SCE clock frequency in active mode
    HWREG(AON_PMCTL_BASE + AON_PMCTL_O_AUXSCECLK) = AON_PMCTL_AUXSCECLK_SRC_SCLK_HFDIV2;

    // Set the default power mode
    scifSetSceOpmode(AUX_SYSIF_OPMODEREQ_REQ_A);

    // Initialize task resource dependencies
    scifInitIo(26, AUXIOMODE_ANALOG, -1, 0);

} // scifDriverSetupInit




/** \brief Performs driver setup dependent hardware uninitialization
  *
  * This function is called by the internal driver uninitialization function, \ref scifUninit().
  */
static void scifDriverSetupUninit(void) {

    // Uninitialize task resource dependencies
    scifUninitIo(26, -1);

} // scifDriverSetupUninit




/** \brief Re-initializes I/O pins used by the specified tasks
  *
  * It is possible to stop a Sensor Controller task and let the System CPU borrow and operate its I/O
  * pins. For example, the Sensor Controller can operate an SPI interface in one application state while
  * the System CPU with SSI operates the SPI interface in another application state.
  *
  * This function must be called before \ref scifExecuteTasksOnceNbl() or \ref scifStartTasksNbl() if
  * I/O pins belonging to Sensor Controller tasks have been borrowed System CPU peripherals.
  *
  * \param[in]      bvTaskIds
  *     Bit-vector of task IDs for the task I/Os to be re-initialized
  */
void scifReinitTaskIo(uint32_t bvTaskIds) {
    if (bvTaskIds & (1 << SCIF_ADC_WINDOW_MONITOR_TASK_ID)) {
        scifReinitIo(26, -1, 0);
    }
} // scifReinitTaskIo




/// Driver setup data, to be used in the call to \ref scifInit()
const SCIF_DATA_T scifDriverSetup = {
    (volatile SCIF_INT_DATA_T*) 0x400E0158,
    (volatile SCIF_TASK_CTRL_T*) 0x400E0166,
    (volatile uint16_t*) 0x400E014C,
    0x0000,
    sizeof(pAuxRamImage),
    pAuxRamImage,
    pScifTaskDataStructInfoLut,
    pAuxIoIndexToMcuIocfgOffsetLut,
    0x0000,
    24,
    scifDriverSetupInit,
    scifDriverSetupUninit,
    (volatile uint16_t*) 0x400E0154,
    (volatile uint16_t*) 0x400E0156,
    pRtlTaskStructSignatures
};




/** \brief Starts or modifies RTC-based task scheduling tick generation
  *
  * RTC-based tick generation will wake up the Sensor Controller first at the specified value of the RTC
  * and then periodically at the specified interval. The application must call this function after
  * calling \ref scifInit().
  *
  * The application must ensure that:
  * - \a tickStart is not in the past (prefer using \ref scifStartRtcTicksNow() to avoid this)
  * - \a tickPeriod is not too short. RTC ticks will be skipped silently if the Sensor Controller does
  *   not complete its tasks within a single tick interval.
  *
  * \param[in]      tickStart
  *     RTC value when the first tick is generated:
  *     - Bits 31:16 = seconds
  *     - Bits 15:0 = 1/65536 of a second
  * \param[in]      tickPeriod
  *     Interval at which subsequent ticks are generated:
  *     - Bits 31:16 = seconds
  *     - Bits 15:0 = 1/65536 of a second
  */
void scifStartRtcTicks(uint32_t tickStart, uint32_t tickPeriod) {

    // Configure RTC channel 2
    HWREG(AON_RTC_BASE + AON_RTC_O_CH2CMP) = tickStart;
    HWREG(AON_RTC_BASE + AON_RTC_O_CH2CMPINC) = tickPeriod;
    HWREG(AON_RTC_BASE + AON_RTC_O_CHCTL) |= AON_RTC_CHCTL_CH2_EN_M | AON_RTC_CHCTL_CH2_CONT_EN_M;

    // Prevent glitches to the edge detector when enabling the wake-up source
    HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_PROGWU0CFG) = AUX_SYSIF_PROGWU0CFG_WU_SRC_AON_RTC_CH2;
    HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_PROGWU0CFG) = AUX_SYSIF_PROGWU0CFG_WU_SRC_AON_RTC_CH2 | AUX_SYSIF_PROGWU0CFG_EN_M;

} // scifStartRtcTicks




/** \brief Starts or modifies RTC-based task scheduling tick generation
  *
  * RTC-based tick generation will wake up the Sensor Controller first after approximately 128 us and
  * then periodically at the specified interval. The application must call this function after calling
  * \ref scifInit().
  *
  * The application must ensure that \a tickPeriod is not too short. RTC ticks will be skipped silently
  * if the Sensor Controller does not complete its tasks within a single tick interval.
  *
  * \param[in]      tickPeriod
  *     Interval at which subsequent ticks are generated:
  *     - Bits 31:16 = seconds
  *     - Bits 15:0 = 1/65536 of a second
  */
void scifStartRtcTicksNow(uint32_t tickPeriod) {
    uint32_t key, sec, subsec;

    // Read the current RTC value
    key = scifOsalEnterCriticalSection();
    sec = HWREG(AON_RTC_BASE + AON_RTC_O_SEC);
    subsec = HWREG(AON_RTC_BASE + AON_RTC_O_SUBSEC);

    // Start RTC tick generation
    scifStartRtcTicks(((sec << 16) | (subsec >> 16)) + 8, tickPeriod);
    scifOsalLeaveCriticalSection(key);

} // scifStartRtcTicksNow




/** \brief Stops RTC-based task scheduling tick generation
  *
  * The application must call this function before calling \ref scifUninit().
  */
void scifStopRtcTicks(void) {

    // Disable RTC channel 2
    HWREG(AON_RTC_BASE + AON_RTC_O_CHCTL) &= ~(AON_RTC_CHCTL_CH2_EN_M | AON_RTC_CHCTL_CH2_CONT_EN_M);
    HWREG(AON_RTC_BASE + AON_RTC_O_SYNC);

    // Prevent glitches to the edge detector when disabling the wake-up source
    HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_PROGWU0CFG) = AUX_SYSIF_PROGWU0CFG_WU_SRC_AON_RTC_CH2;

} // scifStopRtcTicks


//@}


// Generated by DESKTOP-E85T5VB at 2024-04-20 13:22:20.091
