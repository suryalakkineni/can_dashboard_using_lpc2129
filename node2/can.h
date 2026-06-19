/******************************************************************************
 * File Name   : can.h
 * Description : Header file for CAN (Controller Area Network) communication.
 *               Contains CAN frame structure definition and function
 *               prototypes for CAN initialization, transmission, and reception.
 ******************************************************************************/

#include "types.h"   // Contains user-defined data types such as u32

/******************************************************************************
 * Structure Name : CAN_FRAME
 * Description    : Represents a CAN message frame.
 *
 * Members:
 *   ID      - CAN Identifier (11-bit Standard ID or 29-bit Extended ID).
 *
 *   BFV     - Bit-field structure containing CAN frame control information.
 *
 *   Data1   - First 32-bit data field.
 *   Data2   - Second 32-bit data field.
 *
 * Note:
 *   Data1 and Data2 together provide 8 bytes (64 bits) of CAN data,
 *   which is the maximum data payload supported by a standard CAN frame.
 ******************************************************************************/
typedef struct CAN_FRAME
{
    /**************************************************************************
     * CAN Message Identifier
     * Used to identify the source/type/priority of the CAN message.
     **************************************************************************/
    u32 ID;

    /**************************************************************************
     * BitField Structure
     * Contains control information related to the CAN frame.
     **************************************************************************/
    struct BitField
    {
        /**********************************************************************
         * RTR (Remote Transmission Request)
         * 0 = Data Frame (contains actual data)
         * 1 = Remote Frame (requests data from another node)
         **********************************************************************/
        u32 RTR : 1;

        /**********************************************************************
         * DLC (Data Length Code)
         * Indicates the number of valid data bytes in the frame.
         * Range: 0 to 8 bytes.
         *
         * Example:
         * DLC = 8 ? Full CAN data frame (8 bytes).
         * DLC = 4 ? Only 4 data bytes are valid.
         **********************************************************************/
        u32 DLC : 4;

    } BFV;   // BFV = Bit Field Variables

    /**************************************************************************
     * Data Payload
     *
     * Data1:
     *   Stores first 4 bytes of CAN data.
     *
     * Data2:
     *   Stores next 4 bytes of CAN data.
     *
     * Total Payload = 8 Bytes
     **************************************************************************/
    u32 Data1;
    u32 Data2;

} CANF;      // CANF is a typedef alias for CAN_FRAME

/******************************************************************************
 * Function Name : Init_CAN1
 * Description   : Initializes CAN1 peripheral.
 *
 * Purpose:
 *   - Configures CAN1 pins (TXD1 and RXD1).
 *   - Sets baud rate.
 *   - Enables CAN controller.
 *   - Prepares CAN hardware for communication.
 *
 * Parameters:
 *   None
 *
 * Return Value:
 *   None
 ******************************************************************************/
void Init_CAN1(void);

/******************************************************************************
 * Function Name : CAN1_Tx
 * Description   : Transmits a CAN frame through CAN1.
 *
 * Parameters:
 *   txFrame - CAN frame containing:
 *             • CAN ID
 *             • RTR bit
 *             • DLC value
 *             • Data bytes
 *
 * Return Value:
 *   None
 *
 * Example:
 *   txFrame.ID = 0x100;
 *   txFrame.BFV.DLC = 2;
 *   txFrame.Data1 = 0x55AA;
 *   CAN1_Tx(txFrame);
 ******************************************************************************/
void CAN1_Tx(CANF txFrame);

/******************************************************************************
 * Function Name : CAN1_Rx
 * Description   : Receives a CAN frame from CAN1.
 *
 * Parameters:
 *   rxFrame - Pointer to CAN frame structure where received
 *             message data will be stored.
 *
 * Return Value:
 *   None
 *
 * Example:
 *   CANF rxFrame;
 *   CAN1_Rx(&rxFrame);
 *
 * After reception:
 *   rxFrame.ID        -> Received CAN ID
 *   rxFrame.BFV.DLC   -> Data length
 *   rxFrame.Data1     -> First 4 bytes of data
 *   rxFrame.Data2     -> Next 4 bytes of data
 ******************************************************************************/
void CAN1_Rx(CANF *rxFrame);

