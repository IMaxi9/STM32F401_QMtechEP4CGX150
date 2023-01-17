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
#include "dma.h"
#include "fatfs.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pins.h"
#include "osd.h"
#include "sdCard.h"
#include "fileBrowser.h"
#include "passiveSerial.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define BUFFER_SIZE 4096
#define CORE_NAME_SIZE 64

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const unsigned char version[4] = "2033";
unsigned char ret;
unsigned char last_ret;
char dbgstr[256];
static char buffer[512];
char core_name[CORE_NAME_SIZE];
char currentPath[256];
char image_filename[LONG_FILENAME_LEN] = { "" };
char load_data[LONG_FILENAME_LEN] = { "" };
//FATFS SDFatFs;
FATFS fs;
myListEntry myFileList[64];
char tableName[64*(128)];

int event;
unsigned char key;
unsigned char cmd;
int currentLine = 0;
char selectFileName[256];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t waitACK (void);
void noticeVersion(void);
void initialData( void );
void splashScreen();
int readKeyboard(unsigned char *key, unsigned char *cmd);

//void SendStatusWord32(long statusWord);


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
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_DMA_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  uint16_t lenName;
  uint8_t maxFiles;
  int16_t showFirst = 0;
  dbgprintf("\n~ SPI by Imax9 ~\n");

//  for(uint16_t i=0;i<100;i++){
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
//	HAL_Delay(10);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
//	HAL_Delay(10);
//  }
  noticeVersion();
  initializeSdCard();
  dbgprintf("Pause1s\n");
  HAL_Delay(1000);
  strcpy(currentPath,"//core.np2");

  if (waitACK())
	  program_FPGA_PS(currentPath);
  initialData( );
  initOSD();
  splashScreen();
  //removeOSD();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	strcpy(currentPath,"/");
	maxFiles = buildCat(currentPath);
	//maxFiles = countFiles();
	showList(0,0);
//	buildCat("//ORDENADORES");
//	showList(0,0);
  while (1)
  {
	event = readKeyboard( &key, &cmd );
	 if ( event == EVENT_KEYPRESS ) {
		 switch ( key ) {
		 	 case KEY_UP: // up
		 		 dbgprintf("Key UP\n");
		 		 currentLine --;
		        if ( currentLine < 0 ) {
		        	currentLine = 0;
		        }
		        if((currentLine-showFirst)<0)
		        	showFirst-=8;
		    break;

		 	case KEY_LFT: // left
		 		showFirst-=8;
		 		if(showFirst<0)
		 			showFirst+=8;
		 		currentLine = showFirst;
		 	break;

		 	case KEY_DOW:
		 		dbgprintf("Key DOWN\n");
		 		currentLine ++;
		 		if (currentLine>maxFiles)
		 			currentLine=maxFiles;
		 		if((currentLine-showFirst)>7)
		 			showFirst+=8;
		 	break;

		 	case KEY_RGT: // right
		 		showFirst+=8;
		 		if(showFirst>maxFiles)
		 			showFirst-=8;
		 		currentLine = showFirst;
		 	break;

		 	case KEY_RET:
		 		lenName = myFileList[currentLine].lenName;
		 		if(!lenName){
		 			dbgprintf("Error zero lenname");
		 			break;
		 		}
		 		uint16_t offsetName = myFileList[currentLine].offsetName;
		 		uint8_t j=0;
		 		for(j=0;j<lenName;j++){
		 			selectFileName[j] = tableName[offsetName+j];
		 		}
		 		selectFileName[j]=0;
		 		dbgprintf("Select %s\n",selectFileName);
		 		if(myFileList[currentLine].typeFile == PAR_TYPE){
		 			dbgprintf("Current path: %s\n",currentPath);
		 			uint8_t size = strlen(currentPath)-1;
		 			while (size>1){
		 				if(currentPath[size] == 47)
		 					break;
		 				currentPath[size] = 0;
		 				size--;

		 			}
		 			currentPath[size] = 0;
		 			dbgprintf("Parent path: %s\n",currentPath);
		 			maxFiles = buildCat(currentPath);
		 			//maxFiles = countFiles();
		 			currentLine = 0;
		 		}
		 		else if(myFileList[currentLine].typeFile == DIR_TYPE){
		 			strcat(currentPath,"/");
		 			strcat(currentPath,selectFileName);
		 			dbgprintf("Key Enter path: %s\n",currentPath);
		 			maxFiles = buildCat(currentPath);
		 			//maxFiles = countFiles();
		 			currentLine = showFirst = 0;
		 		}
		 		else if(myFileList[currentLine].typeFile == FIL_TYPE){
		 			dbgprintf("File: %s to FPGA\n",selectFileName);
		 			strcat(currentPath,"/");
		 			strcat(currentPath,selectFileName);
		 			program_FPGA_PS(currentPath);
		 		}

		 		//currentLine = 0;
		 	break;

		 	case KEY_NOTHING: // nothing was pressed
		 	break;

		 	default:
		 	dbgprintf("Unrecognized key pressed: '%d'\n", key );
		 	break;
		 }
		 showList(currentLine,showFirst);
	 }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	HAL_Delay(100);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void dbgprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  int len = strlen(buffer);
//  while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY){
//	  __NOP();
//  }
  HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len,100);
//  while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY){
//  	  __NOP();
//    }
}
uint8_t waitACK (void)
{
	uint8_t data=0;
	uint32_t timeWait=HAL_GetTick()+1000;
  SPI_DESELECTED();
  // syncronize and clear the SPI buffers
  // wait for command acknoledge
  while ( ret != 'K' ) {
    SPI_SELECTED();
    // clear the SPI buffers
    HAL_SPI_TransmitReceive(&spiFPGA,&data,&ret,1,100);

    // wait a little to receive the last bit
    HAL_Delay(100);
    if (HAL_GetTick()>timeWait){
    	SPI_DESELECTED();
    	return 1;
    }
    // ensure SS stays high for now
    SPI_DESELECTED();

    if ( last_ret != ret ) {
      last_ret = ret;
    }
  }
  return 0;
}

void noticeVersion() {
  dbgprintf("SPI version: %c %c %c %c\n", version[0], version[1], version[2], version[3]);
  //Log.notice( "SdFat version: %s"CR, SD_FAT_VERSION_STR );
  //Log.notice( "SdFat support: %s"CR, SDFAT_FILE_TYPE_STR );
  dbgprintf("SdFat support: %s\n", SDFAT_FILE_TYPE_STR);
}

void initialData( void ) {
  // config buffer - 16 bytes
  spi_osd_cmd_cont(0x60); //cmd to fill the config buffer

  for (int i = 1; i <= 13; i++){
	  spi8(0xFF);
  }
  //last bytes as STM version
  spi8(version[2] - '0');
  spi8(version[1] - '0');
  spi8(version[0] - '0');

  DisableOsdSPI();

  strcpy(core_name, "");
  strcpy(load_data, "");
  strcpy(image_filename, "");
}

void splashScreen() {
  OsdClear();
  OsdWriteOffset( 3, SPLASH, 0, 0, 0, 0 );
  //Log.notice(CR"%s"CR CR, SPLASH);
  dbgprintf("\n%s\n",SPLASH);
#ifndef DISABLE_SPLASH_DELAY
  HAL_Delay(3000);
#endif
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

