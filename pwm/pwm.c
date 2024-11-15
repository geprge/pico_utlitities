#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "picoUtils.h"

#define PWM1_GPIO	16
#define PWM2_GPIO	17
#define PWM3_GPIO	18
#define PWM4_GPIO	19

#define PWM1_SLICE	0	//pwm_gpio_to_slice_num(PWM1_GPIO)
#define PWM2_SLICE	0	//pwm_gpio_to_slice_num(PWM2_GPIO)
#define PWM3_SLICE	1	//pwm_gpio_to_slice_num(PWM3_GPIO)
#define PWM4_SLICE	1	//pwm_gpio_to_slice_num(PWM4_GPIO)

#define PWM1_CHANNEL	PWM_CHAN_A	//pwm_gpio_to_channel(PWM1_GPIO)
#define PWM2_CHANNEL	PWM_CHAN_B	//pwm_gpio_to_channel(PWM2_GPIO)
#define PWM3_CHANNEL	PWM_CHAN_A	//pwm_gpio_to_channel(PWM3_GPIO)
#define PWM4_CHANNEL	PWM_CHAN_B	//pwm_gpio_to_channel(PWM4_GPIO)

//Setting 100KHz as PWM module clock
#define PWM_MODULE_CLOCK_HZ	(uint)100000
#define PWM_FREQ_HZ	(uint)1000
#define PWM_PERIOD_TICKS	(PWM_MODULE_CLOCK_HZ/PWM_FREQ_HZ)


#define GET_SLICE_FROM_CMD(x) 	(x=='a')?PWM1_SLICE:\
								(x=='b')?PWM2_SLICE:\
								(x=='c')?PWM3_SLICE:\
								(x=='d')?PWM4_SLICE:\
								-1
#define GET_CHANNEL_FROM_CMD(x) (x=='a')?PWM1_CHANNEL:\
								(x=='b')?PWM2_CHANNEL:\
								(x=='c')?PWM3_CHANNEL:\
								(x=='d')?PWM4_CHANNEL:\
								-1
#define GET_PWM_INDEX(x)	(uint)((x)-'a')
#define GET_PWM_NUMBER(x)	(uint)((x)-'a'+1)

int gPwmLevel[4]={0,0,0,0};

void processCmd(char *cmdBuff){
	int tempInt,buffLen=0;
//	printf("\nDEBUG:Cmd received %s",cmdBuff);

	while(cmdBuff[buffLen]!=0)buffLen++;	//to obtain string length

	switch(cmdBuff[0]){
		case 'a':
		case 'b':
		case 'c':
		case 'd':
			if(cmdBuff[1]=='?'){
				printf("\nPWM%d Level=%d\n",GET_PWM_NUMBER( cmdBuff[0] )\
											 ,gPwmLevel[GET_PWM_INDEX(cmdBuff[0]) ]\
						);
			}
			else{
				tempInt=string2Int(&cmdBuff[1],buffLen-1);
				if( (tempInt < PWM_PERIOD_TICKS) && (tempInt >= 0) ){
					gPwmLevel[cmdBuff[0]-'a']=tempInt;
					pwm_set_chan_level(GET_SLICE_FROM_CMD(cmdBuff[0]),GET_CHANNEL_FROM_CMD(cmdBuff[0]),tempInt);
					printf("\nLevel updated for PWM%d to %d (T=%d)\n",GET_PWM_NUMBER(cmdBuff[0]),tempInt,PWM_PERIOD_TICKS);
				}
				else{
					printf("\nExpected Value range (0:%d)!!\n",PWM_PERIOD_TICKS);
				}
 			}
		break;
		case '?':
			switch(cmdBuff[1]){
				case '1':
					printf("\nSystem Clock is %ld\n",clock_get_hz(clk_sys));
				break;
				case '2':
					printf("\nPWM PERIOD_TICKS is %d\n",PWM_PERIOD_TICKS);
				break;
			}
		break;
	}//switch for command type 
}

int main(){
	char cmdBuffer[64];	//Creating a buffer to store the command received
	pwm_config	c;
	uint div=(long)clock_get_hz(clk_sys)/PWM_MODULE_CLOCK_HZ;

    stdio_init_all();

	gpio_set_function_masked((1<<PWM1_GPIO)|(1<<PWM2_GPIO)|(1<<PWM3_GPIO)|(1<<PWM4_GPIO),GPIO_FUNC_PWM);

	sleep_ms(10000); //to give time for USB -Serial to get connected by user

	printf("\n Start level 0\nSystem clk div=%d\nPWM-PERIOD-TICKS %d \n",div,PWM_PERIOD_TICKS);
	c=pwm_get_default_config();
	pwm_config_set_clkdiv_int(&c,(uint)div);
	pwm_config_set_clkdiv_mode(&c,PWM_DIV_FREE_RUNNING);

	pwm_init(PWM1_SLICE,&c,true);	//start is true
	pwm_init(PWM3_SLICE,&c,true);	//start is true

	pwm_set_wrap(PWM1_SLICE,PWM_PERIOD_TICKS);
	pwm_set_wrap(PWM3_SLICE,PWM_PERIOD_TICKS);

	pwm_set_both_levels(PWM1_SLICE,0,0);
	pwm_set_both_levels(PWM3_SLICE,0,0);

	pwm_set_enabled(PWM1_SLICE,true);
	pwm_set_enabled(PWM3_SLICE,true);

    while (true) {
		if(chkUsbCmd(cmdBuffer))
			processCmd(cmdBuffer);
	}
}
