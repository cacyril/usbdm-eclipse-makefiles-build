/**
 * @file     vref.h (180.ARM_Peripherals/Project_Headers/vref.h)
 * @brief    Voltage Reference
 *
 * @version  V4.12.1.210
 * @date     13 April 2016
 *      Author: podonoghue
 */

#ifndef HEADER_VREF_H
#define HEADER_VREF_H
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include "derivative.h"
#include "hardware.h"

namespace USBDM {

/**
 * @addtogroup VREF_Group VREF, Voltage Reference
 * @brief C++ Class allowing access to Voltage Reference
 * @{
 */

/**
 * Template class representing a Voltage Reference
 *
 * @tparam info      Information class for VREF
 *
 * @code
 * using vref = VrefBase_T<VrefInfo>;
 *
 *  vref::initialise();
 *
 * @endcode
 */
template<class Info>
class VrefBase_T {

private:
   static constexpr volatile VREF_Type *vref    = Info::vref;
   static constexpr volatile uint32_t *clockReg = Info::clockReg;

#ifdef DEBUG_BUILD
   static_assert((Info::info[0].gpioBit != UNMAPPED_PCR), "VrefBase_T: Vref signal is not mapped to a pin - Modify Configure.usbdm");
   static_assert((Info::info[0].gpioBit != INVALID_PCR),  "VrefBase_T: Non-existent signal used for Vref input");
   static_assert((Info::info[0].gpioBit == UNMAPPED_PCR)||(Info::info[0].gpioBit == INVALID_PCR)||(Info::info[0].gpioBit >= 0), "VrefBase_T: Illegal signal used for Vref");
#endif

public:
   /**
    * Configures all mapped pins associated with this peripheral
    */
   static void __attribute__((always_inline)) configureAllPins() {
      // Configure pins
      Info::initPCRs();
   }

   /**
    * Basic enable of VREF\n
    * Includes configuring all pins
    */
   static void enable() {
      configureAllPins();

      // Enable clock to VREF interface
      *clockReg |= Info::clockMask;
   }

   /**
    * Enable the voltage reference with default settings
    */
   static void defaultConfigure() {
      enable();

      // Initialise hardware
      vref->TRM  = Info::vref_trm;
      vref->SC   = Info::vref_sc|VREF_SC_VREFEN_MASK;

      while ((vref->SC & VREF_SC_VREFST_MASK) == 0) {
         // Wait until stable
      }
   }

   /**
    * Configures the voltage reference
    */
   static void configure() {
      defaultConfigure();
   }

   /**
    * Sets the voltage reference mode
    *
    * @param scValue Value for SC register e.g. VREF_SC_VREFEN_MASK|VREF_SC_REGEN_MASK|VREF_SC_ICOMPEN_MASK|VREF_SC_MO`DE_LV(2)
    */
   static void setMode(uint32_t scValue=Info::vref_sc|VREF_SC_VREFEN_MASK) {
      vref->SC   = scValue;
   }

   /**
    * Disable Vref
    */
   static void disable() {
      vref->SC = 0;
      *clockReg &= ~Info::clockMask;
   }
};

#if defined(USBDM_VREF_IS_DEFINED)
class Vref : public VrefBase_T<VrefInfo> {};
#endif

#if defined(USBDM_VREF0_IS_DEFINED)
class Vref0 : public Vref<Vref0Info> {};
#endif
/**
 * End TSI_Group
 * @}
 */

} // End namespace USBDM

#endif /* HEADER_VREF_H */
