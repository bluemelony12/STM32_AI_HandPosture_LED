/**
 ******************************************************************************
 * @file    app_comm.c
 * @author  MCD Application Team
 * @brief   Library to manage communication related operation
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "app_comm.h"
#include "app_sensor.h"

#include <string.h>

/* Global variables ----------------------------------------------------------*/
extern AppConfig_TypeDef App_Config;

/* Private macro -------------------------------------------------------------*/
#define UART_COMM_BUFFER_SIZE                     (512)

/* Array size of a static declared array */
#ifndef ARRAY_SIZE
#   define ARRAY_SIZE(a)  ((sizeof(a) / sizeof(a[0])))
#endif

/* Private types -------------------------------------------------------------*/

struct BaseCommand_t{
    const char *Name;
    int (*Parse)(const struct BaseCommand_t *pCmd, const char *Buffer); /** !< Parser is invoked with the command and string right after the command itself*/
    const char *Help;
    const char *Syntax;
    const char *Example;
    int NoAnswer; /**!< When set , successful command do not issue "ok"
                    use it for command that always echo back some answer bad or not */
};

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
static int Parse_Enable(const struct BaseCommand_t *pCmd, const char *Buffer);
static int Parse_Disable(const struct BaseCommand_t *pCmd, const char *Buffer);
static int SC_HandleCmd(const char *Buffer);

struct BaseCommand_t BaseCmd[]={
  { .Name= "enable", .Parse = Parse_Enable,
    .Help="Enable sensor ranging.",
    .Syntax="'enable'",
    .Example="'enable' => Enable sensor ranging"
  },
  { .Name= "disable", .Parse = Parse_Disable,
    .Help="Disable sensor ranging.",
    .Syntax="'disable'",
    .Example="'disable' => Disable sensor ranging",
  }
};

/* Private function definitions ----------------------------------------------*/

/* Parse functions */
static int Parse_Enable(const struct BaseCommand_t *pCmd, const char *Buffer){
  (void)pCmd;
  (void)Buffer;
  /* Start ranging */
  Sensor_StartRanging(&App_Config);
  /* Set application state as ranging */
  App_Config.app_run = true;
  App_Config.IntrCount = 0;
  return(0);
}
static int Parse_Disable(const struct BaseCommand_t *pCmd, const char *Buffer){
  (void)pCmd;
  (void)Buffer;
  /* Stop the sensor */
  Sensor_StopRanging(&App_Config);
  /* Set application state as stopped */
  App_Config.app_run = false;
  return(0);
}

/**
 * @brief  Handle a new command
 * @param  Buffer Command buffer
 * @retval 0 if success, -1 otherwise
 */
int SC_HandleCmd(const char *Buffer)
{
  int Status =-1;
  int CmdLen;
  size_t i;

  for(i = 0 ; i < ARRAY_SIZE(BaseCmd) ; i++)
  {
    int CmdRet;
    CmdLen=strlen(BaseCmd[i].Name);
    if (strncmp(Buffer, BaseCmd[i].Name, CmdLen) == 0 &&
            (Buffer[CmdLen]==' ' || Buffer[CmdLen]=='\t' || Buffer[CmdLen]==0 ))
    {
      CmdRet=BaseCmd[i].Parse(&BaseCmd[i], Buffer+CmdLen);
      if(CmdRet == 0)
      {
        Status = 0;
        if(BaseCmd[i].NoAnswer == 0)
        {
          printf("ok\n");
        }
      }
      break;
    }
  }
  return(Status);
}

/**
 * @brief  COMM Print from UART to terminal
 * @param  App_Config_Ptr Pointer to application context
 * @retval None
 */
static void Comm_PrintTerm(AppConfig_TypeDef *App_Config)
{
	printf("\x1b[2J");
	printf("\x1b[1;1H");
	printf("Hand Posture =  #%d {%s}                                          \r\n",
			(int) (App_Config->AI_Data.handposture_label),
			classes_table[(int)(App_Config->AI_Data.handposture_label)]);

	for (int i = 0; i<AI_NETWORK_OUT_1_SIZE; i++)
	{
		printf("Class #%d {%s} : %f                                           \r\n",
				i,
				classes_table[i],
				App_Config->aiOutData[i]);
	}

}

/**
 * @brief  COMM Handle command
 * @param  App_Config_Ptr Pointer to application context
 * @retval None
 */
void Comm_HandleCmd(AppConfig_TypeDef *App_Config)
{
  /* Process the command */
  if (SC_HandleCmd(App_Config->Comm_RXBuffer) < 0)
  {
    printf("Bad command\n");
  }

  /* Reset the command ready flag */
  App_Config->UartComm_CmdReady = 0;

}


/* Public function definitions -----------------------------------------------*/

__attribute__((weak)) int __io_putchar(int ch)
{
  HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return (status == HAL_OK ? ch : 0);
}

__attribute__((weak)) int __io_getchar(void)
{
  int ch = 0;
  HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return (status == HAL_OK ? ch : 0);
}

/**
 * @brief  COMM Initialization
 * @param  App_Config_Ptr Pointer to application context
 * @retval None
 */
void Comm_Init(AppConfig_TypeDef *App_Config)
{
  /* UART2 initialization */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 921600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    printf("UART init failed\n");
    Error_Handler();
  }

}

/**
 * @brief  COMM Start
 * @param  App_Config_Ptr Pointer to application context
 * @retval None
 */
void Comm_Start(AppConfig_TypeDef *App_Config)
{
  /* If the UART is not busy receiving data, put it in receive interrupt mode */
  HAL_UART_StateTypeDef State;
  App_Config->UartComm_CmdReady = 0;
  App_Config->Uart_RxRcvIndex = 0;
  State = HAL_UART_GetState(&huart2);
  if (HAL_UART_STATE_BUSY_TX_RX != State && HAL_UART_STATE_BUSY_RX != State)
  {
    HAL_UART_Receive_IT(&huart2, (uint8_t *) (App_Config->Uart_RXBuffer), 1);
  }

}

/**
 * @brief  COMM Print through UART
 * @param  App_Config_Ptr Pointer to application context
 * @retval None
 */
void Comm_Print(AppConfig_TypeDef *App_Config)
{
  /* If a new data need to be printed */
  if (App_Config->new_data_received)
  {
    if (1 == App_Config->Params.gesture_gui)
    {

    }
    else
    {
      /* Print the raw data */
      Comm_PrintTerm(App_Config);
    }
  }

}

/**
 * @brief  UART Receive interrupt handler
 * @param  huart UART instance
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* The three following characters launch the processing of the received data
   * as a command */
  if (App_Config.Uart_RXBuffer[App_Config.Uart_RxRcvIndex] == '\r'
        || App_Config.Uart_RXBuffer[App_Config.Uart_RxRcvIndex] == '\n'
        || App_Config.Uart_RXBuffer[App_Config.Uart_RxRcvIndex] == '\x03')
  {
    /* '\x03' is Ctrl-c */
    if (App_Config.Uart_RXBuffer[App_Config.Uart_RxRcvIndex] != '\x03')
    {
      /* Clean the last char */
      App_Config.Uart_RXBuffer[App_Config.Uart_RxRcvIndex] = 0;
    }

    /* Copy data from the UART buffer to the comm buffer */
    memcpy(App_Config.Comm_RXBuffer,
        (char *) App_Config.Uart_RXBuffer,
        App_Config.Uart_RxRcvIndex + 1);
    /* Set a flag indicating a new command is ready to be handled */
    App_Config.UartComm_CmdReady = 1;
    /* Reset the UART buffer index */
    App_Config.Uart_RxRcvIndex = 0;
  }
  else
  {
    if (App_Config.Uart_RxRcvIndex < UART_BUFFER_SIZE)
    {
      /* Increase the index of the UART buffer */
      App_Config.Uart_RxRcvIndex++;
    }
    else
    {
      /* If the index is out of bounds, reset it and increment the overrun
       * counter */
      App_Config.Uart_RxRcvIndex = 0;
      App_Config.Uart_nOverrun++;
    }
  }

  /* Restart the UART in receive interrupt mode */
  HAL_UART_Receive_IT(huart,
      (uint8_t *) &(App_Config.Uart_RXBuffer[App_Config.Uart_RxRcvIndex]),
      1);

}

/**
 * @brief  UART error handler
 * @param  huart UART instance
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
    /* Clear error and  kick of next */
    huart->ErrorCode = 0;
    HAL_UART_Receive_IT(huart,
        (uint8_t *) &(App_Config.Uart_RXBuffer[App_Config.Uart_RxRcvIndex]),
        App_Config.Uart_RxRcvIndex);

}

/**
 * @brief  COMM If a UART command has been received, handle it
 * @param  App_Config_Ptr Pointer to application context
 * @retval None
 */
void Comm_HandleRxCMD(AppConfig_TypeDef *App_Config)
{
  /* If a command has been received */
  if (App_Config->UartComm_CmdReady)
  {
    /* Handle the command */
    Comm_HandleCmd(App_Config);
  }

}
