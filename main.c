/**
 * Tomáš Matìjka
 * 18.3.2018
 */

/**

Program AD pøevodníku.

 * Je zde definována funkce pro inicializaci a nastavení AD pøevodníku
 * Ta obsahuje nastavení gpio, zdroje hodin a prescaleru, nastavení zarovnání dat a rozlišení do výstupního registru,
 * zvolený single mód(pøevod jen na trigger), druh triggeru(softwarový), konfigurace sekvenceru - ten se stará o
 * pøepínání kanálu.
 * Nastavení samotných kanálù. U kanálù které jsou pøiøazené na pevno k vývodùm lze nastavit tzv. RANK.
 * RANK je vlastnì pozice ve stavu sekvenceru. Je možné si udìlat vlastní posloupnost ve ètení jednotlivých kanálù.
 * Dále se nastavuje sampling time, to je èas kdy se pøipojí pøes multiplexer daný kanál a èeká než pøevodník zaène pøevádìt.
 * Je to kvùli nabití vnitøního kondenzátoru, velmi dùležité pøi mìøení rychlých zmìn signálu(vysoké vzorkování)
 * Na konci funkce je power managment-je vypnut deep power mode, a aktivován interní regulátor a èeká se na jeho ustálení.
 * Dále pak kalibrace pøevodníku a jeho úplné povolení.
 *
 * Funkce ADC_read() už jen softwarovì natrigruje pøevodník a èeká na vlajku o ukonèení pøevodu, následnì vyète data.
 *
 * Použité dokumenty:
 * RM0394: STM32L43xxx STM32L44xxx STM32L45xxx STM32L46xxx advanced ARM®-based 32-bit MCUs
 *
 */


#include "main.h"
#include "Periph_Init.h"
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_bus.h"
#include "TomLib_SYS.h"
#include "stm32l4xx.h"

void ADC_Init(){
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	LL_GPIO_SetPinMode(GPIOC,LL_GPIO_PIN_0,LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinPull(GPIOC,LL_GPIO_PIN_0,LL_GPIO_PULL_NO);

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
	//ADC_COMMON je base adresa registrù která je spoleèná pro všechny prvky ADC pøevodníku
	//zde se definuje prescaler a clock

	ADC_Calibration_Power();


	ADC1_COMMON->CCR=ADC_CCR_PRESC_0;
	LL_ADC_SetDataAlignment(ADC1,LL_ADC_DATA_ALIGN_RIGHT);
	LL_ADC_SetResolution(ADC1,LL_ADC_RESOLUTION_12B);
	LL_ADC_REG_SetContinuousMode(ADC1,LL_ADC_REG_CONV_SINGLE);
	LL_ADC_REG_SetTrigSource(ADC1,LL_ADC_REG_TRIG_SW_START);
	LL_ADC_REG_SetSequencerLength(ADC1,LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);
	LL_ADC_REG_SetDMATransfer(ADC1,LL_ADC_REG_DMA_TRANSFER_NONE);

	//vrefin
	LL_ADC_SetCommonPathInternalCh(ADC1_COMMON,LL_ADC_PATH_INTERNAL_VREFINT);
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_247CYCLES_5);

	//set channel pin
	LL_ADC_SetChannelSingleDiff(ADC1,LL_ADC_CHANNEL_1,LL_ADC_SINGLE_ENDED);
	LL_ADC_REG_SetSequencerRanks(ADC1,LL_ADC_REG_RANK_2,LL_ADC_CHANNEL_1);
	LL_ADC_SetChannelSamplingTime(ADC1,LL_ADC_CHANNEL_1,LL_ADC_SAMPLINGTIME_640CYCLES_5);


	ADC_Enable_procedure();


}


void ADC_Enable_procedure(){
	//enable procedure
		LL_ADC_ClearFlag_ADRDY(ADC1);
		LL_ADC_Enable(ADC1);
		while(LL_ADC_IsActiveFlag_ADRDY(ADC1)==RESET);
}

void ADC_Calibration_Power(){
	//power managment & calibration procedure
		LL_ADC_DisableDeepPowerDown(ADC1);
		ADC1->CR|=ADC_CR_ADVREGEN ;
		TL_TIM6_Delay(10);
		LL_ADC_StartCalibration(ADC1,LL_ADC_SINGLE_ENDED);
		while(READ_BIT(ADC1->CR,ADC_CR_ADCAL));
}





uint32_t ADC_read(){
	GPIOB->ODR|=LL_GPIO_PIN_13;
	while(LL_ADC_IsActiveFlag_EOC(ADC1)==RESET){}
	return READ_REG(ADC1->DR);


}


uint32_t rawdata;
uint32_t common_reg;
float voltage;
uint32_t vrefin_data;
uint32_t calib_val;
float f;


int main(void) {
	calib_val=READ_REG(*VREFINT_CAL_ADDR);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	LL_GPIO_SetPinMode(GPIOB,LL_GPIO_PIN_13,LL_GPIO_MODE_OUTPUT);

	SystemClock_Config();
	TIM6_Init();
	ADC_Init();



	while (1) {
		common_reg=READ_REG(ADC1_COMMON->CCR);
		rawdata=0;
		vrefin_data=0;
		TL_TIM6_Delay(1000000);

		LL_ADC_REG_StartConversion(ADC1);

		vrefin_data=ADC_read();
		rawdata=ADC_read();
		LL_ADC_ClearFlag_EOS(ADC1);

		voltage=(3.0*calib_val*rawdata/((float)vrefin_data*4095));

		f=(float)5/2;


	}
}




