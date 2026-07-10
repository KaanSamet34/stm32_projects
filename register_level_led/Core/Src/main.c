/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GPIO_PORTA_ADDR 0x40010800
#define GPIO_PORTB_ADDR 0x40010C00
#define GPIO_PORTC_ADDR 0x40011000

#define GPIO_CRL_OFFSET  0x00
#define GPIO_CRH_OFFSET  0x04

#define GPIO_IDR_OFFSET  0x08
#define GPIO_ODR_OFFSET  0x0C

#define GPIO_BSRR_OFFSET 0x10
#define GPIO_BRR_OFFSET  0x14

#define GPIO_LCKR_OFFSET 0x18

typedef enum  {
  gpioa = GPIO_PORTA_ADDR,
  gpiob = GPIO_PORTB_ADDR,
  gpioc = GPIO_PORTC_ADDR
} GPIOPort;

typedef enum {
  write,
  read
} GPIOPinDirection;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void gpio_configure( GPIOPort port, uint32_t pin, GPIOPinDirection direction);
void gpio_pin_toggle( GPIOPort port, uint32_t pin);
uint32_t gpio_read_odr( GPIOPort port);
void delay(uint32_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  gpio_configure(gpioc, 13, write);
  while (1)
  {
    gpio_pin_toggle(gpioc, 13);
    delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void gpio_configure(GPIOPort port, uint32_t pin, GPIOPinDirection direction){
  __HAL_RCC_GPIOC_CLK_ENABLE();

  uint32_t* config_reg_addr;

  // Write to CRH
  if(pin > 7 && pin < 16){
    config_reg_addr = (uint32_t*) (port + GPIO_CRH_OFFSET);
  }
  // Write to CRL
  else if(pin <= 7){
    config_reg_addr = (uint32_t*) (port + GPIO_CRL_OFFSET);
  }

  uint32_t config_value;

  // 00 01: CNF 0 and 1 = 0 while MODE == 01
  if(direction == write) config_value = (0x1 << pin % 8 * 4);

  // 01 00: CNF 0 and 1 = 01 while MODE == 00
  else config_value = (0x4 << pin * 4);

  *config_reg_addr =  config_value;
}

void gpio_pin_toggle(GPIOPort port, uint32_t pin){
  uint32_t* odr_reg_addr = (uint32_t*) (port + GPIO_ODR_OFFSET);

  // Read the pin value first
  uint32_t value = gpio_read_odr(port);

  // Set Mask
  uint32_t mask = (1 << pin);

  // XOR operation to flip the bit
  value ^= mask;

  *odr_reg_addr = value;
}

uint32_t gpio_read_odr(GPIOPort port){
  uint32_t* odr_reg_addr = (uint32_t*) (port + GPIO_ODR_OFFSET);
  return *odr_reg_addr;
}

void delay(uint32_t time){
  HAL_Delay(time);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
