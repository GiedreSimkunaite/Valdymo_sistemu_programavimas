#include "stm32f4xx_hal.h"

UART_HandleTypeDef UART_Handler;

void UART2_Configuration(void)
{
	/* Inicializuojama UART komunikacija */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	GPIO_InitTypeDef UART2_GPIO_Handler;

	UART2_GPIO_Handler.Pin= GPIO_PIN_2 | GPIO_PIN_3;
	UART2_GPIO_Handler.Mode= GPIO_MODE_AF_PP;
	UART2_GPIO_Handler.Pull= GPIO_PULLUP;
	UART2_GPIO_Handler.Speed= GPIO_SPEED_FREQ_VERY_HIGH;
	UART2_GPIO_Handler.Alternate= GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &UART2_GPIO_Handler);
	UART_Handler.Instance= USART2;
	UART_Handler.Init.BaudRate= 115200;
	UART_Handler.Init.Mode= UART_MODE_TX_RX;
	UART_Handler.Init.WordLength= UART_WORDLENGTH_8B;
	UART_Handler.Init.StopBits= UART_STOPBITS_1;
	UART_Handler.Init.OverSampling= UART_OVERSAMPLING_16;
	HAL_UART_Init(&UART_Handler);
}

void InitializeLED12()
{
	/* Inicializuojamas GPIOD12 (žalias led'as) kaip išėjimas */
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_LED12;
	GPIO_LED12.Pin = GPIO_PIN_12;
	GPIO_LED12.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_LED12.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_LED12);
}

void InitializeLED15()
{
	/* Inicializuojamas GPIOD13 (oranžinis led'as) kaip išėjimas */
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_LED15;
	GPIO_LED15.Pin = GPIO_PIN_15;
	GPIO_LED15.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_LED15.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_LED15);
}

void InitializeLED13()
{
	/* Inicializuojamas GPIOD13 (oranžinis led'as) kaip išėjimas */
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_LED13;
	GPIO_LED13.Pin = GPIO_PIN_13;
	GPIO_LED13.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_LED13.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_LED13);
}

void InitializeLED14()
{
	/* Inicializuojamas GPIOD14 (raudonas led'as) kaip išėjimas */
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_LED14;
	GPIO_LED14.Pin = GPIO_PIN_14;
	GPIO_LED14.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_LED14.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_LED14);
}

void SysTick_Handler()
{
	HAL_IncTick();
}

int main(void)
{
	HAL_Init();
	InitializeLED12();
	InitializeLED13();
	InitializeLED14();
	InitializeLED15();
	UART2_Configuration();

	uint8_t buffer[1];

	while(1)
	{
		HAL_UART_Receive(&UART_Handler, buffer, sizeof(buffer), HAL_MAX_DELAY);

		switch(buffer[0])
		{
			case 'g':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
			break;

			case 'o':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
			break;

			case 'r':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
			break;

			case 'b':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
			break;

			case '2':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
			break;

			case '3':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
			break;

			case '4':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
			break;

			case '5':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
			break;

			case '1':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
			break;

			case '0':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
			break;
		}

	}

}
