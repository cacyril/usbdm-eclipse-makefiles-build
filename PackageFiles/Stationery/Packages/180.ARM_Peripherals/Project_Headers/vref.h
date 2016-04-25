/**
 * @file     vref.h
 * @brief    Voltage Reference Pin routines
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */

#ifndef PROJECT_HEADERS_VREF_H_
#define PROJECT_HEADERS_VREF_H_

#include "derivative.h"
#include "pin_mapping.h"

namespace USBDM {
/**
 * @addtogroup VREF_Group Voltage reference
 * @{
 */
/**
 * Template class representing a Voltage Reference
 *
 * @tparam info      Information class for VREF
 *
 * @code
 * using vref = Vref_T<VrefInfo>;
 *
 *  vref::initialise();
 *
 * @endcode
 */
template<class Info>
class Vref_T {

private:
   static constexpr volatile VREF_Type *vref      = reinterpret_cast<volatile VREF_Type *>(Info::basePtr);
   static constexpr volatile uint32_t *clockReg = reinterpret_cast<volatile uint32_t *>(Info::clockReg);

#ifdef DEBUG_BUILD
   static_assert((Info::info[0].gpioBit != UNMAPPED_PCR), "Vref_T: Vref signal is not mapped to a pin - Modify Configure.usbdm");
   static_assert((Info::info[0].gpioBit != INVALID_PCR),  "Vref_T: Non-existent signal used for Vref input");
   static_assert((Info::info[0].gpioBit == UNMAPPED_PCR)||(Info::info[0].gpioBit == INVALID_PCR)||(Info::info[0].gpioBit >= 0), "Vref_T: Illegal signal used for Vref");
#endif

public:
   /**
    * Enable the voltage reference
    *
    * @param scValue Value for SC register e.g. VREF_SC_VREFEN_MASK|VREF_SC_REGEN_MASK|VREF_SC_ICOMPEN_MASK|VREF_SC_MODE_LV(2)
    */
   static void initialise(uint32_t mode=VREF_SC_VREFEN_MASK|VREF_SC_REGEN_MASK|VREF_SC_ICOMPEN_MASK|VREF_SC_MODE_LV(2)) {
      // Configure pin (if necessary)
      Info::initPCRs();

      // Enable clock to VREF interface
      *clockReg |= Info::clockMask;

      // Initialise hardware
      vref->TRM |= VREF_TRM_CHOPEN_MASK;
      vref->SC   = mode;
      while ((vref->SC & VREF_SC_VREFST_MASK) == 0) {
         // Wait until stable
      }
   }
   static void disable() {
      vref->SC = 0;
      *clockReg &= ~Info::clockMask;
   }
};

#if defined(USBDM_VREF_IS_DEFINED)
using Vref = Vref_T<VrefInfo>;
#endif

#if defined(USBDM_VREF0_IS_DEFINED)
using Vref0 = Vref<Vref0Info>;
#endif
/**
 * @}
 */
} // End namespace USBDM

#endif /* PROJECT_HEADERS_VREF_H_ */
