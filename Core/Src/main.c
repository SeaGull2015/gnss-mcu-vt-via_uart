/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stdio.h"
#include "limits.h"
#include "stdbool.h"
#define inputSize (160) // wave your hands in the air like you don't care

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
 UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char parseResult[] = "\nlatitude is .. ........, longitude is ... ........, height is ......... M\r\n";
char permParseResult[] = "\nlatitude is .. ........, longitude is ... ........, height is ......... M\r\n";

int parseGNSS(uint8_t input[inputSize]){ // we want to find $GPGGA because it contains height, then parse it to spit out the longitude, latitude, height.
	int64_t start = INT_MAX - 1;
	int64_t temp_iter;
	char currentProtocol[6];
	bool identical = true;
	for (int i = 0; i < inputSize; i++){
		if (input[i] == '$'){
			for (int j = i; (j < (i + 6)) && (j < inputSize); j++){
				currentProtocol[j-i] = input[j];
				temp_iter = j;
			}
			if (temp_iter >= inputSize) printf("\nmoved over the edge of the input, trying to find the $GPwhatever, j >= inputSize\r\n");
			for (int j = 0; j < 6; j++){
				if (currentProtocol[j] != "$GPGGA"[j]) identical = false;
			}
			if (identical) {
				start = i;
				break;
			}
			else identical = true;
		}
	}
	// parsing
	if (start == INT_MAX - 1){
		printf("\nparsing failed, start == INT_MAX - 1\r\n");
		return 10;
	}
	int64_t pointer = start;
	while (input[pointer] != ',' && (pointer < inputSize)) {pointer++;} // skip $GPGGA,
	pointer++;
	while (input[pointer] != ',' && (pointer < inputSize)) {pointer++;} // skip time,
	pointer++;

	if (input[pointer] == ',') {} // latitude is missing
	else { // latitude
		parseResult[13] = input[pointer++]; // this is bad, because no && (pointer < inputSize), but ok
		parseResult[14] = input[pointer++];
		parseResult[16] = input[pointer++];
		parseResult[17] = input[pointer++];
		parseResult[18] = input[pointer++];
		parseResult[19] = input[pointer++];
		parseResult[20] = input[pointer++];
		parseResult[21] = input[pointer++];
		parseResult[22] = input[pointer++];
	}
	pointer++;
	//printf("%s\n\n", parseResult); //debug
	if (input[pointer] != ',') { // make sure that north/south isn't missing
		parseResult[23] = input[pointer];
	}
	//printf("%s\n\n", parseResult);//debug
	pointer += 2;

	if (input[pointer] == ',') {} // longitude is missing
	else { // latitude
		parseResult[39] = input[pointer++];
		parseResult[40] = input[pointer++];
		parseResult[41] = input[pointer++];
		parseResult[43] = input[pointer++];
		parseResult[44] = input[pointer++];
		parseResult[45] = input[pointer++];
		parseResult[46] = input[pointer++];
		parseResult[47] = input[pointer++];
		parseResult[48] = input[pointer++];
		parseResult[49] = input[pointer++];
	}
	pointer++;
	if (input[pointer] != ',') { // make sure that hemisphere isn't missing
		parseResult[50] = input[pointer++];
	}
	pointer++;

	while (input[pointer] != ',' && (pointer < inputSize)) {pointer++;} // skip mode,
	pointer++;
	while (input[pointer] != ',' && (pointer < inputSize)) {pointer++;} // skip number of used sattelites,
	pointer++;
	while (input[pointer] != ',' && (pointer < inputSize)) {pointer++;} // skip HDOP, whatever it is
	pointer++;
	// height
	for (int i = pointer; ((i - pointer) < 9) && (i < inputSize); i++){ // pointer+1 because I wanna to put ">" in there if there are too many symbols.
		if (input[i] == ',') break;
		parseResult[63 + i - pointer] = input[i];
		//printf(" i is %u input[i] is %c\r\n", i, input[i]); //debug
	}

	if (pointer >= inputSize){
		printf("\nparsing failed, pointer >= inputSize\r\n");
		return 20;
	}

	return 0;
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  uint8_t input[inputSize];
  printf("hello\n\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	//scanf("%s\n", input);
	HAL_Delay(100); // for some reason gnss sends garbage or useless (for the task) data after startup, so delay.
	HAL_UART_Receive(&huart2, input, sizeof(input)/sizeof(input[0]), 1000);
	int j = parseGNSS(input);
	printf("%s\n\n", parseResult);
	for (int i = 0; i < sizeof(permParseResult)/sizeof(permParseResult[0]); i++){
		parseResult[i] = permParseResult[i];
	}
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

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

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart1, &ch, sizeof(ch), 1000);
	return ch;
}

int __io_getchar(void){
	uint8_t data;
	HAL_UART_Receive(&huart2, &data, sizeof(data), 1000);
	return data;
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
