#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C"
#endif

/* Interrupt'ų metu vykdomos funkcijos */
void EXTI0_IRQHandler(void);
void SysTick_Handler(void);

/* Globalūs kintamieji */
static TIM_HandleTypeDef s_TimerInstance = {.Instance = TIM4};

void InitializeTimer()
{
	/* Inicializuojamas 4tas timer'is */
	__TIM4_CLK_ENABLE();
	s_TimerInstance.Init.Prescaler = 0;
	s_TimerInstance.Init.CounterMode = TIM_COUNTERMODE_UP;
	s_TimerInstance.Init.Period = 100;
	s_TimerInstance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	s_TimerInstance.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&s_TimerInstance);
	HAL_TIM_Base_Start(&s_TimerInstance);
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

void InitializeInterruptBTN()
{
	/* Inicializuojamas interrupt'as su GPIOA0 (User Button) */
	__GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_BTN;
	GPIO_BTN.Pin = GPIO_PIN_0;
	GPIO_BTN.Mode = GPIO_MODE_IT_RISING;
	GPIO_BTN.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_BTN);

	// Nustato HAL_Delay(...) funkcijos interrupt'o prioritetas (0 - aukščiausias)
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	// Prie User Button pririšto interrupt'o EXTI0_IRQHandler(void) funkcijos prioriteto nustatymas
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void Interrupt(void)
{
	/* Kodo dalis vykdoma interrupt'o metu */
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15, GPIO_PIN_SET);

	// Užkomentavus viskas vyksta itin greitai
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==GPIO_PIN_SET);

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* Funkcija neleidžia interrupt'ui užstrigti, nustato pravalo kanalą, kad bųūtų įvykdytas ateinantis interrupt'as */
	UNUSED(GPIO_Pin);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	Interrupt();
}

void EXTI0_IRQHandler(void)
{
	/* User Button interupt'o metu iškviečiama funkcija */
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void SysTick_Handler(void)
{
	/* Inicializuoja timer'io funkcijas */
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

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

int main(void)
{
	HAL_Init();
	InitializeTimer();
	InitializeLED15();
	InitializeLED12();
	InitializeLED13();
	InitializeLED14();
	InitializeInterruptBTN();
	UART2_Configuration();

	uint8_t buffer[1];

	while(1)
	{
		HAL_UART_Receive(&UART_Handler, buffer, sizeof(buffer), HAL_MAX_DELAY);

		switch(buffer[0])
		{
			case 'g':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
			break;

			case 'o':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
			break;

			case 'r':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
			break;

			case 'b':
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
			break;

		}

	}

}
