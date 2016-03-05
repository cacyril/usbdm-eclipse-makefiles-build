/*
 * @file dma.cpp (derived from dma-MKL.cpp)
 *
 *  Created on: 08/07/2014
 *      Author: podonoghue
 */

#include "derivative.h"
#include "dma.h"

namespace USBDM {

DmaChannel0 *DmaChannel0::thisPtr = 0;

void DmaChannel0::handler(void) {
}

void DMA0_IRQHandler(void) {
   DmaChannel0::thisPtr->handler();
}

DmaChannel1 *DmaChannel1::thisPtr = 0;

void DmaChannel1::handler(void) {
}

void DMA1_IRQHandler(void) {
   DmaChannel1::thisPtr->handler();
}

DmaChannel2 *DmaChannel2::thisPtr = 0;

void DmaChannel2::handler(void) {
}

void DMA2_IRQHandler(void) {
   DmaChannel2::thisPtr->handler();
}

DmaChannel3 *DmaChannel3::thisPtr = 0;

void DmaChannel3::handler(void) {
}

void DMA3_IRQHandler(void) {
   DmaChannel3::thisPtr->handler();
}

} // End namespace USBDM
