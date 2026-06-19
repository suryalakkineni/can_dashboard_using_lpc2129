/******************************************************************************
 * File        : can.c
 * Controller  : LPC2129
 * Description : CAN Driver Functions
 ******************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "can.h"
#include "can_defines.h"

/******************************************************************************
 * Function : Init_CAN1()
 * Purpose  : Initialize CAN1 Controller
 ******************************************************************************/
void Init_CAN1(void)
{
    /*----------------------------------------------------------
      Configure CAN1 Pins

      P0.24 -> TD1 (CAN1_TX)
      P0.25 -> RD1 (CAN1_RX)
    ----------------------------------------------------------*/
    PINSEL1 |= (1 << 16);     /* P0.24 = TD1 */
    PINSEL1 |= (1 << 18);     /* P0.25 = RD1 */

    /*----------------------------------------------------------
      Enter Reset Mode
    ----------------------------------------------------------*/
    C1MOD |= (1 << RM_BIT);

    /*----------------------------------------------------------
      Acceptance Filter

      Accept all CAN messages
    ----------------------------------------------------------*/
    AFMR = (1 << AccBP_BIT);

    /*----------------------------------------------------------
      Set CAN Baud Rate
    ----------------------------------------------------------*/
    C1BTR = BTR_LVAL;

    /*----------------------------------------------------------
      Exit Reset Mode
    ----------------------------------------------------------*/
    C1MOD &= ~(1 << RM_BIT);
}

/******************************************************************************
 * Function : CAN1_Tx()
 * Purpose  : Transmit CAN Frame
 ******************************************************************************/
void CAN1_Tx(CANF txF)
{
    /* Wait until TX Buffer-1 is free */
    while (((C1GSR >> TBS1_BIT) & 1) == 0);

    /* Load CAN Identifier */
    C1TID1 = txF.ID;

    /* Load RTR and DLC */
    C1TFI1 =
        ((txF.BFV.RTR << RTR_BIT) |
         (txF.BFV.DLC << DLC_BITS));

    /* Data Frame */
    if (txF.BFV.RTR == 0)
    {
        /* Bytes 1-4 */
        C1TDA1 = txF.Data1;

        /* Bytes 5-8 */
        C1TDB1 = txF.Data2;
    }

    /* Select TX Buffer-1 and start transmission */
    C1CMR = (1 << STB1_BIT) | (1 << TR_BIT);

    /* Wait until transmission completes */
    while (((C1GSR >> TCS1_BIT) & 1) == 0);
}

/******************************************************************************
 * Function : CAN1_Rx()
 * Purpose  : Receive CAN Frame
 ******************************************************************************/
void CAN1_Rx(CANF *rxF)
{
    /* Wait for message reception */
    while (((C1GSR >> RBS_BIT) & 1) == 0);

    /* Read CAN Identifier */
    rxF->ID = C1RID;

    /* Read RTR bit */
    rxF->BFV.RTR = (C1RFS >> RTR_BIT) & 0x01;

    /* Read DLC */
    rxF->BFV.DLC = (C1RFS >> DLC_BITS) & 0x0F;

    /* Data Frame */
    if (rxF->BFV.RTR == 0)
    {
        /* Read bytes 1-4 */
        rxF->Data1 = C1RDA;

        /* Read bytes 5-8 */
        rxF->Data2 = C1RDB;
    }

    /* Release Receive Buffer */
    C1CMR = (1 << RRB_BIT);
}
