/*
 * @file hmc5883l.cpp
 *
 *  Created on: 10/07/2014
 *      Author: podonoghue
 */

#include "hmc5883l.h"

using namespace USBDM;

enum {
   CRA,
   CRB,
   MR,
   XMSB,
   XLSB,
   YMSB,
   YLSB,
   ZMSB,
   ZLSB,
   SR,
   IRA,
   IRB,
   IRC,
};

   /*!
    * Constructor
    *
    * @param i2c - I2C interface to use
    *
    */
   HMC5883L::HMC5883L(USBDM::I2c *i2c) : i2c(i2c) {
      // Set default settings
      static const uint8_t settings[] = {
         CRA,

         HMC5883L_CRA_MA(3)|  // 8 averages
         HMC5883L_CRA_DO(4)|  // Data rate 15Hz (not relevant)
         HMC5883L_CRA_MS(0),  // Measurement configuration = normal

         HMC5883L_CRB_GN(5),  // Gain : Range = +/- 4.7, Resolution=2.56

         HMC5883L_MR_MD(3),   // Mode = Idle
      };
      i2c->transmit(deviceAddress, settings, sizeof(settings));
//      uint8_t confirm[3];
//      i2c->txRx(deviceAddress, settings, 1, confirm, sizeof(confirm));
   }

   /*!
    * Read ID from compass
    *
    * @return ID value as 24-bit number (0x483433 for HMC5883L)
    */
   uint32_t HMC5883L::readID(void) {
      uint8_t values[] = {IRA, 0x00, 0x00};
      i2c->txRx(deviceAddress, values, 1, sizeof(values));
      return (values[0]<<16)|(values[1]<<8)|values[2];
   }

   /*!
    * Set compass gain on all channels
    *
    * @param gain                                     \n
    * G    Recommended   Gain        Resolution       \n
    * 321  Sensor Range  (LSB/Gauss) (mGauss/LSB)     \n
    * 000   +/- 0.88 Ga    1370        0.73             \n
    * 001   +/- 1.3  Ga    1090        0.92 (default)   \n
    * 010   +/- 1.9  Ga     820        1.22             \n
    * 011   +/- 2.5  Ga     660        1.52             \n
    * 100   +/- 4.0  Ga     440        2.27             \n
    * 101   +/- 4.7  Ga     390        2.56             \n
    * 110   +/- 5.6  Ga     330        3.03             \n
    * 111   +/- 8.1  Ga     230        4.35
    */
   void HMC5883L::setGain(uint8_t gain) {
      // Set Gain
      static const uint8_t controlRegB_Setting[] = {CRB, (uint8_t)HMC5883L_CRB_GN(gain)};
      i2c->transmit(deviceAddress, controlRegB_Setting, sizeof(controlRegB_Setting));
   }

   /*!
    * Set Control register values
    *
    * @param cra                                      \n
    *    Use HMC5883L_CRA_MA(), HMC5883L_CRA_DO(), HMC5883L_CRA_MS() macros to construct value
    * @param crb                                      \n
    *    Use HMC5883L_CRB_GN() macro to construct value
    */
   void HMC5883L::setConfiguration(uint8_t cra, uint8_t crb) {
      // Set CRA & CRB
      static const uint8_t controlReg_Settings[] = {CRA, cra, crb};
      i2c->transmit(deviceAddress, controlReg_Settings, sizeof(controlReg_Settings));
   }

   /*!
    * Do a single triggered measurement of magnetic field
    *
    * @param x - X intensity
    * @param y - Y intensity
    * @param z - Z intensity
    */
   void HMC5883L::doMeasurement(int16_t *x, int16_t *y, int16_t *z) {
      static const uint8_t modeReg_Setting[] = {MR, HMC5883L_MR_HS_MASK|HMC5883L_MR_MD(1)};
      i2c->transmit(deviceAddress, modeReg_Setting, sizeof(modeReg_Setting));

      static const uint8_t statusRegAddress[] = {SR};
      uint8_t status[1];
      do {
         i2c->txRx(deviceAddress, statusRegAddress, sizeof(statusRegAddress), status, sizeof(status));
      } while ((status[0]&HMC5883L_SR_RDY) == 0);

      static const uint8_t resultRegAddress[] = {XMSB};
      uint8_t values[6];
      i2c->txRx(deviceAddress, resultRegAddress, sizeof(resultRegAddress), values, sizeof(values));

      *x = (values[0]<<8)+values[1];
      *z = (values[2]<<8)+values[3];
      *y = (values[4]<<8)+values[5];
   }

//   void HMC5883L::calibrate() {
//      const uint8_t cra[]     = {CRA,  0x71};
//      i2c->transmit(deviceAddress, cra,  sizeof(cra));
//      const uint8_t crb[]     = {CRB,  0xA0};
//      i2c->transmit(deviceAddress, crb,  sizeof(cra));
//      const uint8_t mode[]    = {MR,   0x00};
//      i2c->transmit(deviceAddress, mode, sizeof(cra));
//
//      uint8_t status[1];
//      static const uint8_t statusRegAddress[] = {SR};
//      do {
//         i2c->txRx(deviceAddress, statusRegAddress, sizeof(statusRegAddress), status, sizeof(status));
//      } while ((status[0]&HMC5883L_SR_RDY) == 0);
//
//      static const uint8_t resultRegAddress[] = {XMSB};
//      uint8_t values[6];
//      i2c->txRx(deviceAddress, resultRegAddress, sizeof(resultRegAddress), values, sizeof(values));
//
//      int x = (int16_t)((values[0]<<8)+values[1]);
//      int z = (int16_t)((values[2]<<8)+values[3]);
//      int y = (int16_t)((values[4]<<8)+values[5]);
//
//   }
