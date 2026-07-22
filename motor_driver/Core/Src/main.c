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
#include "stm32f103xb.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum { clockwise, counter_clockwise, none } direction;

typedef enum {
  // Enums are provided in clockwise direction

  // Full Stepping
  LLLL,
  HLLL,
  LHLL,
  LLHL,
  LLLH,
  // Repeat

  // --------------------------------------------------

  // Half Stepping
  // Any enum already in the full-stepping part will be commented out

  // LLLL,
  // HLLL,
  HHLL,
  // LHLL,
  LHHL,
  // LLHL,
  LLHH,
  // LLLH,
  HLLH
  // Repeat
} step_state;

typedef enum {
  full_step,
  half_step
  // microstepping
} step_mode;

typedef struct {
  uint32_t delay;
  step_mode mode;
  step_state current_state;
  uint16_t *pins;
  GPIO_TypeDef **ports;
} stepper_motor;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INITIAL_DELAY 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN PV */
// Global Variable controlling speed
uint32_t potentiometer_value;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */
void drive_value(step_state next_state, stepper_motor *motor);
void clockwise_rotation();

uint32_t read_potentiometer();

void step(int32_t step_amount, stepper_motor *motor);
step_state get_next_state(direction direction, stepper_motor *motor);
uint32_t get_delay_time(uint32_t input);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  uint16_t pins[4] = {MOTOR_DRIVER_1_Pin, MOTOR_DRIVER_2_Pin,
                      MOTOR_DRIVER_3_Pin, MOTOR_DRIVER_4_Pin};
  GPIO_TypeDef *ports[4] = {MOTOR_DRIVER_1_GPIO_Port, MOTOR_DRIVER_2_GPIO_Port,
                            MOTOR_DRIVER_3_GPIO_Port, MOTOR_DRIVER_4_GPIO_Port};
  stepper_motor motor = {INITIAL_DELAY, half_step, LLLL, pins, ports};
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    potentiometer_value = read_potentiometer();
    motor.delay = get_delay_time(potentiometer_value);

    step(10, &motor);
    // Rotate Clockwise endlessly
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    Error_Handler();
  }

  /** Configure Regular Channel
   */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA,
                    MOTOR_DRIVER_4_Pin | MOTOR_DRIVER_3_Pin |
                        MOTOR_DRIVER_2_Pin | MOTOR_DRIVER_1_Pin,
                    GPIO_PIN_RESET);

  /*Configure GPIO pins : MOTOR_DRIVER_4_Pin MOTOR_DRIVER_3_Pin
   * MOTOR_DRIVER_2_Pin MOTOR_DRIVER_1_Pin */
  GPIO_InitStruct.Pin = MOTOR_DRIVER_4_Pin | MOTOR_DRIVER_3_Pin |
                        MOTOR_DRIVER_2_Pin | MOTOR_DRIVER_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Rotates the stepper motor clockwise continuously
void clockwise_rotation() {
  /*
    step_state current_state = LLLL;
    while (true) {
      current_state = get_next_state(current_state, clockwise);
      drive_value(current_state);
      HAL_Delay(MOTOR_DELAY);
    }
      */
}

void drive_value(step_state next_state, stepper_motor *motor) {
  switch (next_state) {
  case LLLL:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_RESET);
    break;
  case HLLL:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_RESET);
    break;
  case LHLL:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_RESET);
    break;
  case LLHL:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_RESET);
    break;
  case LLLH:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_SET);
    break;
  case HHLL:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_RESET);
    break;
  case LHHL:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_RESET);
    break;
  case LLHH:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_SET);
    break;
  case HLLH:
    HAL_GPIO_WritePin(motor->ports[0], motor->pins[0], GPIO_PIN_SET);
    HAL_GPIO_WritePin(motor->ports[1], motor->pins[1], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[2], motor->pins[2], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->ports[3], motor->pins[3], GPIO_PIN_SET);
    break;
  }
}

uint32_t read_potentiometer() {
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 10000);
  uint32_t value = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);

  return value;
}

void step(int32_t step_amount, stepper_motor *motor) {
  direction direction = none;

  if (step_amount > 0)
    direction = clockwise;
  else if (step_amount < 0)
    direction = counter_clockwise;

  uint32_t abs_step_count =
      (direction == clockwise) ? step_amount : -step_amount;

  for (uint32_t i = 0; i < abs_step_count; i++) {
    // Get next state and drive it
    motor->current_state = get_next_state(direction, motor);
    drive_value(motor->current_state, motor);
    HAL_Delay(motor->delay);
  }
}

step_state get_next_state(direction direction, stepper_motor *motor) {
  // First, check which mode the motor is in
  switch (motor->mode) {

  // If full-stepping
  case full_step:
    switch (motor->current_state) {
    // If motor has not been moved yet
    case LLLL:
      if (direction == clockwise)
        return HLLL;
      return LLLH;

    case HLLL:
      if (direction == clockwise)
        return LHLL;
      return LLLH;
    case LHLL:
      if (direction == clockwise)
        return LLHL;
      return HLLL;
    case LLHL:
      if (direction == clockwise)
        return LLLH;
      return LHLL;
    case LLLH:
      if (direction == clockwise)
        return HLLL;
      return LLHL;
    default:
      return LLLL;
    }

  // If half-stepping
  case half_step:
    switch (motor->current_state) {
    // If motor has not been moved yet
    case LLLL:
      if (direction == clockwise)
        return HLLL;
      return LLLH;

    case HLLL:
      if (direction == clockwise)
        return HHLL;
      return HLLH;
    case HHLL:
      if (direction == clockwise)
        return LHLL;
      return HLLL;
    case LHLL:
      if (direction == clockwise)
        return LHHL;
      return HHLL;
    case LHHL:
      if (direction == clockwise)
        return LLHL;
      return LHLL;
    case LLHL:
      if (direction == clockwise)
        return LLHH;
      return LHHL;
    case LLHH:
      if (direction == clockwise)
        return LLLH;
      return LLHL;
    case LLLH:
      if (direction == clockwise)
        return HLLH;
      return LLHH;
    case HLLH:
      if (direction == clockwise)
        return HLLL;
      return LLLH;
    default:
      return LLLL;
    }

    // case microstepping:

  default:
    return LLLL;
  }

  // Should not reach here
}

#define POTENTIOMETER_MAX_VALUE 4095
#define POTENTIOMETER_MIN_VALUE 0

#define MOTOR_DELAY_MAX 100
#define MOTOR_DELAY_MIN 4

#define SLOPE                                                                  \
  1.0 * (MOTOR_DELAY_MAX - MOTOR_DELAY_MIN) /                                  \
      (POTENTIOMETER_MAX_VALUE - POTENTIOMETER_MIN_VALUE)

// Map the potentiometer value to a delay of miliseconds between 4 to 100
uint32_t get_delay_time(uint32_t input) {

  return MOTOR_DELAY_MIN + SLOPE * (input - POTENTIOMETER_MIN_VALUE);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
