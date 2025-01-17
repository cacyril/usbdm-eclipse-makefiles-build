/**
 * @file     pcr.h (180.ARM_Peripherals/Project_Headers/pcr-MKV.h)
 * @brief    Port Control Register interface
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */
#ifndef HEADER_PCR_H
#define HEADER_PCR_H
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include <stddef.h>
#include "derivative.h"
#include "bitband.h"

/*
 * Default port information
 */
namespace USBDM {

/**
 * Enable and set priority of interrupts in NVIC.
 *
 * @param[in]  irqNum        Interrupt number
 * @param[in]  nvicPriority  Interrupt priority
 *
 * @note Any pending interrupts are cleared before enabling.
 */
void enableNvicInterrupt(IRQn_Type irqNum, uint32_t nvicPriority);

/**
 * @addtogroup PeripheralPinTables Peripheral Information Classes
 * @brief Provides information about pins used by a peripheral
 * @{
 */

/**
 * Used to indicate or control the polarity of an I/O with selectable polarity
 */
enum Polarity {
   ActiveLow=false,  //!< Signal is active low i.e. Active => Low level, Inactive => High level
   ActiveHigh=true   //!< Signal is active high i.e. Active => High level, Inactive => Low level
};

/** Pin number indicating the function has a fixed mapping to a pin */
constexpr   int8_t FIXED_NO_PCR         = 0x00;

/** Pin number indicating the function doesn't exist. Note: -ve value*/
constexpr   int8_t INVALID_PCR          = 0xA5;

/** Pin number indicating the function is not currently mapped to a pin. Note: -ve value */
constexpr   int8_t UNMAPPED_PCR         = 0xA4;

#ifdef PCC
/**
 * Enable clock to selected ports
 *
 * @param[in] pccAddress Address of PCC register for port to enable
 */
static inline void enablePortClocks(uint32_t pccAddress) {
   *(uint32_t *)pccAddress |= PCC_PCCn_CGC_MASK;
   __DMB();
};

/**
 * Disable clock to selected ports
 *
 * @param[in] pccAddress Address of PCC register for port to disable
 */
static inline void disablePortClocks(uint32_t pccAddress) {
   *(uint32_t *)pccAddress &= ~PCC_PCCn_CGC_MASK;
   __DMB();
};

#else
/**
 * Enable clock to selected ports
 *
 * @param[in] clockMask Mask for PORTs to enable
 */
static inline void enablePortClocks(uint32_t clockMask) {
   SIM->SCGC5 |= clockMask;
   __DMB();
};

/**
 * Disable clock to selected ports
 *
 * @param[in] clockMask Mask for PORTs to disable
 */
static inline void disablePortClocks(uint32_t clockMask) {
   SIM->SCGC5 &= ~clockMask;
   __DMB();
};

/*
 * PORT clock masks
 */
#if defined(SIM_SCGC5_PORTA_MASK)
static constexpr uint32_t PORTA_CLOCK_MASK = SIM_SCGC5_PORTA_MASK;
#endif
#if defined(SIM_SCGC5_PORTB_MASK)
static constexpr uint32_t PORTB_CLOCK_MASK = SIM_SCGC5_PORTB_MASK;
#endif
#if defined(SIM_SCGC5_PORTC_MASK)
static constexpr uint32_t PORTC_CLOCK_MASK = SIM_SCGC5_PORTC_MASK;
#endif
#if defined(SIM_SCGC5_PORTD_MASK)
static constexpr uint32_t PORTD_CLOCK_MASK = SIM_SCGC5_PORTD_MASK;
#endif
#if defined(SIM_SCGC5_PORTE_MASK)
static constexpr uint32_t PORTE_CLOCK_MASK = SIM_SCGC5_PORTE_MASK;
#endif
#if defined(SIM_SCGC5_PORTF_MASK)
static constexpr uint32_t PORTF_CLOCK_MASK = SIM_SCGC5_PORTF_MASK;
#endif
#if defined(SIM_SCGC5_PORTG_MASK)
static constexpr uint32_t PORTG_CLOCK_MASK = SIM_SCGC5_PORTG_MASK;
#endif
#endif

/**
 * Port information
 * Information required to configure the PCR for a particular function
 */
class PortInfo {
public:
   const uint32_t  portAddress;   //!< Port hardware base pointer
   const uint32_t  clockInfo;     //!< Either clock mask or port clock control register address
   const IRQn_Type irqNum;        //!< Port interrupt number
};

/**
 * Pin information
 */
class PinInfo {
public:
   const uint32_t   portAddress;  //!< Port hardware base pointer
   const uint32_t   clockInfo;    //!< Either clock mask or port clock control register address
   const IRQn_Type  irqNum;       //!< Port interrupt number
   const uint32_t   gpioAddress;  //!< GPIO Hardware base pointer
   const int32_t    gpioBit;      //!< Bit number for pin - must be signed for error checks
   const uint32_t   pcrValue;     //!< Default PCR value for pin

   /**
    *
    * @param portInfo      Describes port
    * @param gpioAddress   Base address of associated GPIO
    * @param gpioBit       Bit number GPIO being modified
    * @param pcrValue      Default PCR value for pin
    */
   constexpr PinInfo(
         const PortInfo &portInfo,
         uint32_t        gpioAddress,
         int             gpioBit,
         uint32_t        pcrValue) :
                     portAddress(portInfo.portAddress), clockInfo(portInfo.clockInfo), irqNum(portInfo.irqNum),
                     gpioAddress(gpioAddress), gpioBit(gpioBit), pcrValue(pcrValue) {}
};

#ifndef PORT_PCR_LK
/**
 * Some devices don't have LK function on pin
 */
#define PORT_PCR_LK(x) (0)
#endif

#ifndef PORT_PCR_DSE
/**
 * Some devices don't have DSE function on pin
 */
#define PORT_PCR_DSE(x) (0)
#endif

#ifndef PORT_PCR_ODE
/**
 * Some devices don't have ODE function on pin
 * The open-drain mode is automatically selected when I2C function is selected for the pin
 */
#define PORT_PCR_ODE(x) 0
#endif

#ifndef PORT_PCR_PE
/**
 * Some devices don't have PE function on pin
 */
#define PORT_PCR_PE(x) 0
#endif

#ifndef PORT_PCR_PS
/**
 * Some devices don't have PS function on pin
 */
#define PORT_PCR_PS(x) 0
#endif

#ifndef PORT_PCR_SRE
/**
 * Some devices don't have PS function on pin
 */
#define PORT_PCR_SRE(x) 0
#endif

#ifndef PORT_PCR_PFE
/**
 * Some devices don't have PFE function on pin
 */
#define PORT_PCR_PFE(x) 0
#endif

/**
 * Pull device modes
 *
 * @note Not all pins support this function
 */
enum PinPull {
   PinPull_None = PORT_PCR_PE(0),                //!< No pull device
   PinPull_Up   = PORT_PCR_PE(1)|PORT_PCR_PS(1), //!< Weak pull-up
   PinPull_Down = PORT_PCR_PE(1)|PORT_PCR_PS(0), //!< Weak pull-down
};

/**
 * Pin drive strengths
 *
 * @note Few pins support this function
 */
enum PinDriveStrength {
   PinDriveStrength_Low  = PORT_PCR_DSE(0), //!< Low drive strength
   PinDriveStrength_High = PORT_PCR_DSE(1), //!< High drive strength
};

/**
 * Pin drive mode
 *
 * @note Not all pins support this function
 */
enum PinDriveMode {
   PinDriveMode_PushPull      = PORT_PCR_ODE(0), //!< Push-pull output
   PinDriveMode_OpenDrain     = PORT_PCR_ODE(1), //!< Open-drain output
   PinDriveMode_OpenCollector = PinDriveMode_OpenDrain,       //!< For oldies like me
};

/**
 * Pin Slew rate control
 *
 * @note Few pins support this function
 */
enum PinSlewRate {
   PinSlewRate_Slow = PORT_PCR_SRE(1),  //!< Slow slew rate on output
   PinSlewRate_Fast = PORT_PCR_SRE(0),  //!< Fast slew rate on output
};

/**
 * Pin filter mode
 *
 * @note Few pins support this function
 */
enum PinFilter {
   PinFilter_None      = PORT_PCR_PFE(0),  //!< No pin filter
   PinFilter_Passive   = PORT_PCR_PFE(1),  //!< Pin filter enabled
#ifdef PORT_DFCR_CS_MASK
   PinFilter_Digital   = (1<<14), //!< Digital pin filter enabled - Not available on all ports
#endif
};

/**
 * Pin Multiplexor setting
 */
enum PinMux {
   PinMux_None      = PORT_PCR_MUX(0), //!< Used for default/no mapping
   PinMux_Analogue  = PORT_PCR_MUX(0), //!< Analogue function
   PinMux_Analog    = PinMux_Analogue,  //!< Analogue function
   PinMux_Tsi       = PinMux_Analogue,  //!< Touch Sense function is analogue
   PinMux_Gpio      = PORT_PCR_MUX(1), //!< Gpio function
   PinMux_2         = PORT_PCR_MUX(2), //!< Multiplexor 2
   PinMux_3         = PORT_PCR_MUX(3), //!< Multiplexor 3
   PinMux_4         = PORT_PCR_MUX(4), //!< Multiplexor 4
   PinMux_5         = PORT_PCR_MUX(5), //!< Multiplexor 5
   PinMux_6         = PORT_PCR_MUX(6), //!< Multiplexor 6
   PinMux_7         = PORT_PCR_MUX(7), //!< Multiplexor 7
};

/**
 * Pin interrupt/DMA actions
 *
 * @note Not all pins support this function
 */
enum PinAction {
   PinAction_None        = PORT_PCR_IRQC(0),   //!< No interrupt or DMA function

   PinAction_DmaRising   = PORT_PCR_IRQC(1),   //!< Generate DMA request on rising edge
   PinAction_DmaFalling  = PORT_PCR_IRQC(2),   //!< Generate DMA request on falling edge
   PinAction_DmaEither   = PORT_PCR_IRQC(3),   //!< Generate DMA request on either edge

   PinAction_IrqLow      = PORT_PCR_IRQC(8),   //!< Generate IRQ request when low
   PinAction_IrqRising   = PORT_PCR_IRQC(9),   //!< Generate IRQ request on rising edge
   PinAction_IrqFalling  = PORT_PCR_IRQC(10),  //!< Generate IRQ request on falling edge
   PinAction_IrqEither   = PORT_PCR_IRQC(11),  //!< Generate IRQ request on either edge
   PinAction_IrqHigh     = PORT_PCR_IRQC(12),  //!< Generate IRQ request when high
};

/** Type for PCR value used by PCR functions */
using PcrValue = uint32_t;

/**
 * Construct PCR value from individual bit masks
 *
 * @param[in] pinPull          One of PinPull_None, PinPull_Up, PinPull_Down (defaults to PinPull_None)
 * @param[in] pinDriveStrength One of PinDriveStrength_Low, PinDriveStrength_High (defaults to PinDriveLow)
 * @param[in] pinDriveMode     One of PinDriveMode_PushPull, PinDriveMode_OpenDrain (defaults to PinPushPull)
 * @param[in] pinAction        One of PinAction_None, etc (defaults to PinAction_None)
 * @param[in] pinFilter        One of PinFilter_None, PinFilter_Passive (defaults to PinFilter_None)
 * @param[in] pinSlewRate      One of PinSlewRate_Slow, PinSlewRate_Fast (defaults to PinSlewRate_Fast)
 * @param[in] pinMux           Multiplexor setting (defaults to 0)
 *
 * @return PCR value constructed from individual flags
 */
static constexpr PcrValue pcrValue(
      PinPull           pinPull           = PinPull_None,
      PinDriveStrength  pinDriveStrength  = PinDriveStrength_Low,
      PinDriveMode      pinDriveMode      = PinDriveMode_PushPull,
      PinAction         pinAction         = PinAction_None,
      PinFilter         pinFilter         = PinFilter_None,
      PinSlewRate       pinSlewRate       = PinSlewRate_Fast,
      PinMux            pinMux            = PinMux_None
      ) {
   return pinPull|pinDriveStrength|pinDriveMode|pinAction|pinFilter|pinSlewRate|pinMux;
}

#ifdef PORT_DFCR_CS_MASK
   /**
    * Controls the Digital Pin Filter clock source
    */
   enum PinDigitalFilterClock {
      PinDigitalFilterClock_BusClock = PORT_DFCR_CS(0),//!< Pin Digital Filter Clock = BusClock
      PinDigitalFilterClock_LpoClock = PORT_DFCR_CS(1),//!< Pin Digital Filter Clock = Low Power Oscillator
   };
#endif

/**
 * Construct PCR value from existing PcrValue and extra flags
 *
 * @param[in] original      Original value
 * @param[in] extraFlags    Extra flags
 *
 * @return PCR value constructed from individual flags
 */
static constexpr PcrValue pcrValue(PcrValue original, uint32_t extraFlags) {
   return original|extraFlags;
}

/**
 * Construct PCR value from flags
 *
 * @param[in] flags Extra flags
 *
 * @return PCR value constructed from individual flags
 */
static constexpr PcrValue pcrValue(uint32_t flags) {
   return flags;
}

/**
 * Default PCR setting for pins (excluding multiplexor value)
 * High drive strength + Pull-up
 */
static constexpr PcrValue DEFAULT_PCR = pcrValue(PinPull_None, PinDriveStrength_Low);

/**
 * Default PCR value for pins used as GPIO (including multiplexor value)
 * High drive strength + Pull-up + GPIO_MUX value
 */
static constexpr PcrValue GPIO_DEFAULT_PCR = pcrValue(PinPull_None, PinDriveStrength_Low, PinDriveMode_PushPull, PinAction_None, PinFilter_None, PinSlewRate_Fast, PinMux_Gpio);

/**
 * Default PCR setting for I2C pins (excluding multiplexor value)
 * High drive strength + Pull-up + Open-drain (if available)
 */
static constexpr PcrValue I2C_DEFAULT_PCR =  pcrValue(PinPull_Up, PinDriveStrength_High, PinDriveMode_OpenDrain);

/**
 * Default PCR setting for I2S pins (excluding multiplexor value)
 * High drive strength + Pull-up + Open-drain (if available)
 */
static constexpr PcrValue I2S_DEFAULT_PCR =  pcrValue(PinPull_Up, PinDriveStrength_High, PinDriveMode_OpenDrain);

/**
 * Default PCR setting for (E)XTAL pins (excluding multiplexor value)
 */
static constexpr PcrValue XTAL_DEFAULT_PCR = pcrValue(PinPull_None, PinDriveStrength_Low, PinDriveMode_PushPull);

/**
 * Type definition for PORT interrupt call back.
 * This callback is shared by all port pins
 *
 * @param[in] status 32-bit value from ISFR (each bit indicates a pin interrupt source)
 */
typedef void (*PinCallbackFunction)(uint32_t status);

/**
 * Provides common unhandledCallback for all PORTs
 */
class PcrBase {

private:
   /**
    * This class is not intended to be instantiated
    */
   PcrBase() = delete;
   PcrBase(const PcrBase&) = delete;
   PcrBase(PcrBase&&) = delete;

public:
   /** Callback to catch unhandled interrupt */
   static void unhandledCallback(uint32_t) {
      setAndCheckErrorCode(E_NO_HANDLER);
   }
};

/**
 * Common PORT features shared by all port pins
 *
 * tparam portAddress Address of port to be used
 */
template<uint32_t portAddress, IRQn_Type irqNum>
class PcrBase_T {

private:
   /**
    * This class is not intended to be instantiated
    */
   PcrBase_T() = delete;
   PcrBase_T(const PcrBase&) = delete;
   PcrBase_T(PcrBase&&) = delete;

   /** Callback functions for ISRs */
   static PinCallbackFunction fCallback;

public:
#ifdef PORT_DFCR_CS_MASK
   static volatile PORT_DFER_Type &port()   { return *reinterpret_cast<volatile PORT_DFER_Type *>(portAddress); }
#else
   static volatile PORT_Type &port()   { return *reinterpret_cast<volatile PORT_Type *>(portAddress); }
#endif

   /**
    * Interrupt handler\n
    *  - Clears interrupt flag
    *  - Calls callback
    */
   static void irqHandler() {
      // Capture interrupt flags
      uint32_t status = port().ISFR;

      // Clear flags
      port().ISFR = status;

      fCallback(status);
   }

   /**
    * Set callback for Pin interrupts
    *
    * @param[in] callback The function to call on Pin interrupt. \n
    *                     nullptr to indicate none
    *
    * @return E_NO_ERROR            No error
    * @return E_HANDLER_ALREADY_SET Handler already set
    *
    * @note There is a single callback function for all pins on the related port.
    *       It is necessary to identify the originating pin in the callback
    */
   static ErrorCode setCallback(PinCallbackFunction callback) {
      if (callback == nullptr) {
         fCallback = PcrBase::unhandledCallback;
         return E_NO_ERROR;
      }
#ifdef DEBUG_BUILD
      // Callback is shared across all port pins. Check if callback already assigned
      if ((fCallback != PcrBase::unhandledCallback) && (fCallback != callback)) {
         return setErrorCode(ErrorCode::E_HANDLER_ALREADY_SET);
      }
#endif
      fCallback = callback;
      return E_NO_ERROR;
   }

   /**
    * Enable Pin interrupts in NVIC.
    */
   static void enableNvicInterrupts() {
      static_assert(irqNum>=0, "Pin does not support interrupts");
      NVIC_EnableIRQ(irqNum);
   }

   /**
    * Enable and set priority of Pin interrupts in NVIC.
    * Any pending NVIC interrupts are first cleared.
    *
    * @param[in]  nvicPriority  Interrupt priority
    */
   static void enableNvicInterrupts(uint32_t nvicPriority=NvicPriority_Normal) {
      static_assert(irqNum>=0, "Pin does not support interrupts");
      enableNvicInterrupt(irqNum, nvicPriority);
   }

   /**
    * Disable Pin interrupts in NVIC.
    */
   static void disableNvicInterrupts() {
      static_assert(irqNum>=0, "Pin does not support interrupts");
      NVIC_DisableIRQ(irqNum);
   }
};

/**
 * @brief Template representing a Pin Control Register (PCR)
 *
 * Code examples:
 * @code
 * // Create PCR type
 * using PortC_3 = USBDM::Pcr_T<SIM_SCGC5_PORTC_MASK, PORTC_BasePtr, 3, USBDM::DEFAULT_PCR>;
 *
 * // Configure PCR
 * PortC_3::setPCR(PinPull_Up,PinDriveStrength_High,PinDriveMode_PushPull,PinAction_None,PinFilter_None,PinSlewRate_Fast,PinMux_3);
 *
 * // Disable Port clock
 * PortC_3::disableClock();
 * @endcode
 *
 * @tparam clockInfo       PCC register address or mask for SIM clock register associated with this PCR
 * @tparam portAddress     PORT to be manipulated e.g. PORTA (PCR array)
 * @tparam bitNum          Bit number e.g. 3
 * @tparam defPcrValue     Default value for PCR (including MUX value)
 */
template<uint32_t clockInfo, uint32_t portAddress, IRQn_Type irqNum, int bitNum, PcrValue defPcrValue>
class Pcr_T : public PcrBase_T<portAddress, irqNum> {

//#ifdef DEBUG_BUILD
//   static_assert((bitNum != UNMAPPED_PCR), "Pcr_T: Signal is not mapped to a pin - Modify Configure.usbdm");
//   static_assert((bitNum != INVALID_PCR),  "Pcr_T: Non-existent signal");
//   static_assert((bitNum == UNMAPPED_PCR)||(bitNum == INVALID_PCR)||((bitNum >= 0)&&(bitNum <= 31)), "Pcr_T: Illegal bit number");
//#endif

public:
   static constexpr int      BITNUM  = bitNum;        ///< Bit number for port bit
   static constexpr uint32_t BITMASK = (1<<bitNum);   ///< Bit mask for port bit

private:
   /**
    * This class is not intended to be instantiated
    */
   Pcr_T() = delete;
   Pcr_T(const PcrBase&) = delete;
   Pcr_T(PcrBase&&) = delete;

#ifdef PORT_DFCR_CS_MASK
   static volatile uint32_t       &pcrReg() { return *reinterpret_cast<volatile uint32_t       *>(portAddress+offsetof(PORT_DFER_Type,PCR[bitNum])); }
   static volatile PORT_DFER_Type &port()   { return *reinterpret_cast<volatile PORT_DFER_Type *>(portAddress); }
#else
   static volatile uint32_t       &pcrReg() { return *reinterpret_cast<volatile uint32_t       *>(portAddress+offsetof(PORT_Type,PCR[bitNum])); }
#endif

public:
   /**
    * Enable clock associated with PORT
    */
   static void enableClock() {
      enablePortClocks(clockInfo);
   }

   /**
    * Disable clock associated with PORT
    */
   static void disableClock() {
      disablePortClocks(clockInfo);
   }

   /**
    * Set Pin Control Register (PCR) value \n
    * The clock to the port will be enabled before changing the PCR
    *
    * @param[in] pcrValue PCR value constructed using pcrValue() including MUX value.\n
    *                     Defaults to template value.
    */
   static void setPCR(PcrValue pcrValue=defPcrValue) {
      if (portAddress != 0) {
         enablePortClocks(clockInfo);

#ifdef PORT_DFCR_CS_MASK
         if (pcrValue&PinFilter_Digital) {
            enableDigitalFilter();
            pcrValue &= ~PinFilter_Digital;
         }
         else {
           disableDigitalFilter();
         }
#endif
         // Pointer to PCR register for pin
         pcrReg() = pcrValue;
      }
   }
   /**
    * Get Pin Control Register (PCR) value \n
    * The clock to the port will be enabled before reading the PCR
    *
    * @return pcrValue PCR value.
    */
   static PcrValue getPCR() {
      if (portAddress == 0) {
         return (PcrValue) 0;
      }
      enablePortClocks(clockInfo);
      return pcrReg();
   }
   /**
    * Set Pin Control Register (PCR) value \n
    * The clock to the port will be enabled before changing the PCR
    *
    * @param[in] pinPull          One of PinPull_None, PinPull_Up, PinPull_Down
    * @param[in] pinDriveStrength One of PinDriveStrength_Low, PinDriveStrength_High (defaults to PinDriveLow)
    * @param[in] pinDriveMode     One of PinDriveMode_PushPull, PinDriveMode_OpenDrain (defaults to PinPushPull)
    * @param[in] pinAction        One of PinAction_None, etc (defaults to PinAction_None)
    * @param[in] pinFilter        One of PinFilter_None, PinFilter_Passive (defaults to PinFilter_None)
    * @param[in] pinSlewRate      One of PinSlewRate_Slow, PinSlewRate_Fast (defaults to PinSlewRate_Fast)
    * @param[in] pinMux           One of PinMux_Analogue, PinMux_Gpio etc (defaults to template value)
    */
   static void setPCR(
         PinPull           pinPull,
         PinDriveStrength  pinDriveStrength  = PinDriveStrength_Low,
         PinDriveMode      pinDriveMode      = PinDriveMode_PushPull,
         PinAction         pinAction         = PinAction_None,
         PinFilter         pinFilter         = PinFilter_None,
         PinSlewRate       pinSlewRate       = PinSlewRate_Fast,
         PinMux            pinMux            = (PinMux)(defPcrValue&PORT_PCR_MUX_MASK)
         ) {
      if (portAddress != 0) {
         enablePortClocks(clockInfo);
#ifdef PORT_DFCR_CS_MASK
         if (pinFilter == PinFilter_Digital) {
            enableDigitalFilter();
            pinFilter = PinFilter_None;
         }
         else {
           disableDigitalFilter();
         }
#endif
         // Set PCR register for pin
         pcrReg() = pinPull|pinDriveStrength|pinDriveMode|pinAction|pinFilter|pinSlewRate|pinMux;
      }
   }

#if defined(PORT_PCR_ODE_MASK) and defined (PORT_PCR_SRE_MASK)
   /**
    * @brief
    * Set subset of Pin Control Register Attributes associated with output direction \n
    * Only specified attributes are changed.\n
    * Assumes clock to the port has already been enabled
    *
    * @param[in] pinDriveStrength One of PinDriveStrength_Low, PinDriveStrength_High
    * @param[in] pinDriveMode     One of PinDriveMode_PushPull, PinDriveMode_OpenDrain (defaults to PinPushPull)
    * @param[in] pinSlewRate      One of PinSlewRate_Slow, PinSlewRate_Fast (defaults to PinSlewRate_Fast)
    */
   static void setOutput(
         PinDriveStrength  pinDriveStrength,
         PinDriveMode      pinDriveMode      = PinDriveMode_PushPull,
         PinSlewRate       pinSlewRate       = PinSlewRate_Fast) {

      pcrReg() =
            (pcrReg()&~(PORT_PCR_DSE_MASK|PORT_PCR_ODE_MASK|PORT_PCR_SRE_MASK)) |
            (pinDriveStrength|pinDriveMode|pinSlewRate);
   }
#elif defined(PORT_PCR_ODE_ASK)
   /**
    * @brief
    * Set subset of Pin Control Register Attributes associated with output direction \n
    * Only specified attributes are changed.\n
    * Assumes clock to the port has already been enabled
    *
    * @param[in] pinDriveStrength One of PinDriveStrength_Low, PinDriveStrength_High
    * @param[in] pinDriveMode     One of PinDriveMode_PushPull, PinDriveMode_OpenDrain (defaults to PinPushPull)
    */
   static void setOutput(
         PinDriveStrength  pinDriveStrength,
         PinDriveMode      pinDriveMode      = PinDriveMode_PushPull) {

      pcrReg() =
            (pcrReg()&~(PORT_PCR_DSE_MASK|PORT_PCR_ODE_ASK)) |
            (pinDriveStrength|pinDriveMode);
   }
#elif defined(PORT_PCR_SRE_MASK)
   /**
    * @brief
    * Set subset of Pin Control Register Attributes associated with output direction \n
    * Only specified attributes are changed.\n
    * Assumes clock to the port has already been enabled
    *
    * @param[in] pinDriveStrength One of PinDriveStrength_Low, PinDriveStrength_High
    * @param[in] pinSlewRate      One of PinSlewRate_Slow, PinSlewRate_Fast (defaults to PinSlewRate_Fast)
    */
   static void setOutput(
         PinDriveStrength  pinDriveStrength,
         PinSlewRate       pinSlewRate       = PinSlewRate_Fast) {

      pcrReg() =
            (pcrReg()&~(PORT_PCR_DSE_MASK|PORT_PCR_SRE_MASK)) |
            (pinDriveStrength|pinSlewRate);
   }
#else
   /**
    * @brief
    * Set subset of Pin Control Register Attributes associated with output direction \n
    * Only specified attributes are changed.\n
    * Assumes clock to the port has already been enabled
    *
    * @param[in] pinDriveStrength One of PinDriveStrength_Low, PinDriveStrength_High
    */
   static void setOutput(
         PinDriveStrength  pinDriveStrength) {

      pcrReg() =
            (pcrReg()&~(PORT_PCR_DSE_MASK)) | (pinDriveStrength);
   }

#endif

   /**
    * @brief
    * Set subset of Pin Control Register Attributes associated with input direction \n
    * Only specified attributes are changed.\n
    * Assumes clock to the port has already been enabled.
    *
    * @param[in] pinPull          One of PinPull_None, PinPull_Up, PinPull_Down
    * @param[in] pinAction        One of PinAction_None, etc (defaults to PinAction_None)
    * @param[in] pinFilter        One of PinFilter_None, PinFilter_Passive (defaults to PinFilter_None)
    *
    *  @note see also configureDigitalFilter()
    */
   static void setInput(
         PinPull           pinPull,
         PinAction         pinAction         = PinAction_None,
         PinFilter         pinFilter         = PinFilter_None) {

#ifdef PORT_DFCR_CS_MASK
         if (pinFilter == PinFilter_Digital) {
            enableDigitalFilter();
            pinFilter = PinFilter_None;
         }
         else {
           disableDigitalFilter();
         }
#endif
      pcrReg() =
            (pcrReg()&~(PORT_PCR_PD_MASK|PORT_PCR_IRQC_MASK|PORT_PCR_PFE_MASK)) |
            (pinPull|pinAction|pinFilter);
   }

   /**
    * Set pin PCR.MUX value.
    * Assumes clock to the port has already been enabled.
    *
    * @param[in] pinMux PCR MUX value [0..7]
    */
   static void setMux(PinMux pinMux) {
      pcrReg() = (pcrReg()&~PORT_PCR_MUX_MASK) | pinMux;
   }
   /**
    * Sets pin interrupt/DMA action.
    * Assumes clock to the port has already been enabled.
    *
    * @param[in] pinAction Interrupt/DMA mode
    */
   static void setPinAction(PinAction pinAction) {
      pcrReg() = (pcrReg()&~PORT_PCR_IRQC_MASK) | pinAction;
   }

   /**
    * Clear pin interrupt flag.
    * Assumes clock to the port has already been enabled.
    */
   static void clearInterruptFlag() {
      pcrReg() |= PORT_PCR_ISF_MASK;
   }

#ifdef PORT_PCR_PE_MASK
   /**
    * Set pull device on pin.
    * Assumes clock to the port has already been enabled.
    *
    *  @param[in] pinPull Pull selection mode
    */
   static void setPullDevice(PinPull pinPull) {
      pcrReg() = (pcrReg()&~PORT_PCR_PD_MASK) | pinPull;
   }
#else
   /**
    * Not supported
    */
   static void setPullDevice(PinPullMode) {
   }
#endif

#ifdef PORT_PCR_ODE_MASK
   /**
    * Set drive mode on pin.
    * Assumes clock to the port has already been enabled.
    *
    *  @param[in] pinDriveMode Drive mode
    */
   static void setDriveMode(PinDriveMode pinDriveMode) {
      pcrReg() = (pcrReg()&~PORT_PCR_ODE_MASK) | pinDriveMode;
   }
#else
   /**
    * Not supported
    */
   static void setDriveMode(PinDriveMode) {
   }
#endif

#ifdef PORT_PCR_SRE_MASK
   /**
    * Set slew rate on pin.
    * Assumes clock to the port has already been enabled.
    *
    *  @param[in] pinSlewRate Slew rate. Either PinSlewRate_Slow or PinSlewRate_Fast
    */
   static void setSlewRate(PinSlewRate  pinSlewRate) {
      pcrReg() = (pcrReg()&~PORT_PCR_SRE_MASK) | pinSlewRate;
   }
#else
   /**
    * Not supported
    */
   static void setSlewRate(PinSlewRate) {
   }
#endif

#if defined PORT_PCR_PFE_MASK
   /**
    * Set filter on pin.
    * Assumes clock to the port has already been enabled.
    *
    *  @param[in] pinFilter Pin filter option. Either PinFilter_None or PinFilter_Passive
    *
    *  @note see also configureDigitalFilter()
    */
   static void setFilter(PinFilter pinFilter) {
#ifdef PORT_DFCR_CS_MASK
         if (pinFilter == PinFilter_Digital) {
            enableDigitalFilter();
            pinFilter = PinFilter_None;
         }
         else {
           disableDigitalFilter();
         }
#endif
         pcrReg() = (pcrReg()&~PORT_PCR_PFE_MASK) | pinFilter;
   }
#endif

#if defined(PORT_PCR_DSE_MASK)
   /**
    * Set drive strength on pin.
    * Assumes clock to the port has already been enabled.
    *
    *  @param[in] pinDriveStrength Drive strength to set
    */
   static void setDriveStrength(PinDriveStrength pinDriveStrength) {
      pcrReg() = (pcrReg()&~PORT_PCR_DSE_MASK) | pinDriveStrength;
   }
#else
   /**
    * Not supported
    */
   static void setDriveStrength(PinDriveStrength) {
   }
#endif

#if defined(PORT_PCR_LK_MASK)
   /**
    * Locks most of the PCR properties e.g. drive strength, pull-device etc.
    * Assumes clock to the port has already been enabled
    * The pin properties remains locked until the next reset
    * Not supported on all devices
    */
   static void lock() {
      pcrReg() |= PORT_PCR_LK_MASK;
   }
#else
   /**
    * Not supported
    */
   static void lock() {
   }
#endif

   /**
    * Enable Pin interrupts in NVIC.
    */
   static void enableNvicInterrupts() {
      static_assert(irqNum>=0, "Pin does not support interrupts");
      NVIC_EnableIRQ(irqNum);
   }

   /**
    * Enable and set priority of Pin interrupts in NVIC.
    * Any pending NVIC interrupts are first cleared.
    *
    * @param[in]  nvicPriority  Interrupt priority
    */
   static void enableNvicInterrupts(uint32_t nvicPriority) {
      static_assert(irqNum>=0, "Pin does not support interrupts");
      enableNvicInterrupt(irqNum, nvicPriority);
   }

   /**
    * Disable Pin interrupts in NVIC.
    */
   static void disableNvicInterrupts() {
      static_assert(irqNum>=0, "Pin does not support interrupts");
      // Disable interrupts
      NVIC_DisableIRQ(irqNum);
   }

#ifdef PORT_DFCR_CS_MASK
   /**
    * Configures Digital Pin Filter
    *
    * @param pinDigitalFilterClock  Clock source
    * @param filterLength           Filter length in clock ticks
    *
    * @note Not all ports support this feature
    * @note This affects the digital filter for all pins of this port
    */
   static void configureDigitalFilter(PinDigitalFilterClock pinDigitalFilterClock, int filterLength) {
      // Disable all filtering before change
      uint32_t dfer = port().DFER;
      port().DFER = 0;

      port().DFCR = pinDigitalFilterClock;
      port().DFWR = PORT_DFWR_FILT(filterLength);

      // Restore filtering on pins
      port().DFER = dfer;
   }

   /**
    * Enable digital filter on the pin
    *
    * @note Not all ports support this feature
    */
   static void enableDigitalFilter() {
         port().DFER |= BITMASK;
   }

   /**
    * Disable digital filter on the pin
    *
    * @note Not all ports support this feature
    */
   static void disableDigitalFilter() {
      port().DFER &= ~BITMASK;
   }
#endif

};

template<uint32_t portAddress, IRQn_Type irqNum>
PinCallbackFunction USBDM::PcrBase_T<portAddress, irqNum>::fCallback = PcrBase::unhandledCallback;

/**
 * @brief Template function to set a PCR to the default value
 *
 * @tparam  Last PCR to modify
 */
template<typename Last>
void processPcrs() {
   Last::setPCR();
}
/**
 * @brief Template function to set a collection of PCRs to the default value
 *
 * @tparam  Pcr1 PCR to modify
 * @tparam  Pcr2 PCR to modify
 * @tparam  Rest Remaining PCRs to modify
 */
template<typename Pcr1, typename  Pcr2, typename  ... Rest>
void processPcrs() {
   processPcrs<Pcr1>();
   processPcrs<Pcr2, Rest...>();
}
/**
 * @brief Template function to set a PCR to a given value
 *
 * @param[in]   pcrValue PCR value to set
 *
 * @tparam  Last PCR to modify
 */
template<typename Last>
void processPcrs(uint32_t pcrValue) {
   Last::setPCR(pcrValue);
}

/**
 * @brief Template function to set a collection of PCRs to a given value
 *
 * @param[in] pcrValue PCR value to set
 *
 * @tparam  Pcr1 PCR to modify
 * @tparam  Pcr2 PCR to modify
 * @tparam  Rest Remaining PCRs to modify
 */
template<typename Pcr1, typename  Pcr2, typename  ... Rest>
void processPcrs(uint32_t pcrValue) {
   processPcrs<Pcr1>(pcrValue);
   processPcrs<Pcr2, Rest...>(pcrValue);
}

/**
 * @brief Template representing a Pin Control Register (PCR)
 * Makes use of a configuration class
 *
 * Code examples:
 * @code
 * // Create PCR type
 * PcrTable_T<spiInfo, 3> SpiMOSI;
 *
 * // Configure PCR
 * SpiMOSI::setPCR(PinPull_Up,PinDriveStrength_High,PinDriveMode_PushPull,PinAction_None,PinFilter_None,PinSlewRate_Fast,PinMux_3);
 *
 * // Disable clock to associated PORT
 * SpiMOSI::disableClock();
 *
 * // Alternatively the PCR may be manipulated directly
 * Pcr_T<spiInfo, 3>::setPCR(PORT_PCR_DSE_MASK|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK);
 * @endcode
 *
 * @tparam info          Configuration class
 * @tparam index         Index of pin in configuration table
 */
template<class Info, uint8_t index> using PcrTable_T =
      Pcr_T<Info::info[index].clockInfo, Info::info[index].portAddress, Info::info[index].irqNum, Info::info[index].gpioBit, Info::info[index].pcrValue>;

/**
 * @}
 ** PeripheralPinTables
 */
} // End namespace USBDM

#endif /* HEADER_PCR_H */

