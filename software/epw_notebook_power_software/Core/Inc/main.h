/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern uint16_t ADC_Value[11];
extern float VCC19V5_VADC, VCC19V5_IADC, Output_19V5_Power;
extern float VCC12V_SYS_VADC, VCC12V_SB_VADC, VCC5V0_SYS_VADC, VCC3V3_SYS_VADC;
extern float T_BOOST, T_REAR, T_FRONT;
extern float T_IN, Vref_IN;

extern uint32_t SYS_FAN_TACH_Count;
extern uint32_t SYS_FAN_Speed;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Feed_IWDG(void);
void PMBUS_Read_PSMI(void);
void PMBUS_Read_FRU(void);
void SYS_ADC_Convert_Value(void);
void USB_CDC_Transmit_SYS_Status(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VCC12V_SYS_VADC_Pin GPIO_PIN_1
#define VCC12V_SYS_VADC_GPIO_Port GPIOA
#define VCC12V_SB_VADC_Pin GPIO_PIN_2
#define VCC12V_SB_VADC_GPIO_Port GPIOA
#define VCC19V5_VADC_Pin GPIO_PIN_3
#define VCC19V5_VADC_GPIO_Port GPIOA
#define VCC19V5_IADC_Pin GPIO_PIN_4
#define VCC19V5_IADC_GPIO_Port GPIOA
#define T_BOOST_Pin GPIO_PIN_5
#define T_BOOST_GPIO_Port GPIOA
#define T_REAR_Pin GPIO_PIN_6
#define T_REAR_GPIO_Port GPIOA
#define T_FRONT_Pin GPIO_PIN_7
#define T_FRONT_GPIO_Port GPIOA
#define VCC5V0_SYS_VADC_Pin GPIO_PIN_0
#define VCC5V0_SYS_VADC_GPIO_Port GPIOB
#define VCC3V3_SYS_VADC_Pin GPIO_PIN_1
#define VCC3V3_SYS_VADC_GPIO_Port GPIOB
#define PSON_CTRL_Pin GPIO_PIN_10
#define PSON_CTRL_GPIO_Port GPIOA
#define PSOK_SIG_Pin GPIO_PIN_5
#define PSOK_SIG_GPIO_Port GPIOB
#define LED_FAULT_Pin GPIO_PIN_6
#define LED_FAULT_GPIO_Port GPIOB
#define LED_RUN_Pin GPIO_PIN_7
#define LED_RUN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
