/**
 * @file      mcg.h
 *
 * @brief    Abstraction layer for MCG interface
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */

#ifndef INCLUDE_USBDM_MCG_H_
#define INCLUDE_USBDM_MCG_H_
 /*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include "derivative.h"
#include "system.h"
#include "hardware.h"

namespace USBDM {

extern volatile uint32_t SystemMcgirClock;
extern volatile uint32_t SystemMcgffClock;
extern volatile uint32_t SystemMcgOutClock;
extern volatile uint32_t SystemMcgFllClock;
extern volatile uint32_t SystemMcgPllClock;
extern volatile uint32_t SystemCoreClock;
extern volatile uint32_t SystemBusClock;
extern volatile uint32_t SystemPeripheralClock;
extern volatile uint32_t SystemOscerClock;
extern volatile uint32_t SystemErclk32kClock;
extern volatile uint32_t SystemLpoClock;
extern volatile uint32_t SystemRtcClock;

extern void setSysDividersStub(uint32_t simClkDiv1);

/**
 * @addtogroup MCG_Group MCG, Multipurpose Clock Generator
 * @brief Abstraction for Multipurpose Clock Generator
 * @{
 */
/**
 * Type definition for MCG interrupt call back
 */
typedef void (*MCGCallbackFunction)(void);

/**
 * @brief Class representing the MCG
 *
 * <b>Example</b>
 * @code
 *    Mcg::initialise();
 * @endcode
 */
class Mcg {

private:
   /** Callback function for ISR */
   static MCGCallbackFunction callback;

   /** Pointer to hardware */
   static constexpr volatile MCG_Type *mcg = reinterpret_cast<volatile MCG_Type*>(McgInfo::basePtr);

public:
   /**
    * Transition from current clock mode to mode given
    *
    * @param to Clock mode to transition to
    */
   static int clockTransition(const McgInfo::ClockInfo &to);

   /**
    * Update SystemCoreClock variable
    *
    * Updates the SystemCoreClock variable with current core Clock retrieved from CPU registers.
    */
   static void SystemCoreClockUpdate(void);

   /**
    *  Change SIM->CLKDIV1 value
    *
    * @param simClkDiv1 - Value to write to SIM->CLKDIV1 register
    */
   static void setSysDividers(uint32_t simClkDiv1) {
      if (!McgInfo::ERRATA_E2448) {
         SIM->CLKDIV1 = simClkDiv1;
      }
      else {
         setSysDividersStub(simClkDiv1);
      }
   }

public:
   /**
    * MCG interrupt handler -  Calls MCG callback
    */
   static void irqHandler() {
      if (callback != 0) {
         callback();
      }
   }

   /**
    * Set callback for ISR
    *
    * @param channel  The MCG channel to modify
    * @param callback The function to call from stub ISR
    */
   static void setCallback(MCGCallbackFunction callback) {
      Mcg::callback = callback;
   }

   /** Current clock mode (FEI out of reset) */
   static McgInfo::ClockMode currentClockMode;

   /**
    *  Configure the MCG for given mode
    *
    *  @param mcr       Module Control Register
    */
   static void configure(int settingNumber=0) {
      clockTransition(McgInfo::clockInfo[settingNumber]);
   }

   /**
    *   Disable the MCG channel
    */
   static void finalise() {
      clockTransition(McgInfo::clockInfo[0]);
   }

   /**
    * Switch to/from high speed run mode
    * Changes the CPU clock frequency/1, and bus clock frequency /2
    * If the clock is set up for 120 MHz this will be the highest performance possible.
    *
    * This routine assumes that the clock preferences have been set up for the usual RUN mode and only
    * the Core clock divider needs to be changed.
    *
    * @param enable True to switch to HSRUN mode
    */
   static void hsRunMode(bool enable);

   /**
    * Sets up the clock out of RESET
    */
   static void initialise(void);
};

/**
 * @}
 */

} // End namespace USBDM

#endif /* INCLUDE_USBDM_MCG_H_ */
