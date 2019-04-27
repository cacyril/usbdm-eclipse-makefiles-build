/**
 * @file     lpi2c.h (180.ARM_Peripherals/Project_Headers/lpi2c.h)
 * @brief    LPI2C interface
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */
#ifndef HEADER_LPI2C_H
#define HEADER_LPI2C_H
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include <stdint.h>
#include "derivative.h"
#include "hardware.h"
#ifdef __CMSIS_RTOS
#include "cmsis.h"
#endif

namespace USBDM {

/**
 * @addtogroup LPI2C_Group LPI2C, Inter-Integrated-Circuit Interface
 * @brief Abstraction for Inter-Integrated-Circuit Interface
 * @{
 */

/**
 * Type definition for interrupt call back
 * @param status Interrupt status value from SPI->SR
 */
typedef void (*Lpi2cCallbackFunction)();

/**
 * Virtual Base class for LPI2C interface
 */
class Lpi2c {

public:

protected:
   /** Callback to catch unhandled interrupt */
   static void unhandledCallback() {
      // Not considered an error as may be using polling
   }

   volatile LPI2C_Type  *lpi2c;               //!< LPI2C hardware instance

   /**
    * Construct LPI2C interface
    *
    * @param[in]  lpi2c     Base address of LPI2C hardware
    */
   Lpi2c(volatile LPI2C_Type *lpi2c) : lpi2c(lpi2c) {
   }

   /**
    * Destructor
    */
   ~Lpi2c() {}
};

template<class Info> class Lpi2cBase_T : public Lpi2c {

public:
   // Handle on LPI2C hardware
   static constexpr volatile LPI2C_Type *LPI2C = Info::lpi2c;

   // LPI2C SCL (clock) Pin
   using sclGpio = GpioTable_T<Info, 0, USBDM::ActiveLow>; // Inactive is high

   // LPI2C SDA (data) Pin
   using sdaGpio = GpioTable_T<Info, 1, USBDM::ActiveHigh>;

   /** Used by ISR to obtain handle of object */
   static Lpi2c *thisPtr;

   /** Callback function for ISR */
   static Lpi2cCallbackFunction sCallback;

   /**
    * Enable interrupts in NVIC
    * Any pending NVIC interrupts are first cleared.
    */
   static void enableNvicInterrupts() {
      enableNvicInterrupt(Info::irqNums[0]);
   }

   /**
    * Enable and set priority of interrupts in NVIC
    * Any pending NVIC interrupts are first cleared.
    *
    * @param[in]  nvicPriority  Interrupt priority
    */
   static void enableNvicInterrupts(uint32_t nvicPriority) {
      enableNvicInterrupt(Info::irqNums[0], nvicPriority);
   }

   /**
    * Disable interrupts in NVIC
    */
   static void disableNvicInterrupts() {
      NVIC_DisableIRQ(Info::irqNums[0]);
   }

public:
   /**
    * Configures all mapped pins associated with this peripheral
    */
   static void __attribute__((always_inline)) configureAllPins() {
      // Configure pins
      Info::initPCRs();
   }

   /**
    * Construct LPI2C interface
    *
    * @param[in]  bps        Tx/Rx rate
    */
   Lpi2cBase_T(unsigned bps=400000) : Lpi2c(&Info::lpi2c()) {
   }

   /**
    * Destructor
    */
   virtual ~Lpi2cBase_T() {}

   /**
    * Set channel Callback function\n
    * This callback is executed when the LPI2C state machine returns to the IDLE state
    * at the end of a transaction.
    *
    * @param[in] callback Callback function to execute on interrupt.\n
    *                     Use nullptr to remove callback.
    */
   static __attribute__((always_inline)) void setCallback(Lpi2cCallbackFunction callback) {
      usbdm_assert(Info::irqHandlerInstalled, "LPI2C not configured for interrupts");
      if (callback == nullptr) {
         callback = Lpi2c::unhandledCallback;
      }
      sCallback = callback;
   }

   static void irqMasterHandler() {
      sCallback();
   }

   static void irqSlaveHandler() {
      sCallback();
   }
};

template<class Info> Lpi2cCallbackFunction Lpi2cBase_T<Info>::sCallback = Lpi2c::unhandledCallback;

/** Used by ISR to obtain handle of object */
template<class Info> Lpi2c *Lpi2cBase_T<Info>::thisPtr = 0;

#if defined(USBDM_LPI2C0_IS_DEFINED)
/**
 * @brief Class representing the LPI2C0 interface
 *
 * <b>Example</b>\n
 * Refer @ref Lpi2cBase_T
 */
using Lpi2c0 = Lpi2cBase_T<Lpi2c0Info>;
#endif

#if defined(USBDM_LPI2C1_IS_DEFINED)
/**
 * @brief Class representing the LPI2C1 interface
 *
 * <b>Example</b>
 * Refer @ref Lpi2cBase_T
 */
using Lpi2c1 = Lpi2cBase_T<Lpi2c1Info>;
#endif

#if defined(USBDM_LPI2C2_IS_DEFINED)
/**
 * @brief Class representing the LPI2C2 interface
 *
 * <b>Example</b>
 * Refer @ref Lpi2cBase_T
 */
using Lpi2c2 = Lpi2cBase_T<Lpi2c2Info>;
#endif

/**
 * End LPI2C_Group
 * @}
 */

} // End namespace USBDM

#endif /* HEADER_LPI2C_H */