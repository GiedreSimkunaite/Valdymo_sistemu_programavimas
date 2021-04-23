#include <stm32f4xx_hal.h>

#ifdef __cplusplus
extern "C"
#endif

void SysTick_Handler(void);

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

static void SystemClock_Config(void)
{
	/* Vidinio timer'io konfigūracija */

    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 288;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 6;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
    SystemCoreClockUpdate();

    if (HAL_GetREVID() == 0x1001)
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
}

ADC_HandleTypeDef g_AdcHandle;

void ConfigureADC()
{
	/* ADC inicializavimas */
    GPIO_InitTypeDef gpioInit;

    __GPIOC_CLK_ENABLE();
    __ADC1_CLK_ENABLE();

    gpioInit.Pin = GPIO_PIN_1;
    gpioInit.Mode = GPIO_MODE_ANALOG;
    gpioInit.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &gpioInit);

    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

    ADC_ChannelConfTypeDef adcChannel;

    g_AdcHandle.Instance = ADC1;

    g_AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    g_AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
    g_AdcHandle.Init.ScanConvMode = DISABLE;
    g_AdcHandle.Init.ContinuousConvMode = ENABLE;
    g_AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    g_AdcHandle.Init.NbrOfDiscConversion = 0;
    g_AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    g_AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    g_AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_AdcHandle.Init.NbrOfConversion = 1;
    g_AdcHandle.Init.DMAContinuousRequests = ENABLE;
    g_AdcHandle.Init.EOCSelection = DISABLE;

    HAL_ADC_Init(&g_AdcHandle);

    adcChannel.Channel = ADC_CHANNEL_11;
    adcChannel.Rank = 1;
    adcChannel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    adcChannel.Offset = 0;

    if (HAL_ADC_ConfigChannel(&g_AdcHandle, &adcChannel) != HAL_OK)
    {
        asm("bkpt 255");
    }
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

void InitializeLED12()
{
	/* Inicializuojamas GPIOD12 (žalias led'as) su PWM išėjimu */
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_LED12;
	GPIO_LED12.Pin = GPIO_PIN_12;
	GPIO_LED12.Mode = GPIO_MODE_AF_PP;
	GPIO_LED12.Pull = GPIO_NOPULL;
	GPIO_LED12.Speed = GPIO_SPEED_LOW;
	GPIO_LED12.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD, &GPIO_LED12);
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
	// TIM_CHANNEL_X parenkamas priklausomai nuo GPIO pin'o numerio, kuris nustatytas PWM režimu
	HAL_TIM_PWM_ConfigChannel(&s_TimerInstance, &sConfigOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&s_TimerInstance, TIM_CHANNEL_1);
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

uint32_t g_ADCValue;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    ConfigureADC();
    InitializeTimer();
	InitializeTimerPWM(0);
	InitializeLED12();
	InitializeLED14();

	int i = 1;
	int flag = 1;
	int on = 1;
	int g_MeasurementNumber=0;

	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14, GPIO_PIN_RESET);

    HAL_ADC_Start(&g_AdcHandle);
    for (;;)
    {
    	/* Žiūrima ar neįvyko ADC timeout'as */

        if (HAL_ADC_PollForConversion(&g_AdcHandle, 1000000) == HAL_OK)
        {
        	/* tikrinama i reiškmė */
        	switch(i)
        	{
        		case 0:
        			on = 1;
					flag = 0;
				break;
        		case 25:
					on = 0;
				break;
        		case 50:
					on = 1;
				break;
				case 75:
					on = 0;
				break;
        		case 100:
        			on = 1;
        			flag = 1;
        		break;
        	}

        	/* Gaunama ADC keitiklio vertė */
            g_ADCValue = HAL_ADC_GetValue(&g_AdcHandle);

            /* Pasukus 10K potenciomentrą iki pusės (padidinus įtampą) yra peržengiama ADC keitiklio vertė ir pradeda įsižiebti/užgesti žalias ir mirksėti raudonas led'ai*/
            /* Kol ADC keitiklio riba neperžengta žalias ir raudonas led'ai nekeičia savo būsenos/užšąla */
            /* Potenciomentras jungiamas prie GND, VDD ir PC1 pin'o*/
            if(g_ADCValue>2100)
            {
            	g_MeasurementNumber++;
            	if(g_MeasurementNumber>2400)
            	{
					if(flag==1)
						i--;
					else
						i++;

					if(on == 1)
						HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14, GPIO_PIN_SET);
					else
						HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14, GPIO_PIN_RESET);

					g_MeasurementNumber = 0;
            	}
            }

            InitializeTimerPWM(i);
        }
    }
}

