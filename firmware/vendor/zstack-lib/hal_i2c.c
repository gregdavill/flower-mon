/**************************************************************************************************
  Filename:       hal_i2c.c
  Revised:        $Date: 2012-09-21 06:30:38 -0700 (Fri, 21 Sep 2012) $
  Revision:       $Revision: 31581 $

  Description:    This module defines the HAL I2C API for the CC2541ST. It
                  implements the I2C master.


  Copyright 2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/
#include "hal_i2c.h"

#include "Debug.h"
#include "ioCC2530.h"
#include "ZComDef.h"
#include "utils.h"

#define STATIC static

// the default cofiguration below uses P0.6 for SDA and P0.5 for SCL.
// change these as needed.
#ifndef OCM_CLK_PORT
#define OCM_CLK_PORT 0
#endif

#ifndef OCM_DATA_PORT
#define OCM_DATA_PORT 0
#endif

#ifndef OCM_CLK_PIN
#define OCM_CLK_PIN 5
#endif

#ifndef OCM_DATA_PIN
#define OCM_DATA_PIN 6
#endif

#define SMB_ACK (0)
#define SMB_NAK (1)
#define SEND_STOP (0)
#define NOSEND_STOP (1)
#define SEND_START (0)
#define NOSEND_START (1)

// *************************   MACROS   ************************************
#define SDA_HIGH() { P0DIR &= ~BV(6); }
#define SDA_LOW() { P0DIR |= BV(6); }

STATIC inline uint8 hali2cSendByte(uint8 dByte);
STATIC inline void hali2cWrite(bool dBit);
STATIC inline void hali2cClock(bool dir);
STATIC inline void hali2cStart(void);
STATIC inline void hali2cRepeatStart(void);
STATIC inline void hali2cStop(void);
STATIC inline uint8 hali2cReceiveByte(void);
STATIC inline bool hali2cRead(void);

/*********************************************************************
 * @fn      HalI2CInit
 * @brief   Initializes two-wire serial I/O bus
 * @param   void
 * @return  void
 */
void HalI2CInit(void) {
    // Set for general I/O operation
    P0SEL &= ~(BV(5) | BV(6));
    // Set port pins as inputs
    P0DIR &= ~(BV(5) | BV(6));
    P0 &= ~(BV(5) | BV(6));
    // Disable Pull-up/down
    P0INP |= (BV(5) | BV(6));
}

/*********************************************************************
 * @fn      hali2cSendByte
 * @brief   Serialize and send one byte to SM-Bus device
 * @param   dByte - data byte to send
 * @return  ACK status - 0=none, 1=received
 */
STATIC bool hali2cSendByte(uint8 dByte) {
    uint8 i;

    for (i = 0; i < 8; i++) {
      
      hali2cClock(0);
    
        // Send the MSB
      if(dByte & 0x80){
         SDA_HIGH();
      }else{
       SDA_LOW();
      }
        //hali2cWrite(dByte & 0x80);
        // Next bit into MSB
        dByte <<= 1;
    
    hali2cClock(1);
    
    }
    
    // need clock low so if the SDA transitions on the next statement the
    // slave doesn't stop. Also give opportunity for slave to set SDA
    hali2cClock(0);
    SDA_HIGH(); // set to input to receive ack...
    hali2cClock(1);
    // hali2cWait(1);

    return (!P0_6); // Return ACK status
}

/*********************************************************************
 * @fn      hali2cWrite
 * @brief   Send one bit to SM-Bus device
 * @param   dBit - data bit to clock onto SM-Bus
 * @return  void
 */
STATIC inline void hali2cWrite(bool dBit) {
    hali2cClock(0);
    // hali2cWait(1);
    if (dBit) {
        SDA_HIGH();
    } else {
        SDA_LOW();
    }

    hali2cClock(1);
    // hali2cWait(1);
}

/*********************************************************************
 * @fn      hali2cClock
 * @brief   Clocks the SM-Bus. If a negative edge is going out, the
 *          I/O pin is set as an output and driven low. If a positive
 *          edge is going out, the pin is set as an input and the pin
 *          pull-up drives the line high. This way, the slave device
 *          can hold the node low if longer setup time is desired.
 * @param   dir - clock line direction
 * @return  void
 */
STATIC inline void hali2cClock(bool dir) {
    if (dir) {
        P0DIR &= ~(1 << 5);
        //IO_DIR_PORT_PIN(OCM_CLK_PORT, OCM_CLK_PIN, IO_IN);
    } else {
        P0DIR |= (1 << 5);
        //IO_DIR_PORT_PIN(OCM_CLK_PORT, OCM_CLK_PIN, IO_OUT);
    }
}

/*********************************************************************
 * @fn      hali2cStart
 * @brief   Initiates SM-Bus communication. Makes sure that both the
 *          clock and data lines of the SM-Bus are high. Then the data
 *          line is set high and clock line is set low to start I/O.
 * @param   void
 * @return  void
 */
STATIC void hali2cStart(void) {
    SDA_LOW(); // start
    hali2cClock(0);
}


/*********************************************************************
 * @fn      hali2cRepeatStart
 * @brief   Initiates SM-Bus communication. Makes sure that both the
 *          clock and data lines of the SM-Bus are high. Then the data
 *          line is set high and clock line is set low to start I/O.
 * @param   void
 * @return  void
 */
STATIC void hali2cRepeatStart(void) {
    hali2cClock(0);
    SDA_HIGH(); // SDA high
    hali2cClock(1);
    SDA_LOW(); // start
    hali2cClock(0);
}

/*********************************************************************
 * @fn      hali2cStop
 * @brief   Terminates SM-Bus communication. Waits unitl the data line
 *          is low and the clock line is high. Then sets the data line
 *          high, keeping the clock line high to stop I/O.
 * @param   void
 * @return  void
 */
STATIC void hali2cStop(void) {
    // Wait for clock high and data low
    hali2cClock(0);
    SDA_LOW(); // force low with SCL low

    hali2cClock(1);
    SDA_HIGH(); // stop condition
}

/*********************************************************************
 * @fn      hali2cReceiveByte
 * @brief   Read the 8 data bits.
 * @param   void
 * @return  character read
 */
STATIC uint8 hali2cReceiveByte() {
    int8 i, rval = 0;

    for (i = 7; i >= 0; --i) {
        rval <<= 1;
        if (hali2cRead()) {
            rval |= 1;
        }
    }

    return rval;
}

/*********************************************************************
 * @fn      hali2cRead
 * @brief   Toggle the clock line to let the slave set the data line.
 *          Then read the data line.
 * @param   void
 * @return  TRUE if bit read is 1 else FALSE
 */
STATIC bool hali2cRead(void) {
    // SCL low to let slave set SDA. SCL high for SDA
    // valid and then get bit
    hali2cClock(0);
    // hali2cWait(1);
    hali2cClock(1);
    // hali2cWait(1);

    return P0_6;
}

// http://e2e.ti.com/support/wireless-connectivity/zigbee-and-thread/f/158/t/140917
/*********************************************************************
 * @fn      I2C_ReadMultByte
 * @brief   reads data into a buffer
 * @param   address: linear address on part from which to read
 * @param   reg: internal register address on part read from
 * @param   buffer: target array for read characters
 * @param   len: max number of bytes to read
 */
int8 I2C_ReadMultByte(uint8 address, uint8 reg, uint8 *buffer, uint16 len) {
    uint16 i = 0;
    uint8 _address = 0;

    if (!len) {
        return I2C_ERROR;
    }

    /* Send START condition */
    hali2cStart();

    /* Set direction of transmission */
    // Reset the address bit0 for write
    // _address &= OCM_WRITE;

    _address = ((address << 1) | OCM_WRITE);

    /* Send Address  and get acknowledgement from slave*/
    if (!hali2cSendByte(_address)) {
        return I2C_ERROR;
    }

    /* Send internal register to read from to */
    if (!hali2cSendByte(reg))
        return I2C_ERROR;

    /* Send RESTART condition */
    hali2cRepeatStart();

    /* Set direction of transmission */
    // Reset the address bit0 for read
    // _address |= OCM_READ;
    _address = ((address << 1) | OCM_READ);
    /* Send Address  and get acknowledgement from slave*/
    if (!hali2cSendByte(_address))
        return I2C_ERROR;

    while (len) {
        // SCL may be high. set SCL low. If SDA goes high when input
        // mode is set the slave won't see a STOP
        hali2cClock(0);
        SDA_HIGH();
        buffer[i] = hali2cReceiveByte();
        // Acknowledgement if not sending last byte
        if (len > 1) {
            hali2cWrite(SMB_ACK); // write leaves SCL high
        }
        
        // increment buffer register
        i++;
        // Decrement the read bytes counter
        len--;
    }

    // condition SDA one more time...
    hali2cClock(0);
    SDA_HIGH();
    hali2cWrite(SMB_NAK);

    hali2cStop();

    // condition SDA one more time...
    //   hali2cClock(0);
    //   SDA_HIGH();
    //   buffer[i] = hali2cReceiveByte();
    //   hali2cWrite(SMB_NAK);

    //   hali2cStop();
    return I2C_SUCCESS;
}

/*********************************************************************
 * @fn      I2C_WriteMultByte
 * @brief   reads data into a buffer
 * @param   address: linear address on part from which to read
 * @param   reg: internal register address on part read from
 * @param   buffer: target array for read characters
 * @param   len: max number of bytes to read
 */
int8 I2C_WriteMultByte(uint8 address, uint8 reg, uint8 *buffer, uint16 len) {
    uint16 i = 0;
    uint8 _address = 0;

    if (!len) {
        return I2C_ERROR;
    }

    /* Send START condition */
    hali2cStart();
    
    /* Set direction of transmission */
    // Reset the address bit0 for write
    _address = ((address << 1) | OCM_WRITE);

    /* Send Address  and get acknowledgement from slave*/
    if (!hali2cSendByte(_address))
        return I2C_ERROR;

    /* Send internal register to read from to */
    if (!hali2cSendByte(reg))
        return I2C_ERROR;

    /* Write data into register */
    // read bytes of data into buffer
    while (len) {
        // SCL may be high. set SCL low. If SDA goes high when input
        // mode is set the slave won't see a STOP
        hali2cClock(0);
        SDA_HIGH();

        /* Send Address  and get acknowledgement from slave*/
        if (!hali2cSendByte(buffer[i]))
            return I2C_ERROR;

        // increment buffer register
        i++;
        // Decrement the read bytes counter
        len--;
    }

    hali2cStop();
    return I2C_SUCCESS;
}