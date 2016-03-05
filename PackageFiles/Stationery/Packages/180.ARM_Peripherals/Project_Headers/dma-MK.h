/*
 * dma.h (derived from dma-MKL.h)
 *
 *  Created on: 08/07/2014
 *      Author: podonoghue
 */

#ifndef INCLUDE_USBDM_DMA_H_
#define INCLUDE_USBDM_DMA_H_

#include "derivative.h"
#include "gpio.h"

namespace USBDM {

template<uint channel, uint source>
class DmaMux_T {
public:
   static void enable() {
   }
   static void disable() {
      DMAMUX0->CHCFG[channel] = 0;
   }
};

/**
 * Peripheral pin mapping information for UART, Universal Asynchronous Receiver/Transmitter
 */
class DmaInfo {

public:
   //! Hardware base pointer
   static constexpr uint32_t basePtr   = DMA0_BasePtr;

   //! Clock mask for peripheral
   static constexpr uint32_t clockMask = SIM_SCGC7_DMA_MASK;

   //! Address of clock register for peripheral
   static constexpr uint32_t clockReg  = SIM_BasePtr+offsetof(SIM_Type,SCGC7);
};

/**
 * @addtogroup DMA_Group Direct Memory Access (DMA)
 * @brief Support for DMA operations
 * @{
 */
class DmaChannel {
public:
   enum {
      size8bit    = 0,
      size16bit   = 1,
      size32bit   = 2,
      size16byte  = 4,
      size32byte  = 5,
   };
   static constexpr uint8_t getAttrSize(uint size) {
      return
         (size==1)?0:
         (size==2)?1:
         (size==4)?2:
         (size==16)?4:
         (size==32)?5:7;
   }
   struct SingleTransferInfo {
      const volatile void *sourceAddress;          // Source Address
      volatile void       *destinationAddress;     // Destination Address
      uint32_t             nBytes;                 // Transfer Size in bytes - may include minor-loop offset
      uint16_t             attributes;             // Attributes - see DMA_ATTR_SMOD etc
      uint16_t             sourceOffset;           // Signed increment applied to source address after each transfer
      uint16_t             destinationOffset;      // Signed increment applied to destination address after each transfer
   };
   struct MultipleTransferInfo {
      const volatile void *sourceAddress;          // Source Address
      volatile void       *destinationAddress;     // Destination Address
      uint32_t             nBytes;                 // Transfer Size in bytes - may include minor-loop offset
      uint16_t             attributes;             // Attributes - see DMA_ATTR_SMOD etc
      uint16_t             sourceOffset;           // Signed increment applied to source address after each transfer
      uint16_t             destinationOffset;      // Signed increment applied to destination address after each transfer
      uint16_t             numberOfTransactions;   // Number of transactions
   };
};

/**
 * @addtogroup DMA_Group Direct Memory Access (DMA)
 * @brief Support for DMA operations
 * @{
 */
/**
 * @tparam Info         Information for DMA channel
 * @tparam channel      DMA channel to use
 * @tparam dmaSource    Hardware DMA source e.g. UART0
 */
template<class Info, int channel, int dmaSource=-1>
class DmaChannel_T : public DmaChannel {

protected:
   // Pointer to hardware
   static constexpr volatile DMA_Type *dmac       = reinterpret_cast<volatile DMA_Type*>(Info::basePtr);
   // Pointer to hardware
   static constexpr volatile DMAMUX_Type *dmacmux = reinterpret_cast<volatile DMAMUX_Type*>(Info::info[channel].dmaMux);
   // Pointer to clock register
   static constexpr volatile uint32_t *clockReg   = reinterpret_cast<volatile uint32_t*>(Info::clockReg);
   // Pointer to clock register
   static constexpr volatile uint32_t *muxClockReg   = reinterpret_cast<volatile uint32_t*>(DmaMux0Info::clockReg);
   // IRQ Num
   static constexpr IRQn_Type irqNum              = DMA0_IRQn+channel;

public:
   /**
    * Configure channel for a single transaction
    */
   static void configure(const SingleTransferInfo *information) {
      *muxClockReg  |= DmaMux0Info::clockMask;

      dmac->CR                          = DMA_CR_EMLM_MASK|DMA_CR_EDBG_MASK|DMA_CR_ERCA_MASK; // ML-offset, Debug, RR priority
      if (dmaSource>0) {
         dmac->SERQ                     = channel; // Enable hardware request
      }
      else {
         dmac->CERQ                     = channel;
      }
      dmac->TCD[channel].CITER_ELINKNO  = 1;                                              // Single transaction
      dmac->TCD[channel].BITER_ELINKNO  = 1;                                              // Single transaction
      dmac->TCD[channel].NBYTES_MLNO    = information->nBytes;                            // Number of bytes to transfer
      dmac->TCD[channel].SADDR          = (uint32_t)information->sourceAddress;           // Source address
      dmac->TCD[channel].SOFF           = information->sourceOffset;                      // Increment for SADDR
      dmac->TCD[channel].ATTR           = information->attributes;                        // Attributes - see DMA_ATTR_SMOD etc
      dmac->TCD[channel].SLAST          = 0;                                              // No adjustment as single transfer
      dmac->TCD[channel].DADDR          = (uint32_t)information->destinationAddress;      // Destination address
      dmac->TCD[channel].DOFF           = information->destinationOffset;                 // Increment for DADDR
      dmac->TCD[channel].DLASTSGA       = 0;                                              // No adjustment as single transfer
      dmac->TCD[channel].CSR            = DMA_CSR_START_MASK|DMA_CSR_DREQ_MASK;
   }
   /**
    * Configure channel for a single transaction
    */
   static void configure(const MultipleTransferInfo *information) {
      *muxClockReg  |= DmaMux0Info::clockMask;

      dmac->CR                          = DMA_CR_EMLM_MASK|DMA_CR_EDBG_MASK|DMA_CR_ERCA_MASK; // ML-offset, Debug, RR priority
      if (dmaSource>0) {
         dmac->SERQ                     = channel; // Enable hardware request
         DMAMUX0->CHCFG[channel]        = DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_SOURCE(dmaSource);
      }
      else {
         dmac->CERQ                     = channel;
         DMAMUX0->CHCFG[channel]        = 0;
      }
      dmac->TCD[channel].CITER_ELINKNO  = information->numberOfTransactions;         // Number of transactions
      dmac->TCD[channel].BITER_ELINKNO  = information->numberOfTransactions;         // Number of transactions
      dmac->TCD[channel].NBYTES_MLNO    = information->nBytes;                       // Number of bytes to transfer
      dmac->TCD[channel].SADDR          = (uint32_t)information->sourceAddress;      // Source address
      dmac->TCD[channel].SOFF           = information->sourceOffset;                 // Increment for SADDR
      dmac->TCD[channel].ATTR           = information->attributes;                   // Attributes - see DMA_ATTR_SMOD etc
      dmac->TCD[channel].SLAST          = 0;                                         // No adjustment
      dmac->TCD[channel].DADDR          = (uint32_t)information->destinationAddress; // Destination address
      dmac->TCD[channel].DOFF           = information->destinationOffset;            // Increment for DADDR
      dmac->TCD[channel].DLASTSGA       = 0;                                         // No adjustment
      dmac->TCD[channel].CSR            = DMA_CSR_START_MASK;
   }
   /**
    * Waits until the channel indicates the transaction has completed
    */
   static void waitUntilComplete() {
      int lastCiter = dmac->TCD[channel].CITER_ELINKNO;
      while ((dmac->TCD[channel].CSR & DMA_CSR_DONE_MASK) == 0) {
         int currentCiter = dmac->TCD[channel].CITER_ELINKNO;
         if (lastCiter != currentCiter) {
            lastCiter = currentCiter;
            __asm__ volatile("nop");
         }
      }
   }
   /**
    * Enable hardware requests on channel
    */
   static void enableRequests() {
      DMAMUX0->CHCFG[channel] = DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_SOURCE(dmaSource);
   }
   /**
    * Disable hardware requests on channel
    */
   static void disableRequests() {
      DMAMUX0->CHCFG[channel] = 0;
   }
};

/**
 * @}
 */
} // End namespace USBDM

#endif /* INCLUDE_USBDM_DMA_H_ */
