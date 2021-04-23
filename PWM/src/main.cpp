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

void InitializeLED()
{
	/* Inicializuojamas GPIOD15 (mėlynas led'as) su PWM išėjimu*/
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_15;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
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
	InitializeLED();
	InitializeTimer();
	InitializeTimerPWM(0);

	int i = 0;
	int flag = 1;

	for(;;)
	{
		/* Mėlynas led'as įsižiebia/užgęsta */

		InitializeTimerPWM(i);

		if(i == 100)
			flag = 1;
		else if(i == 0)
			flag = 0;

		if(flag == 0)
			i++;
		else if(flag == 1)
			i--;

		HAL_Delay(10);
	}
}
