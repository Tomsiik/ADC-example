#include "main.h"
#include "Periph_Init.h"
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_bus.h"
#include "TomLib_SYS.h"

void ADC_Init(){
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	LL_GPIO_SetPinMode(GPIOC,LL_GPIO_PIN_0,LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinPull(GPIOC,LL_GPIO_PIN_0,LL_GPIO_PULL_NO);

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
	//ADC_COMMON je base adresa registrù která je spoleèná pro všechny prvky ADC pøevodníku
	//zde se definuje prescaler a clock
	ADC1_COMMON->CCR=0x00000000;
	LL_ADC_SetDataAlignment(ADC1,LL_ADC_DATA_ALIGN_RIGHT);
	LL_ADC_SetResolution(ADC1,LL_ADC_RESOLUTION_12B);
	LL_ADC_REG_SetContinuousMode(ADC1,LL_ADC_REG_CONV_SINGLE);
	LL_ADC_REG_SetTrigSource(ADC1,LL_ADC_REG_TRIG_SW_START);
	LL_ADC_REG_SetSequencerLength(ADC1,LL_ADC_REG_SEQ_SCAN_DISABLE);
	LL_ADC_REG_SetDMATransfer(ADC1,LL_ADC_REG_DMA_TRANSFER_NONE);


	LL_ADC_SetChannelSingleDiff(ADC1,LL_ADC_CHANNEL_1,LL_ADC_SINGLE_ENDED);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_247CYCLES_5);
	LL_ADC_DisableDeepPowerDown(ADC1);
	ADC1->CR|=ADC_CR_ADVREGEN ;
	TL_TIM6_Delay(10);

	LL_ADC_Enable(ADC1);




}

uint32_t ADC_read(){
	LL_ADC_REG_StartConversion(ADC1);
	while(LL_ADC_IsActiveFlag_EOC(ADC1)==RESET){}
	return READ_REG(ADC1->DR);

}


volatile int rawdata;
volatile int common_reg;
float voltage;
int main(void) {
	SystemClock_Config();
	TIM6_Init();
	ADC_Init();



	while (1) {
		rawdata=ADC_read();
		TL_TIM6_Delay(100);
		common_reg=READ_REG(ADC1_COMMON->CCR);
		voltage=(((float)rawdata)/4096)*3.250;


	}
}




