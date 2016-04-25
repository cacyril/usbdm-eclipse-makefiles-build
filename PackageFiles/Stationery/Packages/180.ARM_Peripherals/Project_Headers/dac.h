/**
 * @file     dac.h
 * @brief    Voltage Reference Pin routines
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */

#ifndef PROJECT_HEADERS_DAC_H_
#define PROJECT_HEADERS_DAC_H_

#include "derivative.h"
#include "pin_mapping.h"

namespace USBDM {
/**
 * @addtogroup DAC_Group Voltage reference
 * @{
 */
/**
 * Template class representing a Voltage Reference
 *
 * @tparam info      Information class for DAC
 *
 * @code
 * using vref = Dac_T<VrefInfo>;
 *
 *  vref::initialise();
 *
 * @endcode
 */
template<class Info>
class Dac_T {

private:
   static constexpr volatile DAC_Type *dac      = reinterpret_cast<volatile DAC_Type *>(Info::basePtr);
   static constexpr volatile uint32_t *clockReg = reinterpret_cast<volatile uint32_t *>(Info::clockReg);

#ifdef DEBUG_BUILD
   // ToDO
//   static_assert((Info::info[0].gpioBit != UNMAPPED_PCR), "Dac_T: DAC signal is not mapped to a pin - Modify Configure.usbdm");
//   static_assert((Info::info[0].gpioBit != INVALID_PCR),  "Dac_T: Non-existent signal used for DAC ouput");
//   static_assert((Info::info[0].gpioBit == UNMAPPED_PCR)||(Info::info[0].gpioBit == INVALID_PCR)||(Info::info[0].gpioBit >= 0), "Dac_T: Illegal signal used for DAC");
#endif

public:
   /**
    * Enable the DAC
    *
    * @param scValue Value for C0 register e.g. DAC_C0_DACEN_MASK|DAC_C0_DACRFS_MASK
    */
   static void initialise(uint32_t mode=DAC_C0_DACEN_MASK|DAC_C0_DACRFS_MASK) {
      // Configure pin (if necessary)
      // ToDo Implement
//      Info::initPCRs();

      // Enable clock to DAC interface
      *clockReg |= Info::clockMask;

      // Initialise hardware
      dac->C0 = mode;
      dac->C1 = 0;
   }

   /**
    * Set DAC output value
    */
   static void setValue(uint16_t value) {
      dac->DATA[0] = value;
   }
};

#if defined(USBDM_DAC0_IS_DEFINED)
using Dac0 = Dac_T<Dac0Info>;
#endif

#if defined(USBDM_DAC1_IS_DEFINED)
using Dac1 = Dac_T<Dac1Info>;
#endif
/**
 * @}
 */
} // End namespace USBDM

#endif /* PROJECT_HEADERS_DAC_H_ */
