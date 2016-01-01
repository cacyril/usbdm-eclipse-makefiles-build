/*
 * spi.cpp (derived from spi-MKL.cpp)
 *
 *  Created on: 07/08/2012
 *      Author: podonoghue
 */
#include <stddef.h>
#include <stdint.h>
#include "system.h"
#include "derivative.h"
#include "utilities.h"
#include "spi.h"
#include "gpio.h"

namespace USBDM {

#if 0 && defined(SPI0)
#if (SPI0_SCK_PIN_SEL==0) || (SPI0_MOSI_PIN_SEL==0) || (SPI0_MISO_PIN_SEL==0)
#warning "SPI0 unavailable - Please check pin mappings for SCK, SIN & SOUT in pin_mapping.h"
#else

/**
 * Constructor
 *
 * @param dmaTxChannel DMA Channel for transmission
 * @param dmaRxChannel DMA Channel for reception
 * @param pcs          Manual PCS select signal
 */
Spi0::Spi0(DMAChannel *dmaTxChannel, DMAChannel *dmaRxChannel) :
      Spi(SPI0, dmaTxChannel, dmaRxChannel, DMAChannel::DMA_SLOT_SPI0_Rx) {

   thisPtr = this;

   // Enable SPI port pin clocks
   // MOSI,MISO,SCLK
   SPI0_SCK_GPIO::Pcr::setPCR(PORT_PCR_MUX(SPI0_SCK_FN)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK);
   SPI0_MOSI_GPIO::Pcr::setPCR(PORT_PCR_MUX(SPI0_MOSI_FN)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK);
   SPI0_MISO_GPIO::Pcr::setPCR(PORT_PCR_MUX(SPI0_MISO_FN)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK);

#ifdef SPI0_PCS0_GPIO
   SPI0_PCS0_GPIO::Pcr::setPCR(PORT_PCR_MUX(SPI0_PCS0_FN)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK);
#define SPI0_C1_SSOE_VALUE SPI_C1_SSOE_MASK
#endif
#ifndef SPI0_C1_SSOE_VALUE
#define SPI0_C1_SSOE_VALUE 0
#endif

   // Enable SPI module clock
   SIM->SPI0_CLOCK_REG |= SPI0_CLOCK_MASK;

   setSpeed(0); // default baud

   NVIC_EnableIRQ(SPI0_IRQn);
   NVIC_SetPriority (SPI0_IRQn, 2);

   spi->C2 = SPI_C2_MODFEN_MASK|SPI_C2_TXDMAE_MASK|SPI_C2_RXDMAE_MASK;
   spi->C1 = SPI_C1_SPE_MASK|SPI_C1_MSTR_MASK|SPI0_C1_SSOE_VALUE;//|SPI_C1_CPHA_MASK|SPI_C1_CPOL_MASK;
}

Spi0 *Spi0::thisPtr = NULL;

extern "C"
void SPI0_IRQHandler(void) {
   Spi0::thisPtr->poll();
}

#endif // (SPI0_SCK_PIN_SEL==0) || !defined(SPI0_MOSI_GPIO) || !defined(SPI0_MISO_GPIO)
#endif // SPI0


static const uint16_t spprFactors[] = {1,2,3,4,5,6,7,8};
static const uint16_t sprFactors[]  = {2,4,8,16,32,64,128,256,512};

/**
 * Sets Communication speed for SPI
 *
 * @param frequency => Frequency in Hz (0 => use default value)
 *
 * Note: Chooses the highest speed that is not greater than frequency.
 */
void Spi::setSpeed(uint32_t targetFrequency) {

   int bestSPPR = 0;
   int bestSPR  = 0;
   uint32_t difference = -1;
   for (int sppr = (sizeof(spprFactors)/sizeof(spprFactors[0]))-1; sppr >= 0; sppr--) {
      for (int spr = (sizeof(sprFactors)/sizeof(sprFactors[0]))-1; spr >= 0; spr--) {
         uint32_t calculatedFrequency = SystemBusClock/(spprFactors[sppr]*sprFactors[spr]);
         if (calculatedFrequency > targetFrequency) {
            // Too high stop looking here
            break;
         }
         if ((targetFrequency - calculatedFrequency) < difference) {
            // New "best value"
            difference = (targetFrequency - calculatedFrequency);
            bestSPR  = spr;
            bestSPPR = sppr;
            interfaceFrequency = calculatedFrequency;
         }
      }
   }
   spi->BR = SPI_BR_SPPR(bestSPPR)|SPI_BR_SPR(bestSPR);
}

/**
 * Transmit and receive an 8-bit value over SPI
 *
 * @param data - Data to send
 *
 * @return Data received
 */
uint32_t Spi::txRx(uint32_t data) {
   while ((spi->S & SPI_S_SPTEF_MASK)==0) {
      __asm__("nop");
   }
   spi->D  = data;
   while ((spi->S & SPI_S_SPRF_MASK)==0) {
      __asm__("nop");
   }
   return spi->D; // Return read data
}

/**
 *  Transmit and receive a series of bytes
 *
 *  @param dataSize  Number of bytes to transfer
 *  @param dataOut   Transmit bytes (may be NULL for Rx only)
 *  @param dataIn    Receive byte buffer (may be NULL for Tx only)
 *
 *  Note: dataIn may use same buffer as dataOut
 */
void Spi::txRxBytes(uint32_t dataSize, const uint8_t *dataOut, uint8_t *dataIn) {
   uint8_t dummy = 0xFF;
   uint32_t dataInInc  = DMA_DCR_DINC_MASK|DMA_DCR_ERQ_MASK|DMA_DCR_CS_MASK|DMA_DCR_SSIZE(1)|DMA_DCR_DSIZE(1)|DMA_DCR_D_REQ_MASK;
   uint32_t dataOutInc = DMA_DCR_SINC_MASK|DMA_DCR_ERQ_MASK|DMA_DCR_CS_MASK|DMA_DCR_SSIZE(1)|DMA_DCR_DSIZE(1)|DMA_DCR_D_REQ_MASK;

   if (dataIn == 0) {
      dataIn = &dummy;
      dataInInc &= ~DMA_DCR_DINC_MASK;
   }
   if (dataOut == 0) {
      dataOut = &dummy;
      dataOutInc &= ~DMA_DCR_SINC_MASK;
   }
   // Optional Rx channel
   DMAChannel::DMAInformation dmaRxInformation = {
      (uint32_t)&spi->D,
      (uint32_t)dataIn,
      dataSize,
      dataInInc,
      (uint8_t)dmaSpiRxSlot,
   };
   dmacRxChannel->configure(&dmaRxInformation);

   // Must have Tx channel
   DMAChannel::DMAInformation dmaTxInformation = {
      (uint32_t)dataOut,
      (uint32_t)&spi->D,
      dataSize,
      dataOutInc,
      (uint8_t)(dmaSpiRxSlot+1),
   };
   dmacTxChannel->configure(&dmaTxInformation);

   spi->C2 |= SPI_C2_RXDMAE_MASK|SPI_C2_TXDMAE_MASK;
   dmacTxChannel->wait();
   dmacRxChannel->wait();
   spi->C2 &= ~(SPI_C2_TXDMAE_MASK|SPI_C2_RXDMAE_MASK);
}

} // End namespace USBDM
