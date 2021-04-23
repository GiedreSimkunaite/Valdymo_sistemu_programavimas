#include <stm32f4xx_hal.h>

#ifdef __cplusplus
extern "C"
#endif

/* Interrupt'ų metu vykdomos funkcijos */
void EXTI0_IRQHandler(void);
void SysTick_Handler(void);

/* Globalūs kintamieji */
static TIM_HandleTypeDef s_TimerInstance = {.Instance = TIM4};
int i, flag;

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

void InitializeLED15()
{
	/* Inicializuojamas GPIOD15 (mėlynas led'as) su PWM išėjimu*/
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_LED15;
	GPIO_LED15.Pin = GPIO_PIN_15;
	GPIO_LED15.Mode = GPIO_MODE_AF_PP;
	GPIO_LED15.Pull = GPIO_NOPULL;
	GPIO_LED15.Speed = GPIO_SPEED_LOW;
	GPIO_LED15.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD, &GPIO_LED15);
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

void InitializeTimerPWM(int brightness)
{
	/* Inicializuojamas PWM */
	TIM_OC_InitTypeDef sConfigOC;
	HAL_TIM_PWM_Init(&s_TimerInstance);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = brightness;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	HAL_TIM_PWM_ConfigChannel(&s_TimerInstance, &sConfigOC, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&s_TimerInstance, TIM_CHANNEL_4);
}

void Interrupt(void)
{
	/* Kodo dalis vykdoma interrupt'o metu */
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12, GPIO_PIN_SET);

	if(flag == 0)
		i+=5;
	else
		i-=5;

	// Užkomentavus viskas vyksta itin greitai
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==GPIO_PIN_SET);

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

int main(void)
{
	HAL_Init();
	InitializeTimer();
	InitializeTimerPWM(0);
	InitializeLED15();
	InitializeLED12();
	InitializeLED13();
	InitializeInterruptBTN();
	i = 0;
	flag = 1;
	for(;;)
	{
		/* Interrupt'o  metu ryškiau/blankiau užsišviečia mėlynas led'as, užgesta oranžinis ir įsijungia žalias led'ai*/
		/* Po interrupt'o mėlyno led'o ryškumas nekinta, užgesta žalias ir įsijungia oranžinis led'ai*/
		InitializeTimerPWM(i);

		if(i == 100)
			flag = 1;
		else if(i == 0)
			flag = 0;

		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13, GPIO_PIN_SET);

		/* atkomentavus šią funkciją kontroleris užstringa pertraukties metu vykdomoje kodo dalyje, t. y. oranžinė lemputė nebedega, o žalia dega nuolatos */
		//HAL_Delay(1000);

		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12, GPIO_PIN_RESET);
	}
}

