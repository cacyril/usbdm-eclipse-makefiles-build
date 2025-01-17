/*
 * @file mcg.cpp  (180.ARM_Peripherals/Startup_Code/mcg_lite.cpp)
 *
 * Based on KL25
 *    1 FLL (FRDIV=/1-/128, /32-/1024, /1280, /1536)
 *    1 PLL (VCO PRDIV=/1-/24, VDIV=pll_vdiv_min+x) (pll_vdiv_min=x16,x24)
 *    RTC enable forces low range
 *    SIM_CLKDIV1_OUTDIV4_MASK divides core clock (post divider)
 *
 *  Created on: 04/03/2012
 *      Author: podonoghue
 */
#include "string.h"
#include "derivative.h" /* include peripheral declarations */
#include "system.h"
#include "utilities.h"
#include "stdbool.h"
#include "hardware.h"
#ifdef USBDM_RTC_IS_DEFINED
#include "rtc.h"
#endif
#include "mcg.h"
#include "osc.h"
 /*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
extern "C" uint32_t SystemCoreClock;
extern "C" uint32_t SystemBusClock;

namespace USBDM {

#ifndef SIM_CLKDIV1_OUTDIV2
#define SIM_CLKDIV1_OUTDIV2(x) (0)
#endif

#ifndef SIM_CLKDIV1_OUTDIV3
#define SIM_CLKDIV1_OUTDIV3(x) (0)
#endif

#ifndef MCG_C2_RANGE0
#define MCG_C2_RANGE0(x) (0)
#endif

#ifndef MCG_C2_HGO0
#define MCG_C2_HGO0(x) (0)
#endif

   /**
    * Table of clock settings
    */
   const McgInfo::ClockInfo Mcg::clockInfo[] = {
$(/MCG/McgClockInfoEntries:!!!!!!!Not found!!!!!!!)
   };

/** MCGFFCLK - Fixed frequency clock (input to FLL) */
volatile uint32_t SystemMcgffClock;

/** MCGOUTCLK - Primary output from MCG, various sources */
volatile uint32_t SystemMcgOutClock;

/** MCGFLLCLK - Output of FLL */
volatile uint32_t SystemMcgFllClock;

/** MCGPLLCLK - Output of PLL */
volatile uint32_t SystemMcgPllClock;

/** Core/System clock (from MCGOUTCLK/CLKDIV) */
//volatile uint32_t SystemCoreClock;

/** Bus clock (from MCGOUTCLK/CLKDIV) */
//volatile uint32_t SystemBusClock;

/** LPO - Low power oscillator 1kHz clock available in LP modes */
volatile uint32_t SystemLpoClock;

/** Callback for programmatically set handler */
MCGCallbackFunction Mcg::callback = {0};

/** Current clock mode (FEI out of reset) */
McgInfo::ClockMode Mcg::currentClockMode = McgInfo::ClockMode::ClockMode_LIRC_2M;

constexpr uint8_t clockTransitionTable[4][4] = {
         /*  from                     to =>   ClockMode_LIRC_2M,           ClockMode_LIRC_8M,           ClockMode_HIRC_48M,           ClockMode_EXT,   */
         /* ClockMode_LIRC_2M,  */ { McgInfo::ClockMode_LIRC_2M,  McgInfo::ClockMode_HIRC_48M, McgInfo::ClockMode_HIRC_48M,  McgInfo::ClockMode_EXT,   },
         /* ClockMode_LIRC_8M,  */ { McgInfo::ClockMode_HIRC_48M, McgInfo::ClockMode_LIRC_8M,  McgInfo::ClockMode_HIRC_48M,  McgInfo::ClockMode_EXT,   },
         /* ClockMode_HIRC_48M, */ { McgInfo::ClockMode_LIRC_2M,  McgInfo::ClockMode_LIRC_8M,  McgInfo::ClockMode_HIRC_48M,  McgInfo::ClockMode_EXT,   },
         /* ClockMode_EXT,      */ { McgInfo::ClockMode_LIRC_2M,  McgInfo::ClockMode_LIRC_8M,  McgInfo::ClockMode_HIRC_48M,  McgInfo::ClockMode_EXT,   },
   };

/**
 * Get name for clock mode
 *
 * @return Pointer to static string
 */
const char *Mcg::getClockModeName(McgInfo::ClockMode clockMode) {
   static const char *modeNames[] {
         "LIRC_2M",
         "LIRC_8M",
         "HIRC_48M",
         "EXT",
   };

   if (clockMode<0) {
      return "Not set";
   }
   if ((unsigned)clockMode>=(sizeof(modeNames)/sizeof(modeNames[0]))) {
      return "Illegal";
   }
   return modeNames[clockMode];
}

/**
 * Transition from current clock mode to mode given
 *
 * @param to Clock mode to transition to
 *
 * @return E_NO_ERROR on success
 */
ErrorCode Mcg::clockTransition(const McgInfo::ClockInfo &clockInfo) {
   McgInfo::ClockMode to = clockInfo.clockMode;

   //TODO move!
//#ifdef USB_CLK_RECOVER_IRC_EN_IRC_EN_MASK
//   if (clockInfo.c7&&MCG_C7_OSCSEL_MASK) {
//      // Note IRC48M Internal Oscillator automatically enable if MCG_C7_OSCSEL = 2
//      SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
//      USB0->CLK_RECOVER_IRC_EN = USB_CLK_RECOVER_IRC_EN_IRC_EN_MASK|USB_CLK_RECOVER_IRC_EN_REG_EN_MASK;
//   }
//#endif

   // Set conservative clock dividers
   setSysDividers(SIM_CLKDIV1_OUTDIV4(5)|SIM_CLKDIV1_OUTDIV3(5)|SIM_CLKDIV1_OUTDIV2(5)|SIM_CLKDIV1_OUTDIV1(5));

#ifdef MCG_C7_OSCSEL
   // Select OSCCLK Source
   mcg().C7 = clockInfo.c7; // OSCSEL = 0,1,2 -> XTAL/XTAL32/IRC48M
#endif

   // Set Fast Internal Clock divider
   mcg().SC = clockInfo.sc;

   // Set HIRCEN, LIRC_DIV2
   mcg().MC = clockInfo.mc;

   // Set conservative clock dividers
   setSysDividers(SIM_CLKDIV1_OUTDIV4(5)|SIM_CLKDIV1_OUTDIV3(5)|SIM_CLKDIV1_OUTDIV2(5)|SIM_CLKDIV1_OUTDIV1(5));

   if (to != McgInfo::ClockMode_None) {
      int transitionCount = 0;
      do {
         // Used to indicate that clock stabilization wait is needed
         bool externalClockInUse = false;

         McgInfo::ClockMode next;
         if (currentClockMode == McgInfo::ClockMode_None) {
            next = McgInfo::ClockMode_LIRC_2M;
         }
         else {
            next = (McgInfo::ClockMode)clockTransitionTable[currentClockMode][to];
         }
         switch (next) {

         case McgInfo::ClockMode_None:
         case McgInfo::ClockMode_LIRC_2M: // From HIRC48, EXT or reset(FEI)

            mcg().C2 =
                  MCG_C2_IRCS(0)   | // IRCS = 0 -> LIRC is in 2 MHz mode
                  clockInfo.c2;      // RANGE0, HGO0, EREFS0

            mcg().C1 =
                  MCG_C1_CLKS(1)   | // CLKS     = 1     -> IRC Selected
                  clockInfo.c1;      // IREFSTEN, IRCLKEN

            // Wait for S_IREFST to indicate FLL Reference has switched to IRC
            do {
               __asm__("nop");
            } while ((mcg().S & MCG_S_CLKST_MASK) != (MCG_S_CLKST(1)));
            break;

         case McgInfo::ClockMode_LIRC_8M: // From HIRC48, EXT or reset(FEI)

            mcg().C2 =
                  MCG_C2_IRCS(1)   | // IRCS = 1 -> LIRC is in 2 MHz mode
                  clockInfo.c2;      // RANGE0, HGO0, EREFS0

            mcg().C1 =
                  MCG_C1_CLKS(1)   | // CLKS     = 1     -> IRC Selected
                  clockInfo.c1;      // IREFSTEN, IRCLKEN

            // Wait for S_IREFST to indicate FLL Reference has switched to IRC
            do {
               __asm__("nop");
            } while ((mcg().S & MCG_S_CLKST_MASK) != (MCG_S_CLKST(1)));
            break;

         case McgInfo::ClockMode_HIRC_48M: // from LIRC_2M, LIRC_8M, EXT

            mcg().C2 =
                  clockInfo.c2;      // IRCS, RANGE0, HGO0, EREFS0

            mcg().C1 =
                  MCG_C1_CLKS(0)   | // CLKS     = 0     -> HIRC selected
                  clockInfo.c1;      // IREFSTEN, IRCLKEN

            // Wait for S_CLKST to indicating that OUTCLK has switched to IRC
            do {
               __asm__("nop");
            } while ((mcg().S & MCG_S_CLKST_MASK) != MCG_S_CLKST(0));
            break;

         case McgInfo::ClockMode_EXT:  // from LIRC_2M, LIRC_8M, HIRC

            mcg().C2 =
                  clockInfo.c2;      // IRCS, RANGE0, HGO0, EREFS0

            mcg().C1 =
                  MCG_C1_CLKS(2)   | // CLKS     = 2     -> EXT selected
                  clockInfo.c1;      // IREFSTEN, IRCLKEN

            // Wait for S_CLKST to indicating that OUTCLK has switched to IRC
            do {
               __asm__("nop");
            } while ((mcg().S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2));
            break;
         }
         // Wait for oscillator stable (if used)
         if (externalClockInUse && (clockInfo.c2&MCG_C2_EREFS0_MASK)) {
            do {
               __asm__("nop");
            } while ((mcg().S & MCG_S_OSCINIT0_MASK) == 0);
         }
         currentClockMode = next;
         if (transitionCount++>5) {
            return setErrorCode(E_CLOCK_INIT_FAILED);
         }
      } while (currentClockMode != to);
   }

   // Main clock dividers
   SIM->CLKDIV1 = clockInfo.clkdiv1;

   // Clock sources
   SIM->SOPT2 = clockInfo.sopt2;

   SystemCoreClockUpdate();

   return E_NO_ERROR;
}

/**
 * Update SystemCoreClock variable
 *
 * Updates the SystemCoreClock variable with current core Clock retrieved from CPU registers.
 */
void Mcg::SystemCoreClockUpdate(void) {

   SystemMcgOutClock = McgInfo::getMcgOutClock();

   SystemCoreClock   = SystemMcgOutClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT)+1);
   SystemBusClock    = SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV4_MASK)>>SIM_CLKDIV1_OUTDIV4_SHIFT)+1);

   SystemLpoClock    = 1000;

//   ::SystemBusClock  = SystemBusClock;
//   ::SystemCoreClock = SystemCoreClock;
}

/**
 * Initialise MCG to default settings.
 */
void Mcg::defaultConfigure() {

   currentClockMode = McgInfo::ClockMode::ClockMode_None;

   // Transition to desired clock mode
   clockTransition(clockInfo[ClockConfig_default]);

   SimInfo::initRegs();

   SystemCoreClockUpdate();
}

} // end namespace USBDM

/**
 * Sets up the clock out of RESET
 */
extern "C"
void clock_initialise(void) {

#ifdef USBDM_OSC0_IS_DEFINED
   USBDM::Osc0::initialise();
#endif

#ifdef USBDM_RTC_IS_DEFINED
   USBDM::Rtc::initialise();
#endif

#ifdef USBDM_MCG_IS_DEFINED
   USBDM::Mcg::initialise();
#endif

}

