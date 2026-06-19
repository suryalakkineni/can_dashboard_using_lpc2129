#ifndef CAN_DEFINES_H
#define CAN_DEFINES_H

#include "types.h"

/* P0.24 -> TD1 (CAN1_TX) */
#define TD1_PIN

/* P0.25 -> RD1 (CAN1_RX) */
#define RD1_PIN_0_25

/*--------------------------------------------------
  CAN Baud Rate Configuration
--------------------------------------------------*/

#ifndef PCLK
#define PCLK           15000000UL
#endif

#define BIT_RATE       125000UL

#define QUANTA         20

#define BRP            (PCLK/(BIT_RATE*QUANTA))

/* Sample Point = 70% */
#define SAMPLE_POINT   ((7*QUANTA)/10)

#define TSEG1          ((u32)SAMPLE_POINT-1)

#define TSEG2          (QUANTA-(1+TSEG1))

#define SJW            ((TSEG2>=5)?4:(TSEG2-1))

/*--------------------------------------------------
  C1BTR Register Value
--------------------------------------------------*/

#define SAM            0

#define BTR_LVAL \
((SAM<<23) | \
((TSEG2-1)<<20) | \
((TSEG1-1)<<16) | \
((SJW-1)<<14) | \
(BRP-1))

/*--------------------------------------------------
  C1CMR Bits
--------------------------------------------------*/

#define TR_BIT         0
#define RRB_BIT        2
#define STB1_BIT       5

/*--------------------------------------------------
  C1GSR Bits
--------------------------------------------------*/

#define RBS_BIT        0
#define TBS1_BIT       2
#define TCS1_BIT       3

/*--------------------------------------------------
  C1TFI1 / C1RFS Bits
--------------------------------------------------*/

#define RTR_BIT        30
#define DLC_BITS       16

/*--------------------------------------------------
  C1MOD Bits
--------------------------------------------------*/

#define RM_BIT         0

/*--------------------------------------------------
  AFMR Bits
--------------------------------------------------*/

#define AccOFF_BIT     0
#define AccBP_BIT      1

#endif

