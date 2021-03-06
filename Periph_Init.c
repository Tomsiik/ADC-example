#include "Periph_Init.h"


void SystemClock_Config(void) {
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	LL_RCC_HSE_Enable();
	LL_RCC_HSE_EnableBypass();

	while (LL_RCC_HSE_IsReady() != 1) {}

	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_1, 16,LL_RCC_PLLR_DIV_4);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();

	while (LL_RCC_PLL_IsReady() != 1) {}
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetI2CClockSource(LL_RCC_I2C2_CLKSOURCE_PCLK1);

	LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_SYSCLK, LL_RCC_MCO1_DIV_1);

	SystemCoreClockUpdate();

	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
	SysTick_Config(SystemCoreClock / 100000);
	NVIC_SetPriority(SysTick_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	LL_SYSTICK_DisableIT();

	LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);

}


void GPIO_Init(void)
{ 	/*Povolení hodin do GPIO periferií*/
	WRITE_REG(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN) ;


	/*USBPWREN Init*/
	LL_GPIO_SetPinMode(GPIOA,USBPWREN,LL_GPIO_MODE_INPUT);

	/*USART2 GPIO Init*/
	LL_GPIO_SetPinMode(GPIOA,USART2_TX,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOA,USART2_RX,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA,USART2_TX,LL_GPIO_SPEED_FREQ_MEDIUM);
	LL_GPIO_SetPinSpeed(GPIOA,USART2_RX,LL_GPIO_SPEED_FREQ_MEDIUM);
	LL_GPIO_SetAFPin_0_7(GPIOA,USART2_TX,LL_GPIO_AF_7);
	LL_GPIO_SetAFPin_0_7(GPIOA,USART2_RX,LL_GPIO_AF_7);

	/*Analog SENS GPIO Init*/
	LL_GPIO_SetPinMode(GPIOA,SENS1,LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(GPIOA,SENS2,LL_GPIO_MODE_ANALOG);

	/*Power EN GPIO Init*/
	LL_GPIO_SetPinMode(GPIOA,_5V_EN,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOA,_3V3_EN,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(GPIOA,_5V_EN,LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinSpeed(GPIOA,_3V3_EN,LL_GPIO_SPEED_FREQ_LOW);

	/*MCO Init*/
	LL_GPIO_SetPinMode(GPIOA,MCO,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA,MCO,LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetAFPin_0_7(GPIOA,MCO,LL_GPIO_AF_0);

	/*USART1 GPIO Init*/
	LL_GPIO_SetPinMode(GPIOA,USART1_TX,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOA,USART1_RX,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA,USART1_TX,LL_GPIO_SPEED_FREQ_MEDIUM);
	LL_GPIO_SetPinSpeed(GPIOA,USART1_RX,LL_GPIO_SPEED_FREQ_MEDIUM);
	LL_GPIO_SetAFPin_0_7(GPIOA,USART1_TX,LL_GPIO_AF_7);
	LL_GPIO_SetAFPin_0_7(GPIOA,USART1_RX,LL_GPIO_AF_7);

	/*ALERT Init*/
	LL_GPIO_SetPinMode(GPIOB,ALERT,LL_GPIO_MODE_INPUT);

	/*WP Init*/
	LL_GPIO_SetPinMode(GPIOB,WP,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(GPIOB,WP,LL_GPIO_SPEED_FREQ_MEDIUM);

	/*I2C2 GPIO Init*/
	LL_GPIO_SetPinMode(GPIOB,I2C2_SCL,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOB,I2C2_SDA,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB,I2C2_SCL,LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinSpeed(GPIOB,I2C2_SDA,LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOB,I2C2_SCL,LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinOutputType(GPIOB,I2C2_SDA,LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetAFPin_0_7(GPIOB,I2C2_SCL,LL_GPIO_AF_4);
	LL_GPIO_SetAFPin_0_7(GPIOB,I2C2_SDA,LL_GPIO_AF_4);

	/*LGHTNG_CS Init*/
	LL_GPIO_SetPinMode(GPIOB,LGHTNG_CS,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(GPIOB,LGHTNG_CS,LL_GPIO_SPEED_FREQ_MEDIUM);

	/*SPI2 GPIO Init*/
	LL_GPIO_SetPinMode(GPIOB,SPI2_SCK,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOB,SPI2_MISO,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOB,SPI2_MOSI,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOB,SPI2_SCK,LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinSpeed(GPIOB,SPI2_MISO,LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinSpeed(GPIOB,SPI2_MOSI,LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetAFPin_0_7(GPIOB,SPI2_SCK,LL_GPIO_AF_5);
	LL_GPIO_SetAFPin_0_7(GPIOB,SPI2_MISO,LL_GPIO_AF_5);
	LL_GPIO_SetAFPin_0_7(GPIOB,SPI2_MOSI,LL_GPIO_AF_5);

	/*ULED1 */
	LL_GPIO_SetPinMode(GPIOC,ULED1,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(GPIOC,ULED1,LL_GPIO_SPEED_FREQ_LOW);

	/*ULED2 */
	LL_GPIO_SetPinMode(GPIOC,ULED2,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(GPIOC,ULED2,LL_GPIO_SPEED_FREQ_LOW);

	/*LGHTNG_IRQ*/
	LL_GPIO_SetPinMode(GPIOC,LGHTNG_IRQ,LL_GPIO_MODE_INPUT);


}

void SPI2_Init(void)
{
  LL_SPI_InitTypeDef SPI_InitStruct;
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV64;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_SetRxFIFOThreshold(SPI2,LL_SPI_RX_FIFO_TH_QUARTER);
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  LL_SPI_Enable(SPI2);

}

void USART1_Init(void){

	LL_USART_InitTypeDef USART_InitStruct;
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	USART_InitStruct.BaudRate=115200;
	USART_InitStruct.DataWidth=LL_USART_DATAWIDTH_8B;
	USART_InitStruct.HardwareFlowControl=LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling=LL_USART_OVERSAMPLING_8;
	USART_InitStruct.Parity=LL_USART_PARITY_NONE;
	USART_InitStruct.StopBits=LL_USART_STOPBITS_1;
	USART_InitStruct.TransferDirection=LL_USART_DIRECTION_TX_RX;
	LL_USART_DisableSCLKOutput(USART1);
	LL_USART_Init(USART1,&USART_InitStruct);
	LL_USART_Enable(USART1);

}


void USART2_Init(void){

	LL_USART_InitTypeDef USART_InitStruct;
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

	USART_InitStruct.BaudRate=115200;
	USART_InitStruct.DataWidth=LL_USART_DATAWIDTH_8B;
	USART_InitStruct.HardwareFlowControl=LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling=LL_USART_OVERSAMPLING_8;
	USART_InitStruct.Parity=LL_USART_PARITY_NONE;
	USART_InitStruct.StopBits=LL_USART_STOPBITS_1;
	USART_InitStruct.TransferDirection=LL_USART_DIRECTION_TX_RX;
	LL_USART_DisableSCLKOutput(USART2);
	LL_USART_Init(USART2,&USART_InitStruct);
	LL_USART_Enable(USART2);

}

void I2C2_Init(void){
	LL_I2C_InitTypeDef I2C_InitStruct;
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
	LL_I2C_Disable(I2C2);
	I2C_InitStruct.AnalogFilter=LL_I2C_ANALOGFILTER_ENABLE;
	I2C_InitStruct.DigitalFilter=0x00;
	I2C_InitStruct.OwnAddrSize=LL_I2C_OWNADDRESS1_7BIT;
	I2C_InitStruct.OwnAddress1=0x02;
	I2C_InitStruct.PeripheralMode=LL_I2C_MODE_I2C;
	I2C_InitStruct.Timing=0x00B07CB4;
	I2C_InitStruct.TypeAcknowledge=LL_I2C_NACK;
	LL_I2C_Init(I2C2,&I2C_InitStruct);
	LL_I2C_Enable(I2C2);

}

