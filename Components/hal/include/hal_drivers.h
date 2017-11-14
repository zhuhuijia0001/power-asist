/**************************************************************************************************
  Filename:       hal_drivers.h
  Revised:        $Date: 2012-07-09 13:23:30 -0700 (Mon, 09 Jul 2012) $
  Revision:       $Revision: 30873 $

  Description:    This file contains the interface to the Drivers service.


  Copyright 2005-2012 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
#ifndef HAL_DRIVER_H
#define HAL_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "hal_types.h"

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

#define PERIOD_RSSI_RESET_EVT               (1 << 3)
#define HAL_LED_BLINK_EVENT                 (1 << 4)
#define QC_CHECK_SUPPORT_EVT                (1 << 5)
#define DETECT_CHECK_EVT                    (1 << 6)
#define AUTO_DETECT_CHECK_EVT               (1 << 7)

#define USB_PD_EVENT                        (1 << 8)

#define SAMPLE_A_W_EVENT                    (1 << 9)

#define HAL_KEY_EVENT                       (1 << 10)

#if defined POWER_SAVING
#define HAL_SLEEP_TIMER_EVENT               (1 << 2)
#define HAL_PWRMGR_HOLD_EVENT               (1 << 1)
#define HAL_PWRMGR_CONSERVE_EVENT           (1 << 0)
#endif

#define HAL_PWRMGR_CONSERVE_DELAY           10
#define PERIOD_RSSI_RESET_TIMEOUT           10

/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

extern uint8 Hal_TaskID;

//fusb302 message id
#define FUSB302_MSG_ID                   0xA5

/**************************************************************************************************
 * FUNCTIONS - API
 **************************************************************************************************/

extern void Hal_Init ( uint8 task_id );

/*
 * Process Serial Buffer
 */
extern uint16 Hal_ProcessEvent ( uint8 task_id, uint16 events );

/*
 * Process Polls
 */
extern void Hal_ProcessPoll (void);

/*
 * Initialize HW
 */
extern void HalDriverInit (void);

#ifdef __cplusplus
}
#endif

#endif

/**************************************************************************************************
**************************************************************************************************/
