/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <math.h>

#include "ssd1306.h"
#include "ssd1306_fonts.h"

#include "ring_buffer.h"
#include "keypad.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// receive max 3 characthers on hexa for each buffer

#define KEYPAD_RB_LEN 3
uint8_t keypad_data = 0xFF;
uint8_t keypad_buffer[KEYPAD_RB_LEN];
ring_buffer_t keypad_rb;

// second buffer

#define KEYPAD1_RB_LEN 3
uint8_t keypad_data1 = 0xFF;
uint8_t keypad_buffer1[KEYPAD1_RB_LEN];
ring_buffer_t keypad1_rb;

#define USART2_RB_LEN 1
uint8_t usart2_data = 0xFF;
uint8_t usart2_buffer[USART2_RB_LEN];
ring_buffer_t usart2_rb;

// variable declaration for UART communication
uint8_t HB[] = " system  arithmetic : active \n\r";
volatile uint8_t operation_flag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// weak function to transmit via UART
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 10);
  return len;
}

// implementing reception callback for USART entrys
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Data received in USART2 */
  if (huart->Instance == USART2)
  {
    // selecting of arithmetic operation (2COND REQUIREMENT)
    if (usart2_data == '+' || usart2_data == '-' || usart2_data == '*' || usart2_data == '/')
    {
      ring_buffer_write(&usart2_rb, usart2_data);
      if (ring_buffer_is_full(&usart2_rb) != 0)
      {
        // Transmit "receiving data" message via USART2
        HAL_UART_Transmit(&huart2, (uint8_t *)"Operator received. Press '=' to calculate.\r\n", 46, HAL_MAX_DELAY);
      }
      HAL_UART_Transmit(&huart2, (uint8_t *)" data received from UART \r\n", 25, HAL_MAX_DELAY);
    }
    else if (usart2_data == '=') // validate when user press ==
    {
      operation_flag = 1;
    }
    HAL_UART_Receive_IT(&huart2, &usart2_data, 1);
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

  uint8_t key_pressed = keypad_scan(GPIO_Pin);
  if (key_pressed != 0xFF)
  {
    if (key_pressed == '#')
    {
      ring_buffer_reset(&keypad_rb);
      ring_buffer_reset(&keypad1_rb);
      ring_buffer_reset(&usart2_rb);
      printf("buffers reseted \r\n");
      ssd1306_Fill(Black);

    }

    else if (key_pressed >= '0' && key_pressed <= '9' ) // validation of numbers on HEXA keyboard
    {

      if (ring_buffer_is_full(&keypad_rb) == 0)
      {
        // Transmit "receiving data" message via USART2

        ring_buffer_write(&keypad_rb, key_pressed);
        keypad_data = key_pressed;
      }
      else if (ring_buffer_is_full(&keypad1_rb) == 0)
      {
        ring_buffer_write(&keypad1_rb, key_pressed);
        keypad_data1 = key_pressed;
      }
      else
      {
        printf("both buffers full, do some operation or press '=' if youre already did it\r\n"); // if ring buffer is full, continue with the second buffer
      }
    }
  }
}

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
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  ssd1306_Fill(Black);
  HAL_UART_Transmit(&huart2, HB, sizeof(HB) - 1, 100);

  ssd1306_WriteString(" ARITHMETIC software...\r\n", Font_6x8, White);

  ssd1306_UpdateScreen();

  //initializing all respectives periphericals

  ring_buffer_init(&usart2_rb, usart2_buffer, USART2_RB_LEN);
  ring_buffer_init(&keypad_rb, keypad_buffer, KEYPAD_RB_LEN);
  ring_buffer_init(&keypad1_rb, keypad_buffer1, KEYPAD1_RB_LEN);

  HAL_UART_Receive_IT(&huart2, &usart2_data, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    // adding logic to implement heartbeat
    static uint32_t last_heartbeat_time = 0;
    if (HAL_GetTick() - last_heartbeat_time >= 500) // toggling for 1Hz (500ms on / 500ms off)
    {
      HAL_GPIO_TogglePin(GPIOA, LD2_Pin);
      last_heartbeat_time = HAL_GetTick();
      // HAL_UART_Transmit(&huart2, HB, sizeof(HB) - 1, 100);
    }
    // show entry data on OLED for UART and KEYPAD
    // Mostrar en OLED los datos del teclado
    if (keypad_data != 0xFF)
    {
      ssd1306_SetCursor(20, 20);
      char buffer[2] = {keypad_data, '\0'};
      ssd1306_WriteString(buffer, Font_11x18, White);
      ssd1306_UpdateScreen();
      keypad_data = 0xFF;
    }

    if (keypad_data1 != 0xFF)
    {
      ssd1306_SetCursor(40, 20);
      char buffer[2] = {keypad_data1, '\0'};
      ssd1306_WriteString(buffer, Font_11x18, White);
      ssd1306_UpdateScreen();
      keypad_data1 = 0xFF;
    }

    // show UART entry on OLED
    if (usart2_data != 0xFF)
    {
      ssd1306_SetCursor(20, 40);
      char buffer[2] = {usart2_data, '\0'};
      ssd1306_WriteString(buffer, Font_11x18, White);
      ssd1306_UpdateScreen();
      usart2_data = 0xFF;
    }

    // implementing arithmetic logic


    if (operation_flag == 1)
    {

      HAL_UART_Transmit(&huart2, (uint8_t *)"calculating...", 46, HAL_MAX_DELAY);
      uint32_t num1 = 0;
      uint32_t num2 = 0;
      uint8_t digit;

      while (ring_buffer_is_empty(&keypad_rb) == 0)
      {
        ring_buffer_read(&keypad_rb, &digit);
        num1 = num1 * 10 + (digit - '0');
      }


      while (ring_buffer_is_empty(&keypad1_rb) == 0)
      {
        ring_buffer_read(&keypad1_rb, &digit);
        num2 = num2 * 10 + (digit - '0');
      }

      // scanning uart operator
      uint8_t operator= 0;
      ring_buffer_read(&usart2_rb, &operator);

      // realize respective arithmetic operation
      int32_t result = 0;
      switch (operator)
      {
      case '+':
        result = num1 + num2;
        break;
      case '-':
        result = num1 - num2;
        break;
      case '*':
        result = num1 * num2;
        break;
      case '/':
        if (num2 != 0) // zero validation
          result = num1 / num2;
        else
        {
          printf("Error: division by zero not avaliable.\r\n");
          ssd1306_Fill(Black);
          ssd1306_SetCursor(20, 30);
          ssd1306_WriteString("Error: Div 0", Font_11x18, White);
          ssd1306_UpdateScreen();
          break;
        }
        break;
      default:
        printf("operator not validate \r\n");
        break;
      }
      // validating last functional requirement (CONTROL STATE LED)
      if (result >  0)
            {
              HAL_GPIO_WritePin(GPIOA, LED3_Pin, 1); //  > 0
            }
            else // less or equal
            {
              HAL_GPIO_WritePin(GPIOA, LED3_Pin, 0); // <=0
            }

      // deploy result on  OLED and UART
      ssd1306_Fill(Black);
      char result_str[32];
      sprintf(result_str, "%ld () %ld = %ld", num1, num2, result);
      ssd1306_SetCursor(20, 30);
      ssd1306_WriteString(result_str, Font_6x8, White);
      ssd1306_UpdateScreen();

      printf("Resultado: %ld\r\n", result); // display via UART
      printf("%ld () %ld = %s\r\n", num1 ,num2, result_str);

      // reset operatioo flag

      operation_flag = 0;
    }


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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10D19CE4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ROW_4_GPIO_Port, ROW_4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ROW_3_Pin|ROW_2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin LED3_Pin ROW_3_Pin ROW_2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|LED3_Pin|ROW_3_Pin|ROW_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ROW_4_Pin */
  GPIO_InitStruct.Pin = ROW_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ROW_4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ROW_1_Pin */
  GPIO_InitStruct.Pin = ROW_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ROW_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : COL_4_Pin */
  GPIO_InitStruct.Pin = COL_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(COL_4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : COL_3_Pin COL_1_Pin COL_2_Pin */
  GPIO_InitStruct.Pin = COL_3_Pin|COL_1_Pin|COL_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
