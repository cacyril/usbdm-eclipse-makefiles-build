/**
 * @file     llwu.h (180.ARM_Peripherals/Project_Headers/llwu.h)
 * @brief    Low leakage wake-up unit
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */

#ifndef HEADER_LLWU_H
#define HEADER_LLWU_H
 /*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include "hardware.h"

namespace USBDM {

/**
 * @addtogroup LLWU_Group LLWU, Low-leakage Wake-up Unit
 * @brief Abstraction for Low-leakage Wake-up Unit
 * @{
 */

/**
 * LLWU pin wake-up mode
 */
enum LlwuPinMode {
   LlwuPinMode_Disabled    = LLWU_PE1_WUPE0(0)|LLWU_PE1_WUPE1(0)|LLWU_PE1_WUPE2(0)|LLWU_PE1_WUPE3(0), //!< Wake-up by pin change disabled
   LlwuPinMode_RisingEdge  = LLWU_PE1_WUPE0(1)|LLWU_PE1_WUPE1(1)|LLWU_PE1_WUPE2(1)|LLWU_PE1_WUPE3(1), //!< Wake-up on pin rising edge
   LlwuPinMode_FallingEdge = LLWU_PE1_WUPE0(2)|LLWU_PE1_WUPE1(2)|LLWU_PE1_WUPE2(2)|LLWU_PE1_WUPE3(2), //!< Wake-up on pin falling edge
   LlwuPinMode_EitherEdge  = LLWU_PE1_WUPE0(3)|LLWU_PE1_WUPE1(3)|LLWU_PE1_WUPE2(3)|LLWU_PE1_WUPE3(3), //!< Wake-up on pin rising or falling edge
};

/**
 * LLWU peripheral wake-up mode
 */
enum LlwuPeripheralMode {
   LlwuPeripheralMode_Disabled = false, //!< Wake-up by peripheral disabled
   LlwuPeripheralMode_Enabled  = true,  //!< Wake-up by peripheral enabled
};

$(/LLWU/LlwuPins)
$(/LLWU/LlwuPeripherals)

/**
 * LLWU pin wake-up mode
 */
enum LlwuFilterPinMode {
   LlwuFilterPinMode_Disabled    = LLWU_FILT_FILTE(0), //!< Wake-up by pin change disabled
   LlwuFilterPinMode_RisingEdge  = LLWU_FILT_FILTE(1), //!< Wake-up on pin rising edge
   LlwuFilterPinMode_FallingEdge = LLWU_FILT_FILTE(2), //!< Wake-up on pin falling edge
   LlwuFilterPinMode_EitherEdge  = LLWU_FILT_FILTE(3), //!< Wake-up on pin rising or falling edge
};

#ifdef LLWU_RST_LLRSTE
/**
 * Controls Low-Leakage Mode RESET Enable
 */
enum LlwuResetWakeup {
   LlwuResetWakeup_Disabled = LLWU_RST_LLRSTE(0), //!< Wake-up by Reset disabled
   LlwuResetWakeup_Enabled  = LLWU_RST_LLRSTE(1), //!< Wake-up by Reset enabled
};

/**
 * Controls Digital Filter On RESET Pin
 */
enum LlwuResetFilter {
   LlwuResetFilter_Disabled = LLWU_RST_RSTFILT(0), //!< Reset filter disabled
   LlwuResetFilter_Enabled  = LLWU_RST_RSTFILT(1), //!< Reset filter enabled
};
#endif

/**
 * Type definition for LLWU interrupt call back
 */
typedef void (*LLWUCallbackFunction)();

/**
 * Template class providing interface to Low Leakage Wake-up Unit
 *
 * @tparam info      Information class for LLWU
 *
 * @code
 * using llwu = LlwuBase_T<LlwuInfo>;
 *
 *  llwu::configure();
 *
 * @endcode
 */
template <class Info>
class LlwuBase_T {

protected:
   /** Class to static check channel pin mapping is valid */
   template<LlwuPin llwuPin> class CheckSignal {
      static_assert((llwuPin<Info::numSignals), "Non-existent LLWU Input - Modify Configure.usbdm");
      static_assert((llwuPin>=Info::numSignals)||(Info::info[llwuPin].gpioBit != UNMAPPED_PCR), "LLWU Input is not mapped to a pin - Modify Configure.usbdm");
      static_assert((llwuPin>=Info::numSignals)||(Info::info[llwuPin].gpioBit != INVALID_PCR),  "LLWU Input doesn't exist in this device/package - Modify Configure.usbdm");
      static_assert((llwuPin>=Info::numSignals)||((Info::info[llwuPin].gpioBit == UNMAPPED_PCR)||(Info::info[llwuPin].gpioBit == INVALID_PCR)||(Info::info[llwuPin].gpioBit >= 0)), "Illegal LLWU Input - Modify Configure.usbdm");
   public:
      /** Dummy function to allow convenient in-line checking */
      static constexpr void check() {}
   };

   /** Callback function for ISR */
   static LLWUCallbackFunction callback;

public:
   /**
    * IRQ handler
    */
   static void irqHandler(void) {
      if (callback != 0) {
         callback();
      }
      else {
         setAndCheckErrorCode(E_NO_HANDLER);
      }
   }

   /**
    * Set Callback function
    *
    *   @param[in]  theCallback - Callback function to be executed on LLWU interrupt
    */
   static void setCallback(LLWUCallbackFunction theCallback) {
      callback = theCallback;
   }

protected:
   /** Pointer to hardware */
   static __attribute__((always_inline)) volatile LLWU_Type &llwu() { return Info::llwu(); }

public:

   /**
    * Configures all mapped pins associated with this peripheral
    */
   static void configureAllPins() {
      // Configure pins
      Info::initPCRs();
   }

   /**
    * Basic enable of LLWU\n
    * Includes configuring all pins
    */
   static void enable() {
      configureAllPins();
   }

   /**
    * Configure with settings from Configure.usbdmProject.
    */
   static void defaultConfigure() {

      // Configure pins
      Info::initPCRs();

      llwu().PE1   = Info::pe1;
      llwu().PE2   = Info::pe2;
      llwu().PE3   = Info::pe3;
      llwu().PE4   = Info::pe4;

      llwu().ME    = Info::me;

      llwu().FILT1 = Info::filt1|LLWU_FILT_FILTF_MASK;
      llwu().FILT2 = Info::filt2|LLWU_FILT_FILTF_MASK;

#ifdef LLWU_RST_LLRSTE
      llwu().RST   = Info::rst;
#endif

      enableNvicInterrupts();
   }

   /*
    * ***************************************************
    * Wake-up pins
    * ***************************************************
    */
   /**
    * Configure pin as wake-up source
    *
    * @param[in] llwuPin       Pin to configure
    * @param[in] llwuPinMode   Mode for pin as wake-up input
    */
   static void configurePinSource(
         LlwuPin     llwuPin,
         LlwuPinMode llwuPinMode) {

      static const uint8_t masks[] = {(0x3<<0),(0x3<<2),(0x3<<4),(0x3<<6)};
      volatile uint8_t &llwuPe = llwu().PE[llwuPin>>2];
      uint8_t mask = masks[llwuPin&3];
      llwuPe = (llwuPe&~mask) | (llwuPinMode&mask);
   }

   /**
    * Get bit mask indicating wake-up pin sources\n
    * The pin masks correspond to Pin sources.
    *
    * @code
    *    if ((pinWakeupSource&(1<<LlwuPin_ptc1)) != 0) {
    *       // Wakeup from LPTMR
    *    }
    * @endcode
    *
    * @return Bit mask
    */
   static uint32_t getPinWakeupSources() {
      return (llwu().PF[1]<<8)|llwu().PF[0];
   }

   /**
    * Check if pin is source of wake-up
    *
    * @param[in] llwuPin  Pin to check
    *
    * @return false Given pin is not source of wake-up.
    * @return true  Given pin is source of wake-up.
    */
   static bool isPinWakeupSource(LlwuPin llwuPin) {
      return getPinWakeupSources() & (1<<llwuPin);
   }

   /**
    * Clear wake-up pin flag
    *
    *  @param[in] llwuPin Pin indicating which flag to clear
    */
   static void clearPinWakeupFlag(LlwuPin llwuPin) {
      llwu().PF[llwuPin>>3] = (1<<(llwuPin&0x7));
   }

   /**
    * Clear all wake-up flags\n
    * Peripherals sources are not cleared since they are controlled by the peripheral
    */
   static void clearAllFlags() {
      clearPinWakeupFlags();
      clearFilteredPinWakeupFlags();
   }

   /**
    * Clear all wake-up pin flags
    */
   static void clearPinWakeupFlags() {
      llwu().PF[0] = 0xFF;
      llwu().PF[1] = 0xFF;
      if (sizeof(llwu().PF) > 2) {
         llwu().PF[2] = 0xFF;
      }
      if (sizeof(llwu().PF) > 3) {
         llwu().PF[3] = 0xFF;
      }
   }

   /*
    * ***************************************************
    * Wake-up filtered pins
    * ***************************************************
    */
   /**
    * Configure one of the input pins as a filtered wake-up source
    *
    * @param[in] filterNum          Filter to configure - number available depends on device
    * @param[in] llwuPin            Pin to assign to filter
    * @param[in] llwuFilterPinMode  Mode for pin as wake-up input
    *
    * @note Filtering is bypassed in VLLS0
    */
   static ErrorCode configureFilteredPinSource(
         unsigned          filterNum,
         LlwuPin           llwuPin,
         LlwuFilterPinMode llwuFilterPinMode) {

      if (filterNum>(sizeof(llwu().FILT)/sizeof(llwu().FILT[0]))) {
         return setErrorCode(E_ILLEGAL_PARAM);
      }
      llwu().FILT[filterNum] = llwuPin|llwuFilterPinMode;
      return E_NO_ERROR;
   }

   /**
    * Check if filtered wake-up pin is source of wake-up
    *
    * @param[in] filterNum Pin Filter to check
    *
    * @return false Given filtered pin is not source of wake-up.
    * @return true  Given filtered pin is source of wake-up.
    */
   static bool isFilteredPinWakeupSource(unsigned filterNum) {
      return (llwu().FILT[filterNum] & LLWU_FILT_FILTF_MASK);
   }

   /**
    * Clear filtered wake-up pin flag
    *
    * @param[in] filterNum Pin Filter to clear flag
    */
   static void clearFilteredPinWakeupFlag(unsigned filterNum) {
      llwu().FILT[filterNum] |= LLWU_FILT_FILTF_MASK;
   }

   /**
    * Clear all filtered wake-up pin flags
    */
   static void clearFilteredPinWakeupFlags() {
      llwu().FILT[0] |= LLWU_FILT_FILTF_MASK;
      if (sizeof(llwu().FILT)/sizeof(llwu().FILT[0]) > 1) {
         llwu().FILT[1] |= LLWU_FILT_FILTF_MASK;
      }
      if (sizeof(llwu().FILT)/sizeof(llwu().FILT[0]) > 2) {
         llwu().FILT[2] |= LLWU_FILT_FILTF_MASK;
      }
      if (sizeof(llwu().FILT)/sizeof(llwu().FILT[0]) > 3) {
         llwu().FILT[3] |= LLWU_FILT_FILTF_MASK;
      }
   }

#ifdef LLWU_RST_LLRSTE
   /**
    * Controls Reset wake-up control
    *
    * @param llwuResetFilter  Whether filtering is applied to reset pin
    * @param llwuResetWakeup  Whether reset is enabled as a wake-up source
    */
   static void configureResetFilter(LlwuResetFilter llwuResetFilter, LlwuResetWakeup llwuResetWakeup=LlwuResetWakeup_Enabled) {
      llwu().RST = llwuResetFilter|llwuResetWakeup;
   }
#endif

   /*
    * ***************************************************
    * Wake-up peripherals
    * ***************************************************
    */
   /**
    * Configure peripheral as wake-up source
    *
    * @param[in] llwuPeripheral     Peripheral to configure
    * @param[in] llwuPeripheralMode Whether to enable peripheral as wake-up source
    */
   static void configurePeripheralSource(
         LlwuPeripheral       llwuPeripheral,
         LlwuPeripheralMode   llwuPeripheralMode=LlwuPeripheralMode_Enabled) {

      if (llwuPeripheralMode) {
         llwu().ME |= llwuPeripheral;
      }
      else {
         llwu().ME &= (uint8_t)~llwuPeripheral;
      }
   }

   /**
    * Disable all wake-up sources (pins and peripherals)
    */
   static void disableAllSources() {
      llwu().PE[1] = 0;
      llwu().PE[2] = 0;
      if ((sizeof(llwu().PE)/sizeof(llwu().PE[0]))>=3) {
         llwu().PE[3] = 0;
      }
      if ((sizeof(llwu().PE)/sizeof(llwu().PE[0]))>=4) {
         llwu().PE[4] = 0;
      }
      llwu().ME  = 0;
   }
   /**
    * Get flag bit mask indicating wake-up peripheral sources\n
    * The mask returned correspond to (multiple) peripheral sources.\n
    * These flags are cleared through the originating peripheral.
    *
    *
    * Example checking source
    * @code
    *    if ((peripheralWakeupSource&LlwuPeripheral_Lptmr) != 0) {
    *       // Wakeup from LPTMR
    *    }
    * @endcode
    *
    * @return Bit mask
    */
   static uint32_t getPeripheralWakeupSources() {
      return llwu().MF;
   }

   /**
    *  Check if peripheral is source of wake-up\n
    *  These flags are cleared through the originating peripheral.
    *
    * @param[in] llwuPeripheral  Peripheral to check
    *
    * @return false Given peripheral is not source of wake-up.
    * @return true  Given peripheral is source of wake-up.
    */
   static bool isPeripheralWakeupSource(LlwuPeripheral llwuPeripheral) {
      return llwu().MF & llwuPeripheral;
   }

   /**
    * Enable/disable interrupts in NVIC
    *
    * @param[in]  enable        True => enable, False => disable
    * @param[in]  nvicPriority  Interrupt priority
    */
   static void enableNvicInterrupts(bool enable=true, uint32_t nvicPriority=NvicPriority_Normal) {

      if (enable) {
         enableNvicInterrupt(Info::irqNums[0], nvicPriority);
      }
      else {
         // Disable interrupts
         NVIC_DisableIRQ(Info::irqNums[0]);
      }
   }

   template<LlwuPin llwuPin>
   class Pin {

   private:
      // Checks pin mapping is valid
      LlwuBase_T::CheckSignal<llwuPin> check;

      using Pcr = PcrTable_T<Info, llwuPin>;

   public:
      static constexpr LlwuPin pin = llwuPin;

      /**
       * Set Pin Control Register (PCR) value \n
       * This will map the pin to the LLWU function (mux value) \n
       * The clock to the port will be enabled before changing the PCR
       *
       * @tparam llwuPin LLWU pin to configure e.g. LlwuPin_Pte1
       *
       * @param[in]  pinPull          One of PinPull_None, PinPull_Up, PinPull_Down
       * @param[in]  pinAction        One of PinAction_None, etc (defaults to PinAction_None)
       * @param[in]  pinFilter        One of PinFilter_None, PinFilter_Passive (defaults to PinFilter_None)
       */
      static void setInput(
            PinPull           pinPull           = PinPull_None,
            PinAction         pinAction         = PinAction_None,
            PinFilter         pinFilter         = PinFilter_None
            ) {
         Pcr::setPCR(pinPull|pinAction|pinFilter|(Info::info[llwuPin].pcrValue&PORT_PCR_MUX_MASK));
      }

      /**
       * Clear pin interrupt flag.
       * Assumes clock to the port has already been enabled.
       */
      static void clearInterruptFlag() {
         Pcr::clearInterruptFlag();
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
      static void setCallback(PinCallbackFunction pinCallback) {
         Pcr::setCallback(pinCallback);
      }

      /**
       * Enable/disable Pin interrupts in NVIC.
       * Any pending NVIC interrupts are first cleared.
       *
       * @param[in]  enable        True => enable, False => disable
       * @param[in]  nvicPriority  Interrupt priority
       */
      static void enableNvicInterrupts(bool enable=true, uint32_t nvicPriority=NvicPriority_Normal) {
         Pcr::enableNvicInterrupts(enable, nvicPriority);
      }

   };
};

template<class Info> LLWUCallbackFunction LlwuBase_T<Info>::callback = 0;

#ifdef USBDM_LLWU_IS_DEFINED
/**
 * Class representing LLWU
 */
class Llwu : public LlwuBase_T<LlwuInfo> {};

#endif

/**
 * End LLWU_Group
 * @}
 */

} // End namespace USBDM

#endif /* HEADER_LLWU_H */
