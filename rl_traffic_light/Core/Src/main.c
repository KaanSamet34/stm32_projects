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
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GPIO_ADDR 0x40010800
#define GPIO_PORTA_ADDR GPIO_ADDR + (0x400 * 0)
#define GPIO_PORTB_ADDR GPIO_ADDR + (0x400 * 1)
#define GPIO_PORTC_ADDR GPIO_ADDR + (0x400 * 2)

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


#define BUTTON_PIN  11
#define BUTTON_PORT gpiob

#define GREEN_LED_PIN  12
#define GREEN_LED_PORT gpiob

#define YELLOW_LED_PIN  13
#define YELLOW_LED_PORT gpiob

#define RED_LED_PIN  14
#define RED_LED_PORT gpiob


typedef struct {
  uint32_t pin;
  GPIOPort port;
} led_t;

typedef struct {
  led_t* current_led;
  led_t* previous_led;
} traffic_state_t;

typedef struct{
  uint32_t pin;
  GPIOPort port;
  bool held_down;
} button_t;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
led_t green_led = {.pin = GREEN_LED_PIN, .port = GREEN_LED_PORT};
led_t yellow_led = {.pin = YELLOW_LED_PIN, .port = YELLOW_LED_PORT};
led_t red_led = {.pin = RED_LED_PIN, .port = RED_LED_PORT};
traffic_state_t traffic_state = {&red_led, &red_led};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void gpio_pin_toggle( GPIOPort port, uint32_t pin);
uint32_t gpio_read_odr( GPIOPort port);
void delay(uint32_t);
void gpio_port_initialize(GPIOPort port);
void gpio_pin_configure(GPIOPort port, uint32_t pin, GPIOPinDirection direction);
void update_traffic_light(traffic_state_t* traffic_state, button_t* button);
bool gpio_pin_value(GPIOPort port, uint32_t pin);
bool button_pressed(button_t* button);
uint32_t gpio_read_idr(GPIOPort port);
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

  gpio_port_initialize(gpiob);

  gpio_pin_configure(red_led.port, red_led.pin, write);
  gpio_pin_configure(yellow_led.port, yellow_led.pin, write);
  gpio_pin_configure(green_led.port, green_led.pin, write);
  gpio_pin_configure(BUTTON_PORT, BUTTON_PIN, read);


  

  button_t button = {BUTTON_PIN, BUTTON_PORT, false};
  
  gpio_pin_toggle(red_led.port, red_led.pin);

  while (1)
  {
    update_traffic_light(&traffic_state, &button);
    delay(1);

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
void update_traffic_light(traffic_state_t* traffic_state, button_t* button){
  if(button_pressed(button)){
    // Extinguish the current led
    gpio_pin_toggle(traffic_state->current_led->port, traffic_state->current_led->pin);

    // Update the previous led
    traffic_state->previous_led = traffic_state->current_led;

    // Light the next led and update state
    if(traffic_state->current_led == &red_led){
      gpio_pin_toggle(yellow_led.port, yellow_led.pin);
      traffic_state->current_led = &yellow_led;
    }
    else if(traffic_state->current_led == &yellow_led){
      gpio_pin_toggle(green_led.port, green_led.pin);
      traffic_state->current_led = &green_led;
    }
    else if(traffic_state->current_led == &green_led){
      gpio_pin_toggle(red_led.port, red_led.pin);
      traffic_state->current_led = &red_led;
    }
  }
  
  return;
}

void gpio_port_initialize(GPIOPort port){
  switch (port) {
    case gpioa:
      __HAL_RCC_GPIOA_CLK_ENABLE();
      break;
    case gpiob:
      __HAL_RCC_GPIOB_CLK_ENABLE();
      break;
    case gpioc:
      __HAL_RCC_GPIOC_CLK_ENABLE();
      break;
  }
}

bool button_pressed(button_t* button){
  // 0 is the value received when the button is pressed
  if(!gpio_pin_value(button->port, button->pin)){
    // Debounce
    delay(20);

    if(!gpio_pin_value(button->port, button->pin)){
      // Button is actually down
      // Only proceed if button was not already pressed
      if(!button->held_down){
        button->held_down = 1;
        return true;
      }

      return false;
    }
  }

  button->held_down = 0;
  return false;
}

bool gpio_pin_value(GPIOPort port, uint32_t pin){
  uint32_t value = gpio_read_idr(port);
  uint32_t mask = (1 << pin);
  uint32_t reading = value & mask;

  if(reading == 0) return false;
  return true;
}

void gpio_pin_configure(GPIOPort port, uint32_t pin, GPIOPinDirection direction){
  uint32_t* config_reg_addr;

  // Write to CRH
  if(pin > 7 && pin < 16){
    config_reg_addr = (uint32_t*) (port + GPIO_CRH_OFFSET);
  }
  // Write to CRL
  else if(pin <= 7){
    config_reg_addr = (uint32_t*) (port + GPIO_CRL_OFFSET);
  }

  // Copy the current value
  uint32_t config_value = *config_reg_addr;

  // 00 01: CNF 0 and 1 = 0 while MODE == 01
  if(direction == write){
    config_value &= ~(0xF << pin % 8 * 4);
    config_value |= (0x1 << pin % 8 * 4);
  }

  // 01 00: CNF 0 and 1 = 01 while MODE == 00
  else {
    config_value &= ~(0xF << pin % 8 * 4);
    config_value |= (0x4 << pin  % 8 * 4);
  }

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

uint32_t gpio_read_idr(GPIOPort port){
  uint32_t* idr_reg_addr = (uint32_t*) (port + GPIO_IDR_OFFSET);
  return *idr_reg_addr;
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
