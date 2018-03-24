/**
 * Tom� Mat�jka
 * 18.3.2018
 */

/**

Program AD p�evodn�ku.

 * Je zde definov�na funkce pro inicializaci a nastaven� AD p�evodn�ku
 * Ta obsahuje nastaven� gpio, zdroje hodin a prescaleru, nastaven� zarovn�n� dat a rozli�en� do v�stupn�ho registru,
 * zvolen� single m�d(p�evod jen na trigger), druh triggeru(softwarov�), konfigurace sekvenceru - ten se star� o
 * p�ep�n�n� kan�lu.
 * Nastaven� samotn�ch kan�l�. U kan�l� kter� jsou p�i�azen� na pevno k v�vod�m lze nastavit tzv. RANK.
 * RANK je vlastn� pozice ve stavu sekvenceru. Je mo�n� si ud�lat vlastn� posloupnost ve �ten� jednotliv�ch kan�l�.
 * D�le se nastavuje sampling time, to je �as kdy se p�ipoj� p�es multiplexer dan� kan�l a �ek� ne� p�evodn�k za�ne p�ev�d�t.
 * Je to kv�li nabit� vnit�n�ho kondenz�toru, velmi d�le�it� p�i m��en� rychl�ch zm�n sign�lu(vysok� vzorkov�n�)
 * Na konci funkce je power managment-je vypnut deep power mode, a aktivov�n intern� regul�tor a �ek� se na jeho ust�len�.
 * D�le pak kalibrace p�evodn�ku a jeho �pln� povolen�.
 *
 * Funkce ADC_read() u� jen softwarov� natrigruje p�evodn�k a �ek� na vlajku o ukon�en� p�evodu, n�sledn� vy�te data.
 *
 * Pou�it� dokumenty:
 * RM0394: STM32L43xxx STM32L44xxx STM32L45xxx STM32L46xxx advanced ARM�-based 32-bit MCUs
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
	//ADC_COMMON je base adresa registr� kter� je spole�n� pro v�echny prvky ADC p�evodn�ku
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




