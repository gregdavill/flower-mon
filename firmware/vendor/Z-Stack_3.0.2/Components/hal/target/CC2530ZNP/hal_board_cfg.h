/**************************************************************************************************
  Filename:       hal_board_cfg.h
  Revised:        $Date: 2014-11-19 13:29:24 -0800 (Wed, 19 Nov 2014) $
  Revision:       $Revision: 41175 $

  Description:    Declarations for the CC2530EM used as a ZNP replacement for
                  the CC2520 on MSP platforms (and possibly others.)


  Copyright 2009-2014 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License"). You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product. Other than for
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

#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H


/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"

/* ------------------------------------------------------------------------------------------------
 *                                       CC2590/CC2591 support
 *
 *                        Define HAL_PA_LNA_CC2590 if CC2530+CC2590EM is used
 *                        Define HAL_PA_LNA if CC2530+CC2591EM is used
 *                        Note that only one of them can be defined
 * ------------------------------------------------------------------------------------------------
 */
#define xHAL_PA_LNA
#define xHAL_PA_LNA_CC2590


/* ------------------------------------------------------------------------------------------------
 *                                       Board Indentifier
 *
 *      Define the Board Identifier to HAL_BOARD_CC2530EB_REV13 for SmartRF05 EB 1.3 board
 * ------------------------------------------------------------------------------------------------
 */

#if !defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_BOARD_CC2530EB_REV13)
#define HAL_BOARD_CC2530EB_REV17
#endif

/* ------------------------------------------------------------------------------------------------
 *                                          Clock Speed
 * ------------------------------------------------------------------------------------------------
 */

#define HAL_CPU_CLOCK_MHZ     32

/* This flag should be defined if the SoC uses the 32MHz crystal
 * as the main clock source (instead of DCO).
 */
#define HAL_CLOCK_CRYSTAL

// Values based on powerup h/w config as input with pull-up - not using dynamic cfg of transport.
#define ZNP_CFG0_32K_XTAL        1         /* 32kHz crystal installed and used */
#define ZNP_CFG0_32K_OSC         0         /* 32kHz crystal not installed; internal osc. used */
#define ZNP_CFG1_SPI             1         /* use SPI transport */
#define ZNP_CFG1_UART            0         /* use UART transport */
#define ZNP_CFG1_UART_USB        2         /* definition required for CC2531 compatibility */
extern unsigned char znpCfg1;
extern unsigned char znpCfg0;


/* ------------------------------------------------------------------------------------------------
 *                                       LED Configuration
 * ------------------------------------------------------------------------------------------------
 */

#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)
  #define HAL_NUM_LEDS            3
#elif defined (HAL_BOARD_CC2530EB_REV13) || defined (HAL_PA_LNA) || defined (HAL_PA_LNA_CC2590)
  #define HAL_NUM_LEDS            1
#else
  #error Unknown Board Indentifier
#endif

#define HAL_LED_BLINK_DELAY()   st( { volatile uint32 i; for (i=0; i<0x5800; i++) { }; } )

/* 1 - Green */
#define LED1_BV           BV(0)
#define LED1_SBIT         P1_0
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_HIGH

#if defined (HAL_BOARD_CC2530EB_REV17)
  /* 2 - Red */
  #define LED2_BV           BV(1)
  #define LED2_SBIT         P1_1
  #define LED2_DDR          P1DIR
  #define LED2_POLARITY     ACTIVE_HIGH

  /* 3 - Yellow */
  #define LED3_BV           BV(4)
  #define LED3_SBIT         P1_4
  #define LED3_DDR          P1DIR
  #define LED3_POLARITY     ACTIVE_HIGH
#endif

/* ------------------------------------------------------------------------------------------------
 *                                       GPIO Configuration
 * ------------------------------------------------------------------------------------------------
 */

#ifndef HAL_GPIO
#define HAL_GPIO TRUE
#endif

#if defined CC2530_MK
#define GPIO_0_PORT         0
#define GPIO_0_PIN          6
#define GPIO_1_PORT         0
#define GPIO_1_PIN          7
#define GPIO_2_PORT         1
#define GPIO_2_PIN          6
#define GPIO_3_PORT         1
#define GPIO_3_PIN          7
#else
#define GPIO_0_PORT         0
#define GPIO_0_PIN          0
#define GPIO_1_PORT         0
#define GPIO_1_PIN          1
#define GPIO_2_PORT         0
#define GPIO_2_PIN          6
#define GPIO_3_PORT         1
#define GPIO_3_PIN          0
#endif

#define GPIO_DIR_IN(IDX)    MCU_IO_DIR_INPUT(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN)
#define GPIO_DIR_OUT(IDX)   MCU_IO_DIR_OUTPUT(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN)
#define GPIO_TRI(IDX)       MCU_IO_INPUT(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN, MCU_IO_TRISTATE)
#define GPIO_PULL_UP(IDX)   MCU_IO_INPUT(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN, MCU_IO_PULLUP)
#define GPIO_PULL_DN(IDX)   MCU_IO_INPUT(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN, MCU_IO_PULLDOWN)
#define GPIO_SET(IDX)       MCU_IO_SET_HIGH(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN)
#define GPIO_CLR(IDX)       MCU_IO_SET_LOW(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN)
#define GPIO_TOG(IDX)       MCU_IO_TGL(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN)
#define GPIO_GET(IDX)       MCU_IO_GET(GPIO_##IDX##_PORT, GPIO_##IDX##_PIN)
#define GPIO_HiD_SET()     (PICTL |=  BV(7))  /* PADSC */
#define GPIO_HiD_CLR()     (PICTL &= ~BV(7))  /* PADSC */

// Used as "func" for the macros below
#define MCU_IO_TRISTATE     1
#define MCU_IO_PULLUP       2
#define MCU_IO_PULLDOWN     3

//-----------------------------------------------------------------------------
//  Macros for simple configuration of IO pins on TI LPW SoCs
//-----------------------------------------------------------------------------
#define MCU_IO_PERIPHERAL(port, pin)   MCU_IO_PERIPHERAL_PREP(port, pin)
#define MCU_IO_INPUT(port, pin, func)  MCU_IO_INPUT_PREP(port, pin, func)
#define MCU_IO_OUTPUT(port, pin, val)  MCU_IO_OUTPUT_PREP(port, pin, val)
#define MCU_IO_SET(port, pin, val)     MCU_IO_SET_PREP(port, pin, val)
#define MCU_IO_SET_HIGH(port, pin)     MCU_IO_SET_HIGH_PREP(port, pin)
#define MCU_IO_SET_LOW(port, pin)      MCU_IO_SET_LOW_PREP(port, pin)
#define MCU_IO_TGL(port, pin)          MCU_IO_TGL_PREP(port, pin)
#define MCU_IO_GET(port, pin)          MCU_IO_GET_PREP(port, pin)

#define MCU_IO_DIR_INPUT(port, pin)    MCU_IO_DIR_INPUT_PREP(port, pin)
#define MCU_IO_DIR_OUTPUT(port, pin)   MCU_IO_DIR_OUTPUT_PREP(port, pin)

//----------------------------------------------------------------------------------
//  Macros for internal use (the macros above need a new round in the preprocessor)
//----------------------------------------------------------------------------------
#define MCU_IO_PERIPHERAL_PREP(port, pin)   st( P##port##SEL |= BV(pin); )

#define MCU_IO_INPUT_PREP(port, pin, func)  st( P##port##SEL &= ~BV(pin); \
                                                P##port##DIR &= ~BV(pin); \
                                                switch (func) { \
                                                case MCU_IO_PULLUP: \
                                                    P##port##INP &= ~BV(pin); \
                                                    P2INP &= ~BV(port + 5); \
                                                    break; \
                                                case MCU_IO_PULLDOWN: \
                                                    P##port##INP &= ~BV(pin); \
                                                    P2INP |= BV(port + 5); \
                                                    break; \
                                                default: \
                                                    P##port##INP |= BV(pin); \
                                                    break; } )

#define MCU_IO_OUTPUT_PREP(port, pin, val)  st( P##port##SEL &= ~BV(pin); \
                                                P##port##_##pin = val; \
                                                P##port##DIR |= BV(pin); )

#define MCU_IO_SET_HIGH_PREP(port, pin)     st( P##port##_##pin = 1; )
#define MCU_IO_SET_LOW_PREP(port, pin)      st( P##port##_##pin = 0; )

#define MCU_IO_SET_PREP(port, pin, val)     st( P##port##_##pin = val; )
#define MCU_IO_TGL_PREP(port, pin)          st( P##port##_##pin ^= 1; )
#define MCU_IO_GET_PREP(port, pin)          (P##port & BV(pin))

#define MCU_IO_DIR_INPUT_PREP(port, pin)    st( P##port##DIR &= ~BV(pin); )
#define MCU_IO_DIR_OUTPUT_PREP(port, pin)   st( P##port##DIR |= BV(pin); )

/* ------------------------------------------------------------------------------------------------
 *                                    Push Button Configuration
 * ------------------------------------------------------------------------------------------------
 */

#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */

/* S1 */
#define PUSH1_BV          BV(1)
#define PUSH1_SBIT        P0_1

#if defined (HAL_BOARD_CC2530EB_REV17)
  #define PUSH1_POLARITY    ACTIVE_HIGH
#elif defined (HAL_BOARD_CC2530EB_REV13)
  #define PUSH1_POLARITY    ACTIVE_LOW
#else
  #error Unknown Board Indentifier
#endif

/* Joystick Center Press */
#define PUSH2_BV          BV(0)
#define PUSH2_SBIT        P2_0
#define PUSH2_POLARITY    ACTIVE_HIGH

/* ------------------------------------------------------------------------------------------------
 *                         OSAL NV implemented by internal flash pages.
 * ------------------------------------------------------------------------------------------------
 */

// Flash is partitioned into 8 banks of 32 KB or 16 pages.
#define HAL_FLASH_PAGE_PER_BANK    16
// Flash is constructed of 128 pages of 2 KB.
#define HAL_FLASH_PAGE_SIZE        2048
#define HAL_FLASH_WORD_SIZE        4

// CODE banks get mapped into the XDATA range 8000-FFFF.
#define HAL_FLASH_PAGE_MAP         0x8000

// The last 16 bytes of the last available page are reserved for flash lock bits.
// NV page definitions must coincide with segment declaration in project *.xcl file.
#if defined NON_BANKED
#define HAL_FLASH_LOCK_BITS        16
#define HAL_NV_PAGE_END            30
#define HAL_NV_PAGE_CNT            2
#else
#define HAL_FLASH_LOCK_BITS        16
#define HAL_NV_PAGE_END            126
#define HAL_NV_PAGE_CNT            6
#endif

// Re-defining Z_EXTADDR_LEN here so as not to include a Z-Stack .h file.
#define HAL_FLASH_IEEE_SIZE        8
#define HAL_FLASH_IEEE_PAGE       (HAL_NV_PAGE_END+1)
#define HAL_FLASH_IEEE_OSET       (HAL_FLASH_PAGE_SIZE - HAL_FLASH_LOCK_BITS - HAL_FLASH_IEEE_SIZE)
#define HAL_INFOP_IEEE_OSET        0xC

#define HAL_FLASH_DEV_PRIVATE_KEY_OSET     0x7D2
#define HAL_FLASH_CA_PUBLIC_KEY_OSET       0x7BC
#define HAL_FLASH_IMPLICIT_CERT_OSET       0x78C

#define HAL_NV_PAGE_BEG           (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)

// Used by DMA macros to shift 1 to create a mask for DMA registers.
#define HAL_NV_DMA_CH              0
#define HAL_DMA_CH_RX              3
#define HAL_DMA_CH_TX              4

#define HAL_NV_DMA_GET_DESC()      HAL_DMA_GET_DESC0()
#define HAL_NV_DMA_SET_ADDR(a)     HAL_DMA_SET_ADDR_DESC0((a))

/* ------------------------------------------------------------------------------------------------
 *  Serial Boot Loader: reserving the first 4 pages of flash and other memory in cc2530-sb.xcl.
 * ------------------------------------------------------------------------------------------------
 */

#define HAL_SB_IMG_ADDR       0x2000
#define HAL_SB_CRC_ADDR       0x2090
#define HAL_SB_CRC_LEN        4
// Size of internal flash less 4 pages for boot loader, 6 pages for NV, & 1 page for lock bits.
#define HAL_SB_IMG_SIZE      (0x40000 - 0x2000 - 0x3000 - 0x0800)

/* ------------------------------------------------------------------------------------------------
 *                                            Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ----------- RF-frontend Connection Initialization ---------- */
#if defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590
extern void MAC_RfFrontendSetup(void);
#define HAL_BOARD_RF_FRONTEND_SETUP() MAC_RfFrontendSetup()
#else
#define HAL_BOARD_RF_FRONTEND_SETUP()
#endif

/* ----------- Cache Prefetch control ---------- */
#define PREFETCH_ENABLE()     st( FCTL = 0x08; )
#define PREFETCH_DISABLE()    st( FCTL = 0x04; )

/* Default powerup with P1_2 as input with pullup. P1_2 is read into znpCfg0 is in "init_board()".
 * 1->0x00 external 32 kHz xosc & 0->0x80 for internal. 
 */
#define HAL_CLOCK_STABLE()    st( uint8 vOSC_32KHZ = ((znpCfg0 == ZNP_CFG0_32K_XTAL) ? 0x00 : 0x80); \
                                  while (CLKCONSTA != (CLKCONCMD_32MHZ | vOSC_32KHZ)); )

#if defined CC2530_MK
#define OSC_32KHZ  0x80
#else
/* Default powerup with P1_2 as input with pullup, so 1->0x00 external 32 kHz xosc & 0->0x80 for internal */
#if (P1_2 == ZNP_CFG0_32K_XTAL)
#define OSC_32KHZ  0x00 /* external 32 KHz xosc */
#else
#define OSC_32KHZ  0x80 /* internal 32 KHz rcosc */
#endif
#endif

/* ----------- Board Initialization ---------- */
#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)

#define HAL_BOARD_INIT() st                                      \
(                                                                \
  uint8 vOSC_32KHZ = OSC_32KHZ;                                  \
  uint16 i;                                                      \
                                                                 \
  SLEEPCMD &= ~OSC_PD;                        /* turn on 16MHz RC and 32MHz XOSC */                \
  while (!(SLEEPSTA & XOSC_STB));             /* wait for 32MHz XOSC stable */                     \
  asm("NOP");                                 /* chip bug workaround */                            \
  for (i=0; i<504; i++) asm("NOP");           /* Require 63us delay for all revs */                \
  CLKCONCMD = (CLKCONCMD_32MHZ | vOSC_32KHZ); /* Select 32MHz XOSC and the source for 32K clock */ \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | vOSC_32KHZ)); /* Wait for the change to be effective */   \
  SLEEPCMD |= OSC_PD;                         /* turn off 16MHz RC */                              \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
)

#elif defined (HAL_BOARD_CC2530EB_REV13) || defined (HAL_PA_LNA) || defined (HAL_PA_LNA_CC2590)

#ifdef HAL_ENABLE_WIFI_COEX_PINS
#define HAL_BOARD_ENABLE_WIFI_COEX_PINS() st                                      \
( 															   \
  /* Set WIFI_TX_DISABLE control P1_6 */							\
  P1DIR |= BV(6); 								               \
)
#else
#define HAL_BOARD_ENABLE_WIFI_COEX_PINS()
#endif

#ifdef HAL_ENABLE_SPARE_PINS_AS_OUTPUTS
#define HAL_BOARD_ENABLE_SPARE_PINS_AS_OUTPUTS() st                                      \
( 															   \
  /* Set SPARE_1 control P0_6 */							\
  P0DIR |= BV(6); 								               \
  /* Set SPARE_2 control P1_5 */							  \
  P1DIR |= BV(5); 											 \
)
#else
#define HAL_BOARD_ENABLE_SPARE_PINS_AS_OUTPUTS()
#endif


#define HAL_BOARD_INIT() st                                      \
(                                                                \
  uint8 vOSC_32KHZ = OSC_32KHZ;                                  \
  uint16 i;                                                      \
                                                                 \
  SLEEPCMD &= ~OSC_PD;                        /* turn on 16MHz RC and 32MHz XOSC */                \
  while (!(SLEEPSTA & XOSC_STB));             /* wait for 32MHz XOSC stable */                     \
  asm("NOP");                                 /* chip bug workaround */                            \
  for (i=0; i<504; i++) asm("NOP");           /* Require 63us delay for all revs */                \
  CLKCONCMD = (CLKCONCMD_32MHZ | vOSC_32KHZ); /* Select 32MHz XOSC and the source for 32K clock */ \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | vOSC_32KHZ)); /* Wait for the change to be effective */   \
  SLEEPCMD |= OSC_PD;                         /* turn off 16MHz RC */                              \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
                                                                 \
  /* set direction for GPIO outputs  */                          \
  LED1_DDR |= LED1_BV;                                           \
                                                                 \
  /* Set PA/LNA HGM control P0_7 */                              \
  P0DIR |= BV(7);                                                \
                                                                 \
  HAL_BOARD_ENABLE_WIFI_COEX_PINS();                             \
                                                                 \
  HAL_BOARD_ENABLE_SPARE_PINS_AS_OUTPUTS();                      \
                                                                 \
  /* configure tristates */                                      \
  P0INP |= PUSH2_BV;                                             \
                                                                 \
  /* setup RF frontend if necessary */                           \
  HAL_BOARD_RF_FRONTEND_SETUP();                                 \
)

#endif

/* ----------- Debounce ---------- */
#define HAL_DEBOUNCE(expr)    { int i; for (i=0; i<500; i++) { if (!(expr)) i = 0; } }

/* ----------- Push Buttons ---------- */
#define HAL_PUSH_BUTTON1()        (PUSH1_POLARITY (PUSH1_SBIT))
#define HAL_PUSH_BUTTON2()        (PUSH2_POLARITY (PUSH2_SBIT))
#define HAL_PUSH_BUTTON3()        (0)
#define HAL_PUSH_BUTTON4()        (0)
#define HAL_PUSH_BUTTON5()        (0)
#define HAL_PUSH_BUTTON6()        (0)

/* ----------- LED's ---------- */
#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)

  #define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
  #define HAL_TURN_OFF_LED2()       st( LED2_SBIT = LED2_POLARITY (0); )
  #define HAL_TURN_OFF_LED3()       st( LED3_SBIT = LED3_POLARITY (0); )
  #define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()

  #define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
  #define HAL_TURN_ON_LED2()        st( LED2_SBIT = LED2_POLARITY (1); )
  #define HAL_TURN_ON_LED3()        st( LED3_SBIT = LED3_POLARITY (1); )
  #define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()

  #define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
  #define HAL_TOGGLE_LED2()         st( if (LED2_SBIT) { LED2_SBIT = 0; } else { LED2_SBIT = 1;} )
  #define HAL_TOGGLE_LED3()         st( if (LED3_SBIT) { LED3_SBIT = 0; } else { LED3_SBIT = 1;} )
  #define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()

  #define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
  #define HAL_STATE_LED2()          (LED2_POLARITY (LED2_SBIT))
  #define HAL_STATE_LED3()          (LED3_POLARITY (LED3_SBIT))
  #define HAL_STATE_LED4()          HAL_STATE_LED1()

#elif defined (HAL_BOARD_CC2530EB_REV13) || defined (HAL_PA_LNA) || defined (HAL_PA_LNA_CC2590)

  #define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
  #define HAL_TURN_OFF_LED2()       HAL_TURN_OFF_LED1()
  #define HAL_TURN_OFF_LED3()       HAL_TURN_OFF_LED1()
  #define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()

  #define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
  #define HAL_TURN_ON_LED2()        HAL_TURN_ON_LED1()
  #define HAL_TURN_ON_LED3()        HAL_TURN_ON_LED1()
  #define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()

  #define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
  #define HAL_TOGGLE_LED2()         HAL_TOGGLE_LED1()
  #define HAL_TOGGLE_LED3()         HAL_TOGGLE_LED1()
  #define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()

  #define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
  #define HAL_STATE_LED2()          HAL_STATE_LED1()
  #define HAL_STATE_LED3()          HAL_STATE_LED1()
  #define HAL_STATE_LED4()          HAL_STATE_LED1()

#endif

/* ----------- XNV ---------- */
#define XNV_SPI_BEGIN()             st(P1_3 = 0;)
#define XNV_SPI_TX(x)               st(U1CSR &= ~0x02; U1DBUF = (x);)
#define XNV_SPI_RX()                U1DBUF
#define XNV_SPI_WAIT_RXRDY()        st(while (!(U1CSR & 0x02));)
#define XNV_SPI_END()               st(P1_3 = 1;)

// The TI reference design uses UART1 Alt. 2 in SPI mode.
#define XNV_SPI_INIT() \
st( \
  /* Mode select UART1 SPI Mode as master. */\
  U1CSR = 0; \
  \
  /* Setup for 115200 baud. */\
  U1GCR = 11; \
  U1BAUD = 216; \
  \
  /* Set bit order to MSB */\
  U1GCR |= BV(5); \
  \
  /* Set UART1 I/O to alternate 2 location on P1 pins. */\
  PERCFG |= 0x02;  /* U1CFG */\
  \
  /* Select peripheral function on I/O pins but SS is left as GPIO for separate control. */\
  P1SEL |= 0xE0;  /* SELP1_[7:4] */\
  /* P1.1,2,3: reset, LCD CS, XNV CS. */\
  P1SEL &= ~0x0E; \
  P1 |= 0x0E; \
  P1_1 = 0; \
  P1DIR |= 0x0E; \
  \
  /* Give UART1 priority over Timer3. */\
  P2SEL &= ~0x20;  /* PRI2P1 */\
  \
  /* When SPI config is complete, enable it. */\
  U1CSR |= 0x40; \
  /* Release XNV reset. */\
  P1_1 = 1; \
)

/* ----------- Minimum safe bus voltage ---------- */

// Vdd/3 / Internal Reference X ENOB --> (Vdd / 3) / 1.15 X 127
#define VDD_2_0  74   // 2.0 V required to safely read/write internal flash.
#define VDD_2_7  100  // 2.7 V required for the Numonyx device.

#define VDD_MIN_RUN  (VDD_2_0+4)  // VDD_MIN_RUN = VDD_MIN_NV */
#define VDD_MIN_NV   (VDD_2_0+4)  // 5% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_GOOD (VDD_2_0+8)  // 10% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_XNV  (VDD_2_7+5)  // 5% margin over minimum to survive a page erase and compaction.

/* ------------------------------------------------------------------------------------------------
 *                                     Driver Configuration
 * ------------------------------------------------------------------------------------------------
 */

/* Set to TRUE enable H/W TIMER usage, FALSE disable it */
#ifndef HAL_TIMER
#define HAL_TIMER FALSE
#endif

/* Set to TRUE enable ADC usage, FALSE disable it */
#ifndef HAL_ADC
#define HAL_ADC TRUE
#endif

/* Set to TRUE enable DMA usage, FALSE disable it */
#ifndef HAL_DMA
#define HAL_DMA TRUE
#endif

/* Set to TRUE enable Flash access, FALSE disable it */
#ifndef HAL_FLASH
#define HAL_FLASH TRUE
#endif

/* Set to TRUE enable AES usage, FALSE disable it */
#ifndef HAL_AES
#define HAL_AES TRUE
#endif

#ifndef HAL_AES_DMA
#define HAL_AES_DMA TRUE
#endif

/* Set to TRUE enable LCD usage, FALSE disable it */
#ifndef HAL_LCD
#define HAL_LCD FALSE
#endif

/* Set to TRUE enable LED usage, FALSE disable it */
#ifndef HAL_LED
#define HAL_LED FALSE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
#define BLINK_LEDS
#endif

/* Set to TRUE enable KEY usage, FALSE disable it */
#ifndef HAL_KEY
#define HAL_KEY FALSE
#endif

#ifndef HAL_SPI
#define HAL_SPI       TRUE
#endif
#define HAL_UART      TRUE

#if defined HAL_SB_BOOT_CODE
#define HAL_UART_DMA  0
#define HAL_UART_ISR  1
#else
#define HAL_UART_DMA  1
#define HAL_UART_ISR  0
#endif
#define HAL_UART_USB  0

// Used to set P2 priority - USART0 over USART1 if both are defined.
#if ((HAL_UART_DMA == 1) || (HAL_UART_ISR == 1))
#define HAL_UART_PRIPO             0x00
#else
#define HAL_UART_PRIPO             0x40
#endif

#endif
/*******************************************************************************************************
*/
