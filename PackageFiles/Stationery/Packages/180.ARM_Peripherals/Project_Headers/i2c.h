/**
 * @file     i2c.h
 * @brief    I2C interface
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */
#ifndef INCLUDE_USBDM_I2C_H_
#define INCLUDE_USBDM_I2C_H_

#include <stdint.h>
#include "derivative.h"
#include "pin_mapping.h"

namespace USBDM {

/**
 * @addtogroup I2C_Group I2C, Inter-Integrated-Circuit Interface
 * @brief C++ Class allowing access to I2C interface
 * @{
 */

/** I2C Operating mode */
enum I2c_Mode {
   i2c_polled    = 0,                   //!< Operate in i2c_polled mode
   i2c_interrupt = I2C_C1_IICIE_MASK,   //!< Operate in i2c_interrupt mode
};

/**
 * Virtual Base class for I2C interface
 */
class I2c {

public:
   /** States for the I2C state machine */
   enum I2C_State { i2c_idle, i2c_txData, i2c_rxData, i2c_rxAddress };

protected:
   I2C_Type           *i2c;                 //!< I2C hardware instance
   I2C_State           state;               //!< State of current transaction
   const I2c_Mode      mode;                //!< Mode of operation (i2c_interrupt/i2c_polled)
   uint16_t            rxBytesRemaining;    //!< Number of receive bytes remaining in current transaction
   uint16_t            txBytesRemaining;    //!< Number of transmit bytes remaining in current transaction
   uint8_t            *rxDataPtr;           //!< Pointer to receive data for current transaction
   const uint8_t      *txDataPtr;           //!< Pointer to transmit data for current transaction
   uint8_t             addressedDevice;     //!< Address of device being communicated with
   uint8_t             errorCode;           //!< Error code from last transaction

   /** I2C baud rate divisor table */
   static const uint16_t I2C_DIVISORS[4*16];

   /**
    * Construct I2C interface
    *
    * @param i2c  Base address of I2C hardware
    * @param mode Mode of operation (i2c_interrupt or i2c_polled)
    *
    */
   I2c(I2C_Type *i2c, I2c_Mode mode) : i2c(i2c), state(i2c_idle), mode(mode), rxBytesRemaining(0), rxDataPtr(0), addressedDevice(0), errorCode(0) {
   }

   /**
    * Calculate value for baud rate register of I2C
    *
    * This is calculated from processor frequency and given bits-per-second
    *
    * @param bps - Interface speed in bits-per-second
    */
   static uint8_t getBPSValue(uint32_t bps);

   /**
    * Start Rx/Tx sequence by sending address byte
    *
    * @param address - address of slave to access
    */
   void sendAddress(uint8_t address);

   /**
    * Set baud factor value for interface
    *
    * This is calculated from processor frequency and given bits-per-second
    *
    * @param bps - Interface speed in bits-per-second
    */
   void setBPS(uint32_t bps) {
      i2c->F = getBPSValue(bps);
   }

public:
   /**
    * Clear bus hang
    *
    * Generates I2C_SCL clock until I2C_SDA goes high.
    *
    * This is useful if a slave is part-way through a transaction when the master goes away!
    */
   virtual void busHangReset() = 0;

   /**
    * Wait for current sequence to complete
    */
   void waitWhileBusy(void) {
      while (state != i2c_idle) {
         if ((i2c->C1&I2C_C1_IICIE_MASK) == 0) {
            poll();
         }
         else {
            __asm__("wfi");
         }
      }
   }

   /**
    * I2C state-machine based interrupt handler
    */
   virtual void poll(void);

   /**
    * Transmit message
    *
    * @param address  Address of slave to communicate with
    * @param size     Size of transmission data
    * @param data     Data to transmit, 0th byte is often register address
    */
   int transmit(uint8_t address, uint16_t size, const uint8_t data[]);

   /**
    * Receive message
    *
    * @param address  Address of slave to communicate with
    * @param size     Size of reception data
    * @param data     Data buffer for reception
    */
   int receive(uint8_t address, uint16_t size,  uint8_t data[]);

   /**
    * Transmit message followed by receive message.
    *
    * Uses repeated-start.
    *
    * @param address  Address of slave to communicate with
    * @param txSize   Size of transmission data
    * @param txData   Data for transmission
    * @param rxSize   Size of reception data
    * @param rxData   Date buffer for reception
    */
   int txRx(uint8_t address, uint16_t txSize, const uint8_t txData[], uint16_t rxSize, uint8_t rxData[] );

   /**
    * Transmit message followed by receive message.
    * Uses repeated-start.\n
    * Uses shared transmit and receive buffer
    *
    * @param address  Address of slave to communicate with
    * @param txSize   Size of transmission data
    * @param rxSize   Size of reception data
    * @param data     Data for transmission and reception
    */
   int txRx(uint8_t address, uint16_t txSize, uint16_t rxSize, uint8_t data[] );

};

/**
 * @brief Template class representing an I2C interface
 *
 * <b>Example</b>
 *
 * @code
 *  // Instantiate interface
 *  I2C_0 *i2c0 = new USBDM::I2c_T<I2cInfo>();
 *
 *  // Transmit data
 *  const uint8_t txDataBuffer[] = {0x11, 0x22, 0x33, 0x44};
 *
 *  // Receive buffer
 *  uint8_t rxDataBuffer[5];
 *
 *  for(;;) {
 *     // Transmit block
 *     i2c->transmit(0x1D<<1, sizeof(txDataBuffer), txDataBuffer);
 *
 *     // Receive block
 *     i2c->receive((0x1D<<1)|1, sizeof(rxDataBuffer), rxDataBuffer);
 *
 *     // Transmit block followed by read block (using repeated-start)
 *     // Note rxDataBuffer may be the same as txDataBuffer
 *     i2c->txRx(0x1D<<1, sizeof(txDataBuffer), txDataBuffer, sizeof(rxDataBuffer), rxDataBuffer);
 *  }
 *  @endcode
 *
 * @tparam Info            Class describing I2C hardware
 */
template<class Info> class I2c_T : public I2c {

public:
   /** Used by ISR to obtain handle of object */
   static I2c *thisPtr;

public:
   /**
    * Construct I2C interface
    *
    * @param baud
    * @param mode       Mode of operation
    * @param myAddress  Address of this device on bus (not currently used)
    */
   I2c_T(unsigned baud=400000, I2c_Mode mode=i2c_polled, uint8_t myAddress=0) : I2c(reinterpret_cast<I2C_Type*>(Info::basePtr), mode) {

#ifdef DEBUG_BUILD
   // Check pin assignments
   static_assert(Info::info[0].gpioBit != UNMAPPED_PCR, "I2Cx_SCL has not been assigned to a pin");
   static_assert(Info::info[1].gpioBit != UNMAPPED_PCR, "I2Cx_SDA has not been assigned to a pin");
#endif

      busHangReset();
      init(myAddress);
      setBPS(baud);
   }

   /**
    * Initialise interface
    *
    * @param myAddress Address of self (not used)
    */
   void init(const uint8_t myAddress) {

      // Enable clock to I2C interface
      *reinterpret_cast<uint32_t *>(Info::clockReg) |= Info::clockMask;

      thisPtr = this;

      // Configure I2C pins
      Info::initPCRs();

      if (mode&I2C_C1_IICIE_MASK) {
         NVIC_EnableIRQ(Info::irqNums[0]);
      }
      // Enable I2C peripheral
      i2c->C1 = I2C_C1_IICEN_MASK|mode;

      // Default options
      i2c->C2  = I2C_C2_AD(myAddress>>8);
      i2c->A1  = myAddress&~1;
      i2c->FLT = I2C_FLT_FLT(2);
   }

   /**
    * Clear bus hang
    *
    * Generates I2C_SCL clock until I2C_SDA goes high\n
    * This is useful if a slave is part-way through a transaction when the master goes away!
    */
   virtual void busHangReset() {
      // GPIOs used for bit-banging
      using SclGpio = GpioTable_T<Info, 0, I2C_DEFAULT_PCR|PORT_PCR_MUX(FIXED_GPIO_FN)>;
      using SdaGpio = GpioTable_T<Info, 1, I2C_DEFAULT_PCR|PORT_PCR_MUX(FIXED_GPIO_FN)>;

      SdaGpio::setInput();
      /*
       * Set SCL initially high before enabling to minimise disturbance to bus
       */
      SclGpio::setInput();
      SclGpio::set();
      SclGpio::setOutput();
      for (int i=0; i<9; i++) {
         // Set clock high (3-state)
         SclGpio::set();
         for(int j=0; j<20; j++) {
            __asm__("nop");
         }
         // If data is high bus is OK
         if (SdaGpio::read()) {
            break;
         }
         // Set clock low
         SclGpio::clear();
         for(int j=0; j<20; j++) {
            __asm__("nop");
         }
      }
   }

   static void irqHandler() {
      thisPtr->poll();
   }
};

/** Used by ISR to obtain handle of object */
template<class Info> I2c *I2c_T<Info>::thisPtr = 0;

#if defined(USBDM_I2C1_IS_DEFINED)
/**
 * @brief Class representing the I2C0 interface
 *
 * <b>Example</b>\n
 * Refer @ref I2c_T
 */
using I2c0 = I2c_T<I2c0Info>;
#endif

#if defined(USBDM_I2C1_IS_DEFINED)
/**
 * @brief Class representing the I2C1 interface
 *
 * <b>Example</b>
 * Refer @ref I2c_T
 */
using I2c1 = I2c_T<I2c1Info>;
#endif

#if defined(USBDM_I2C2_IS_DEFINED)
/**
 * @brief Class representing the I2C2 interface
 *
 * <b>Example</b>
 * Refer @ref I2c_T
 */
using I2c2 = I2c_T<I2c2Info>;
#endif

/**
 * @}
 */

} // End namespace USBDM

#endif /* INCLUDE_USBDM_I2C_H_ */
