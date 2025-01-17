/*
 * @file spi.cpp  (180.ARM_Peripherals/Sources/spi_16bit-MKL.cpp)
 *
 *  Created on: 07/08/2012
 *      Author: podonoghue
 */
#include <stddef.h>
#include <stdint.h>
#include "system.h"
#include "derivative.h"
#include "utilities.h"
#include "hardware.h"
#include "spi.h"
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
namespace USBDM {

static const uint16_t spprFactors[] = {1,2,3,4,5,6,7,8};
static const uint16_t sprFactors[]  = {2,4,8,16,32,64,128,256,512};

/**
 * Calculate communication BR value for SPI
 *
 * @param[in]  clockFrequency => Clock frequency of SPI in Hz
 * @param[in]  frequency      => Communication frequency in Hz
 *
 * @return BR register value
 *
 * Note: Chooses the highest speed that is not greater than frequency.
 */
uint8_t Spi::calculateBr(uint32_t clockFrequency, uint32_t frequency) {

   int bestSPPR = 0;
   int bestSPR  = 0;
   int32_t bestDifference = 0x7FFFFFFF;
   for (int sppr = (sizeof(spprFactors)/sizeof(spprFactors[0]))-1; sppr >= 0; sppr--) {
      for (int spr = (sizeof(sprFactors)/sizeof(sprFactors[0]))-1; spr >= 0; spr--) {
         uint32_t calculatedFrequency = clockFrequency/(spprFactors[sppr]*sprFactors[spr]);
         int32_t difference = frequency-calculatedFrequency;
         if (difference < 0) {
            // Too high stop looking here
            break;
         }
         if (difference < bestDifference) {
            // New "best value"
            bestDifference = difference;
            bestSPR  = spr;
            bestSPPR = sppr;
         }
      }
   }
   return SPI_BR_SPPR(bestSPPR)|SPI_BR_SPR(bestSPR);
}

/**
 * Calculate communication BR value for SPI
 *
 * @param[in]  clockFrequency => Clock frequency of SPI in Hz
 * @param[in]  frequency      => Communication frequency in Hz
 *
 * @return BR register value
 *
 * Note: Chooses the highest speed that is not greater than frequency.
 */
uint32_t Spi::calculateSpeed(uint32_t clockFrequency, uint32_t br) {
   uint32_t sppr = (br&SPI_BR_SPPR_MASK)>>SPI_BR_SPPR_SHIFT;
   uint32_t spr  = (br&SPI_BR_SPR_MASK)>>SPI_BR_SPR_SHIFT;
   return clockFrequency/(spprFactors[sppr]*sprFactors[spr]);
}

} // End namespace USBDM
