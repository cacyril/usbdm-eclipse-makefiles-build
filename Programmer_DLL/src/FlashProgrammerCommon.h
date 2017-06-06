/*
 * FlashProgrammerCommon.h
 *
 *  Created on: 29 Mar 2015
 *      Author: podonoghue
 */

#ifndef SRC_FLASHPROGRAMMERCOMMON_H_
#define SRC_FLASHPROGRAMMERCOMMON_H_

#include "FlashProgrammer.h"
#include "UsbdmTclInterpreter.h"
#include "BdmInterface.h"
#include "ProgressTimer.h"

class ProgressTimer;

class FlashProgrammerCommon : public FlashProgrammer {

public:
   FlashProgrammerCommon(DeviceData::EraseMethod defaultEraseMethod, DeviceData::ResetMethod defaultResetMethod);
   virtual ~FlashProgrammerCommon();

   virtual USBDM_ErrorCode    setDeviceData(const DeviceDataConstPtr device);
   virtual USBDM_ErrorCode    setDeviceData(const DeviceDataConstPtr device, UsbdmTclInterperPtr tclInterpreter);
   virtual DeviceDataConstPtr getDeviceData();
   virtual USBDM_ErrorCode    setTargetInterface(BdmInterfacePtr bdmInterface);

   virtual USBDM_ErrorCode    massEraseTarget() { return massEraseTarget(true); };
   virtual uint16_t           getCalculatedTrimValue() { return calculatedClockTrimValue; };

protected:
   static const int MaxSecurityAreaSize = 100;  //<! Maximum size of a security area that may be saved

   /**
    * Used to save a region of a flash image modified for security
    */
   struct SecurityDataCache {
      static const uint16_t BLANK = 0xFFFF;
      uint32_t address;                   //!< Start address of security area
      uint32_t size;                      //!< Size of area
      uint16_t data[MaxSecurityAreaSize]; //!< Security area data (BLANK indicates empty/invalid location)
   };

   class SetProgrammingMode {
   private:
      BdmInterfacePtr bdmInterface;
   public:
      SetProgrammingMode(BdmInterfacePtr bdmInterface) :
      bdmInterface(bdmInterface) {
         LOGGING_E;
         bdmInterface->setProgrammingMode(true);
      }
      ~SetProgrammingMode() {
         LOGGING_E;
         bdmInterface->setProgrammingMode(false);
      }
   };

   //! Structure for MCGCG parameters
   struct MCG_ClockParameters_t {
      uint8_t  mcgC1;
      uint8_t  mcgC2;
      uint8_t  mcgC3;
      uint8_t  mcgTrim;
      uint8_t  mcgSC;
      uint8_t  mcgCT;
   } ;
   //! Structure for ICGCG parameters
   struct ICG_ClockParameters_t {
      uint8_t  icgC1;        //!< ICSC1 value
      uint8_t  icgC2;        //!< ICSC1 value
      uint16_t icgFilter;    //!< Not used
      uint8_t  icgTrim;      //!< Trim value
   } ;
   //! Structure for ICSCG parameters
   struct ICS_ClockParameters_t {
      uint8_t  icsC1;        //!< ICSC1 value
      uint8_t  icsC2;        //!< ICSC2 value
      uint8_t  icsTrim;      //!< ICSTRM value
      uint8_t  icsSC;        //!< ICSSC value (FTRIM)
   } ;
   union ClockParameters {
      ICG_ClockParameters_t icg;
      MCG_ClockParameters_t mcg;
      ICS_ClockParameters_t ics;
   };

   USBDM_ErrorCode setFlashTrimValues(FlashImagePtr flashImage);
   USBDM_ErrorCode configureTargetClock(unsigned long *busFrequency);
   USBDM_ErrorCode dummyTrimLocations(FlashImagePtr flashImage);

   USBDM_ErrorCode configureICS_Clock(unsigned long *busFrequency, ICS_ClockParameters_t *clockParameters);
   USBDM_ErrorCode configureICG_Clock(unsigned long *busFrequency, ICG_ClockParameters_t *clockParameters);
   USBDM_ErrorCode configureMCG_Clock(unsigned long *busFrequency, MCG_ClockParameters_t *clockParameters);
   USBDM_ErrorCode configureExternal_Clock(unsigned long  *busFrequency);
   USBDM_ErrorCode trimTargetClock(
         uint32_t trimAddress, unsigned long targetBusFrequency, uint16_t *returnTrimValue,
         unsigned long *measuredBusFrequency, int do9BitTrim);
   USBDM_ErrorCode trimICS_Clock(ICS_ClockParameters_t *clockParameters);
   USBDM_ErrorCode trimMCG_Clock(MCG_ClockParameters_t *clockParameters);
   USBDM_ErrorCode trimICG_Clock(ICG_ClockParameters_t *clockParameters);
   /**
    * Checks that there are no modified security areas
    *
    * @return error code if security areas are present
    */
   USBDM_ErrorCode checkNoSecurityAreas(void);
   /**
    * Record the original contents of a security area for later restoration
    *
    * @param flashImage Flash image meing manipulated
    * @param address    Start address of security area
    * @param size       Size of area
    *
    * @return error code see \ref USBDM_ErrorCode.
    */
   USBDM_ErrorCode recordSecurityArea(FlashImagePtr flashImage, const uint32_t address, const uint32_t size);
   /**
    * Restores the contents of the security areas to their saved values
    * All records are cleared
    *
    * @param flashImage    Flash contents to be programmed.
    */
   void            restoreSecurityAreas(FlashImagePtr flashImage);

   bool                       flashReady;               //!< Safety check - only TRUE when flash is ready for programming
   DeviceDataConstPtr         device;                   //!< Parameters describing the current device
   UsbdmTclInterperPtr        tclInterpreter;           //!< TCL interpreter
   BdmInterfacePtr            bdmInterface;             //!< Target specific BDM interface
   FlashProgramConstPtr       currentFlashProgram;      //!< Current program for flash operation
   ProgressTimerPtr           progressTimer;            //!< Progress timer
   uint16_t                   calculatedClockTrimValue; //!< Clock trim value determined from programmed device
   unsigned                   securityAreaCount;
   SecurityDataCache          securityData[2];

   const DeviceData::EraseMethod   defaultEraseMethod;
   const DeviceData::ResetMethod   defaultResetMethod;

   static const char *getFlashOperationName(FlashOperation flashOperation);

   virtual USBDM_ErrorCode massEraseTarget(bool resetTarget) = 0;

   /**
    * Get erase method to use
    *
    * @return erase method
    */
   DeviceData::EraseMethod getEraseMethod();

   /**
    * Get reset method to use
    *
    * @return reset method
    */
   DeviceData::ResetMethod getResetMethod();

   /*
    * Initialises TCL support for current target
    */
   USBDM_ErrorCode initTCL();
   /*
    * Sets and initialises the TCL interpreter
    */
   USBDM_ErrorCode setTCLInterpreter(UsbdmTclInterperPtr ti);
   /*
    * Release the current TCL interpreter
    */
   USBDM_ErrorCode releaseTCL(void);
//   /*
//    * Executes a TCL script in the current TCL interpreter
//    */
//   USBDM_ErrorCode runTCLScript(TclScriptConstPtr script);
   /*
    * Executes a TCL command previously loaded in the TCL interpreter
    */
   USBDM_ErrorCode runTCLCommand(const char *command);

   /*
    * Probe memory location to check if RAM
    */
   USBDM_ErrorCode probeMemory(MemorySpace_t memorySpace, uint32_t address);
};

#endif /* SRC_FLASHPROGRAMMERCOMMON_H_ */
