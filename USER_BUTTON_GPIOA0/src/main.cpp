#include <stm32f4xx_hal.h>

#ifdef __cplusplus
extern "C"
#endif

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

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

void InitializeLED13()
{
	/* Inicializuojamas GPIOD13 (oranžinis led'as) kaip išėjimas */
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_LED13;
	GPIO_LED13.Pin = GPIO_PIN_13;
	GPIO_LED13.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_LED13.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_LED13);}

void InitializeBTN()
{
	/* Inicializuojamas User Button (GPIOA0) */
	__GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_BTN;
	GPIO_BTN.Pin = GPIO_PIN_0;
	GPIO_BTN.Mode = GPIO_MODE_INPUT;
	GPIO_BTN.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_BTN);
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


int main(void)
{
	HAL_Init();
	InitializeLED15();
	InitializeLED13();
	InitializeTimer();
	InitializeTimerPWM(0);
	InitializeBTN();

	int i = 0;
	int flag = 1;

	for(;;)
	{
		/* Paspaudus User Button mėlynas led'as įsižiebia ryškiau/blankiau, oranžinis led'as išsijungia */
		/* Atleidus User Button mėlyno led'o ryškumas nekinta, oranžinis led'as įsijungia */
		InitializeTimerPWM(i);

		if(i == 100)
			flag = 1;
		else if(i == 0)
			flag = 0;

		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13, GPIO_PIN_SET);

		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
		{
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13, GPIO_PIN_RESET);

			if(flag == 0)
				i++;
			else
				i--;
		}

		HAL_Delay(10);
	}
}

