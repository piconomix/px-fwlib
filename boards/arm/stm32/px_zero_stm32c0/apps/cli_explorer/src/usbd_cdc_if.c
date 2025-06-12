/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           USB_Device/CDC_Standalone/USB_Device/App/usbd_cdc_if.c
  * @author         MCD Application Team
  * @brief          This file implements the USB device descriptors.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "px_stm32cube.h"
#include "px_usb_cdc_stdio.h"
#include "px_board.h"
#include "main.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#undef APP_RX_DATA_SIZE
#define APP_RX_DATA_SIZE  64
#undef APP_TX_DATA_SIZE
#define APP_TX_DATA_SIZE  64

/* Periodically, the state of the buffer "UserTxBuffer" is checked.
   The period depends on CDC_POLLING_INTERVAL */
#define CDC_POLLING_INTERVAL    5 /* in ms. The max is 65 and the min is 1 */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
static void   usbd_cdc_if_tim_start(void);
static void   usbd_cdc_if_tim_stop(void);
/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */  

  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);

  // Configure TIM to generate periodic interrupts
  usbd_cdc_if_tim_start();

  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  // Disable periodic TIM interrupts
  usbd_cdc_if_tim_stop();

  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:
    /* Add your code here */
    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
    /* Add your code here */
    break;

  case CDC_SET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_GET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_CLEAR_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_SET_LINE_CODING:
      /*******************************************************************************/
      /* Line Coding Structure                                                       */
      /*-----------------------------------------------------------------------------*/
      /* Offset | Field       | Size | Value  | Description                          */
      /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
      /* 4      | bCharFormat |   1  | Number | Stop bits                            */
      /*                                        0 - 1 Stop bit                       */
      /*                                        1 - 1.5 Stop bits                    */
      /*                                        2 - 2 Stop bits                      */
      /* 5      | bParityType |  1   | Number | Parity                               */
      /*                                        0 - None                             */
      /*                                        1 - Odd                              */
      /*                                        2 - Even                             */
      /*                                        3 - Mark                             */
      /*                                        4 - Space                            */
      /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
      /*******************************************************************************/
    break;

  case CDC_GET_LINE_CODING:
    break;

  case CDC_SET_CONTROL_LINE_STATE:
      {
          USBD_SetupReqTypedef * req = (USBD_SetupReqTypedef *)pbuf;
          // If DTR is set, Virtual Com Port (VCP) has been opened by an 
          // application
          // https://stackoverflow.com/questions/5338875/detecting-open-pc-com-port-from-usb-virtual-com-port-device
          // http://markdingst.blogspot.co.za/2014/06/implementing-usb-communication-device.html
          if((req->wValue &0x0001) != 0)
          {
              main_usb_event_connected();
          }
      }

  case CDC_SEND_BREAK:
     /* Add your code here */
    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  // Pass received data to STDIO handler
  _px_usb_cdc_stdio_on_rx_data(Buf, (uint16_t)*Len);

  // Start reception of next OUT packet
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/**
  * @brief  CDC_TransmitCplt_FS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  /* USER CODE END 13 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
static void usbd_cdc_if_tim_start(void)
{
    // Enable TIM peripheral clock
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM17);

    // Set prescaler so that each tick is 1 ms
    LL_TIM_SetPrescaler(TIM17, __LL_TIM_CALC_PSC(PX_BOARD_PER_CLK_HZ, 1000));

    // Set auto reload value according to desired interval
    LL_TIM_EnableARRPreload(TIM17);
    LL_TIM_SetAutoReload(TIM17, CDC_POLLING_INTERVAL - 1);

    // Set the priority of TIM interrupt
    NVIC_SetPriority(TIM17_IRQn, 5);

    // Enable TIM interrupt handler (NVIC_ISER: SETPENA[18])
    NVIC_EnableIRQ(TIM17_IRQn);

    // Enable TIM update interrupt
    LL_TIM_EnableIT_UPDATE(TIM17);

    // Start Timer
    LL_TIM_EnableCounter(TIM17);
}

static void usbd_cdc_if_tim_stop(void)
{
    // Disable TIM interrupt handler
    NVIC_DisableIRQ(TIM17_IRQn);

    // Stop Timer
    LL_TIM_DisableCounter(TIM17);

    // Disable TIM peripheral clock
    LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_TIM17);
}

void TIM17_IRQHandler(void)
{
    USBD_CDC_HandleTypeDef * hcdc = (USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData;
    uint16_t nr_of_bytes;

    // Clear TIM update interrupt flag
    LL_TIM_ClearFlag_UPDATE(TIM17);

    // Still busy transmitting data on USB?
    if((hcdc == NULL) || (hcdc->TxState != 0))
    {
        return;
    }

    // Fetch data to be sent on USB IN endpoint
    nr_of_bytes = _px_usb_cdc_stdio_get_tx_data(UserTxBufferFS,
                                                CDC_DATA_FS_MAX_PACKET_SIZE);

    // Data to be sent?
    if(nr_of_bytes != 0)
    {
        // Set USB IN transmit data
        USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, nr_of_bytes);

        // Start transmission
        CDC_Transmit_FS(UserTxBufferFS, nr_of_bytes);
    }
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
