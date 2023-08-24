/*
 * Copyright © 2019 Patrick Rademacher
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

//Patrick Rademacher (c) 2019
//Professor Bryant York
//Portland State University
//March 12, 2019

// CC-by-www.Electrosmash.com open-source project.

#include "delaylines.h" 
#include <stdio.h>
#include <bcm2835.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define Input Pins
#define PUSH1                   RPI_GPIO_P1_08          //GPIO14
#define PUSH2                   RPI_V2_GPIO_P1_38       //GPIO20 
#define TOGGLE_SWITCH   RPI_V2_GPIO_P1_32       //GPIO12
#define FOOT_SWITCH     RPI_GPIO_P1_10          //GPIO15
#define LED                     RPI_V2_GPIO_P1_36       //GPIO16
#define MAX_LOOP 8000000
#define sample_delay 10
#define PI 3.14159265

#define LFO_SIN_LEN 1000
uint32_t LFO_sine_wave[LFO_SIN_LEN];

uint8_t FOOT_SWITCH_val;
uint8_t TOGGLE_SWITCH_val;
uint8_t PUSH1_val;
uint8_t PUSH2_val;
uint32_t loop_buffer[MAX_LOOP];
uint32_t phase_loop_buffer[MAX_LOOP]; 
uint32_t flang_loop_buffer[MAX_LOOP];
uint32_t pog_loop_buffer[MAX_LOOP];
uint32_t rad_loop_buffer[MAX_LOOP];
int flang_delay_1 = 10;
int flang_delay_2 = 20;
int flang_delay_3 = 30;
int flang_delay_4 = 40;
int flang_delay_5 = 50;
int flang_delay_6 = 60;
int flang_delay_7 = 70;
int flang_delay_8 = 80;
int flang_delay_9 = 90;
int flang_delay_10 = 100;

int main(int argc, char** argv) {
    for (int i = 0; i < LFO_SIN_LEN; i++) {
        double sin_sample = sin(2.0 * M_PI * i / (float) LFO_SIN_LEN);
        LFO_sine_wave[i] = floor((1 << 12) * (1.0 + sin_sample));
    }
    uint32_t loop_cue = 0;
    uint32_t loop_read = 0;
    uint32_t no_loop_output = 0;
    uint32_t with_loop_output = 0;
    uint32_t read_timer = 0;
    uint32_t final_loop_read = 0;
    uint32_t dry_signal = 0;
    uint32_t phased_signal = 0;
    int number_of_filters = 16;
    double * wet_signal = (double*)malloc((number_of_filters + 1)*sizeof(double));
    double ** IST = (double**)malloc(number_of_filters * sizeof(double*));
    double ** OST = (double**)malloc(number_of_filters * sizeof(double*));
    int * ISC = (int*)malloc(number_of_filters*sizeof(int));
    int * OSC = (int*)malloc(number_of_filters*sizeof(int));
    double gain = 0;
	uint32_t flang_signal = 0;
    double * flang_table_1 = (double*)malloc(10 * sizeof(double));
    double * flang_table_2 = (double*)malloc(20 * sizeof(double));
    double * flang_table_3 = (double*)malloc(30 * sizeof(double));
    double * flang_table_4 = (double*)malloc(40 * sizeof(double));
    double * flang_table_5 = (double*)malloc(50 * sizeof(double));
    double * flang_table_6 = (double*)malloc(60 * sizeof(double));
    double * flang_table_7 = (double*)malloc(70 * sizeof(double));
    double * flang_table_8 = (double*)malloc(80 * sizeof(double));
    double * flang_table_9 = (double*)malloc(90 * sizeof(double));
    double * flang_table_10 = (double*)malloc(100 * sizeof(double));
    double * flang_wet_signal = (double*)malloc(10 * sizeof(double));
    int * ISC_2 = (int*)malloc(10 * sizeof(int));
    int delay_switch_counter = 0;
    double flang_gain = .5;
    int index = 0;
    double LFO_rate = 200;
    int LFO_sample = 0;
    double LFO_counter = 1;
    uint32_t delay_buffer[3000];
    uint32_t pog_signal = 0;
    uint32_t pog_value = 1;
    uint32_t delay_depth = 5000; //default starting delay is 100000 is 0.25 sec approx.
    int  delay_read[6] = {0, 0, 0, 0, 0, 0};
    double delay_read_double[6] = {0, 0, 0, 0, 0, 0};
    int delay_write = 0;
    double val = PI/180;
    double rad_signal = 0;
    int i = 0;
    int j = 0;
    int loop_effect = 0;
    int signal_effect = 0;
    for (i = 0; i < number_of_filters; i++)
    {
        IST[i] = (double *)malloc(sample_delay * sizeof(double));
        OST[i] = (double *)malloc(sample_delay * sizeof(double));
        wet_signal[i] = 0;
        ISC[i] = 0;
        OSC[i] = 0;
            for (j = 0; j < sample_delay; j++)
            {
                IST[i][j] = 0;
                OST[i][j] = 0;

            }
    }

    for (i = 0; i < 10; i++)
    {
        flang_table_1[i] = 0;
    }

    for (i = 0; i < 20; i++)
    {
    	flang_table_2[i] = 0;
    }

    for (i = 0; i < 30; i++)
    {
    	flang_table_3[i] = 0;
    }

    for (i = 0; i < 40; i++)
    { 
        flang_table_4[i] = 0;
    }

    for (i = 0; i < 50; i++)
    { 
    	flang_table_4[i] = 0;
    }

    for (i = 0; i < 60; i++)
    { 
    	flang_table_5[i] = 0;
    }

    for (i = 0; i < 70; i++)
    { 
        flang_table_5[i] = 0;
    }

    for (i = 0; i < 80; i++)
    { 
        flang_table_6[i] = 0;
    }

    for (i = 0; i < 90; i++)
    {    
        flang_table_9[i] = 0;
    }

    for (i = 0; i < 100; i++)
    { 
        flang_table_10[i] = 0;
    }

    for (i = 0; i < 10; i++)
    { 
        ISC_2[i] = 0;
        flang_wet_signal[i] = 0;
    }




    // Start the BCM2835 Library to access GPIO.
    if (!bcm2835_init())
    {
		printf("bcm2835_init failed. Are you running as root??\n");
       return 1;
	}
        // Start the SPI BUS.
    if (!bcm2835_spi_begin())
    {
		printf("bcm2835_spi_begin failed. Are you running as root??\n");
        return 1;
	}
 
//define PWM    
    bcm2835_gpio_fsel(18,BCM2835_GPIO_FSEL_ALT5 ); //PWM0 signal on GPIO18    
    bcm2835_gpio_fsel(13,BCM2835_GPIO_FSEL_ALT0 ); //PWM1 signal on GPIO13    
    bcm2835_pwm_set_clock(2); // Max clk frequency (19.2MHz/2 = 9.6MHz)
    bcm2835_pwm_set_mode(0,1 , 1); //channel 0, markspace mode, PWM enabled. 
    bcm2835_pwm_set_range(0,64);   //channel 0, 64 is max range (6bits): 9.6MHz/64=150KHz switching PWM freq.
    bcm2835_pwm_set_mode(1, 1, 1); //channel 1, markspace mode, PWM enabled.
    bcm2835_pwm_set_range(1,64);   //channel 0, 64 is max range (6bits): 9.6MHz/64=150KHz switching PWM freq.
 
        //define SPI bus configuration
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);    // 4MHz clock with _64 
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
 
    uint8_t mosi[10] = { 0x01, 0x00, 0x00 }; //12 bit ADC read 0x08 ch0, - 0c for ch1 
    uint8_t miso[10] = { 0 };
 
    //Define GPIO pins configuration
    bcm2835_gpio_fsel(PUSH1, BCM2835_GPIO_FSEL_INPT);                   //PUSH1 button as input
    bcm2835_gpio_fsel(PUSH2, BCM2835_GPIO_FSEL_INPT);                       //PUSH2 button as input
    bcm2835_gpio_fsel(TOGGLE_SWITCH, BCM2835_GPIO_FSEL_INPT);       //TOGGLE_SWITCH as input
    bcm2835_gpio_fsel(FOOT_SWITCH, BCM2835_GPIO_FSEL_INPT);         //FOOT_SWITCH as input
    bcm2835_gpio_fsel(LED, BCM2835_GPIO_FSEL_OUTP);                         //LED as output
 
    bcm2835_gpio_set_pud(PUSH1, BCM2835_GPIO_PUD_UP);           //PUSH1 pull-up enabled   
    bcm2835_gpio_set_pud(PUSH2, BCM2835_GPIO_PUD_UP);           //PUSH2 pull-up enabled 
    bcm2835_gpio_set_pud(TOGGLE_SWITCH, BCM2835_GPIO_PUD_UP);   //TOGGLE_SWITCH pull-up enabled 
    bcm2835_gpio_set_pud(FOOT_SWITCH, BCM2835_GPIO_PUD_UP);     //FOOT_SWITCH pull-up enabled 
 
 	while(1)
 	{
        //read 12 bits ADC
    	bcm2835_spi_transfernb(mosi, miso, 3);
    	dry_signal = miso[2] + ((miso[1] & 0x0F) << 8); 
 
                //Read the PUSH buttons every 50000 times (0.25s) to save resources.
        read_timer++;
        if (read_timer==3000)
        {
        	read_timer=0;
        	PUSH1_val = bcm2835_gpio_lev(PUSH1);
        	PUSH2_val = bcm2835_gpio_lev(PUSH2);
    		TOGGLE_SWITCH_val = bcm2835_gpio_lev(TOGGLE_SWITCH);
        	FOOT_SWITCH_val = bcm2835_gpio_lev(FOOT_SWITCH);
        	//light the effect when the footswitch is activated.
        	bcm2835_gpio_write(LED,!FOOT_SWITCH_val); 
                                

                                
			if (PUSH2_val == 0 && loop_cue == 0)
        	{   
        		bcm2835_delay(100); //100ms delay for buttons debouncing. 
            	printf("\nstart of loop\n");
            	loop_read = 0;
            	loop_cue = 1;
        	}

        	else if (PUSH2_val == 0 && loop_cue == 1)
        	{
        		bcm2835_delay(100); //100ms delay for buttons debouncing.
            	printf("\nend of loop\n");
            	final_loop_read = loop_read;
            	loop_read = 0;
            	loop_cue = 2;
        	}

        	else if (PUSH2_val == 0 && loop_cue == 2)
        	{
        		bcm2835_delay(100); //100ms delay for buttons debouncing.
            	loop_read = 0;
            	loop_cue = 0;
         	}

        	if (PUSH1_val==0) 
        	{               
        		bcm2835_delay(100); //100ms delay for buttons debouncing.
            	if (LFO_rate == 200)
            	{
            		LFO_rate = 10;
            	}   
            
				else if (LFO_rate >= 10 && LFO_rate <20)
            	{
            		LFO_rate += 1;
            	}
                    
				else if (LFO_rate == 20)
            	{
            		LFO_rate = 22;
            	}
            
				else if (LFO_rate == 22)
            	{
            		LFO_rate = 25;
            	}
                    
				else if (LFO_rate == 25)
            	{
            		LFO_rate = 28;
            	}
                    
				else if (LFO_rate == 28)
            	{
            		LFO_rate = 33;
            	}
                    
				else if (LFO_rate == 33)
            	{
            		LFO_rate = 40;
            	}

            	else if (LFO_rate == 40)
            	{
            		LFO_rate = 50;
            	}
                    
				else if (LFO_rate == 50)
            	{
             		LFO_rate = 66;
            	}
                    
				else if (LFO_rate == 66)
            	{
            		LFO_rate = 100;
            	}	
                    
				else if (LFO_rate == 100)
            	{
            		LFO_rate = 200;
            	}
           
				if (pog_value<12)
            	{       
            		pog_value++;
            	}
                                        
				else if (pog_value == 12)
            	{
            		pog_value = 1;
            	}                                
        	}	
		}

		LFO_counter++;
    	if (LFO_counter >= LFO_rate)
    	{
        	LFO_counter = 1;
        	LFO_sample ++;
    	}

    	if (LFO_sample == 999)
   		{
        	LFO_sample = 1;
    	}
    
		index = 0;
    	wet_signal[index] = (double) dry_signal;
    	gain = .2;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index); 
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);
    	index += 1;
    	gain += .05;
    	all_pass_filter(&wet_signal[index], &wet_signal[index + 1], gain, IST, OST, ISC, OSC, index);

    	wet_signal[index + 1] = wet_signal[index + 1] * (LFO_sine_wave[LFO_sample]/4095.0);
    	phased_signal = (uint32_t) ((wet_signal[index + 1] * 1.1) + (dry_signal * .7));
    	phased_signal = (phased_signal >> 1) * 1.4;
 
        
    	index = 0;
    	flang_gain = .95;
    	flang_wet_signal[0] = (double) dry_signal;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[1], flang_gain, flang_table_1, ISC_2, flang_delay_1, index);
    	flang_wet_signal[1] = flang_wet_signal[1] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[2], flang_gain, flang_table_2, ISC_2, flang_delay_2, index);
    	flang_wet_signal[2] = flang_wet_signal[2] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[3], flang_gain, flang_table_3, ISC_2, flang_delay_3, index);
    	flang_wet_signal[3] = flang_wet_signal[3] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[4], flang_gain, flang_table_4, ISC_2, flang_delay_4, index);
    	flang_wet_signal[4] = flang_wet_signal[4] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[5], flang_gain, flang_table_5, ISC_2, flang_delay_5, index);
    	flang_wet_signal[5] = flang_wet_signal[5] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[6], flang_gain, flang_table_6, ISC_2, flang_delay_6, index);
    	flang_wet_signal[6] = flang_wet_signal[6] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[7], flang_gain, flang_table_7, ISC_2, flang_delay_7, index);
    	flang_wet_signal[7] = flang_wet_signal[7] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[8], flang_gain, flang_table_8, ISC_2, flang_delay_8, index);
    	flang_wet_signal[8] = flang_wet_signal[8] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[9], flang_gain, flang_table_9, ISC_2, flang_delay_9, index);
    	flang_wet_signal[9] = flang_wet_signal[9] * (LFO_sine_wave[LFO_sample]/4095.0);
    	index += 1;
    	flanger(&flang_wet_signal[0], &flang_wet_signal[10], flang_gain, flang_table_10, ISC_2, flang_delay_10, index);
    	flang_wet_signal[10] = flang_wet_signal[10] * (LFO_sine_wave[LFO_sample]/4095.0);


		if (delay_switch_counter >= 1000000)
    	{
    		delay_switch_counter = 0;
    	}
    
		delay_switch_counter++;
        
		if (delay_switch_counter >= 0 && delay_switch_counter < 100000)
    	{
    		flang_signal = (uint32_t)(flang_wet_signal[1] + (.5 * dry_signal));
    	}

    	else if (delay_switch_counter >= 100000 && delay_switch_counter < 200000)
    	{
    		flang_signal = (uint32_t)(flang_wet_signal[2] + (.5 * dry_signal));
   	 	}	

    	else if (delay_switch_counter >= 200000 && delay_switch_counter < 300000)
   	 	{
    		flang_signal = (uint32_t)(flang_wet_signal[3] + (.5 * dry_signal));
    	}

		else if (delay_switch_counter >= 300000 && delay_switch_counter < 400000)
    	{
    		flang_signal = (uint32_t)(flang_wet_signal[4] + (.5 * dry_signal));
    	}

		else if (delay_switch_counter >= 400000 && delay_switch_counter < 500000)
    	{
    		flang_signal = (uint32_t)(flang_wet_signal[5] + (.5 * dry_signal));
    	}

    	else if (delay_switch_counter >= 500000 && delay_switch_counter < 600000)
    	{
    		flang_signal = (uint32_t)(flang_wet_signal[6] + (.5 * dry_signal));
    	}

    	else if (delay_switch_counter >= 600000 && delay_switch_counter < 700000)
    	{
    		flang_signal = (uint32_t)(flang_wet_signal[7] + (.5 * dry_signal));
    	}

		else if (delay_switch_counter >= 700000 && delay_switch_counter < 800000)
    	{
    		flang_signal = (uint32_t)(flang_wet_signal[8] + (.5 * dry_signal));
    	}

    	else if (delay_switch_counter >= 800000 && delay_switch_counter < 900000)
    	{
			flang_signal = (uint32_t)(flang_wet_signal[9] + (.5 * dry_signal));
    	}

    	else if (delay_switch_counter >= 900000 && delay_switch_counter < 1000000)
    	{
    		flang_signal = (uint32_t)(flang_wet_signal[10] + (.5 * dry_signal));
    	}

        

 		delay_buffer[delay_write] = dry_signal;
		delay_write++;
    
		if(delay_write >= delay_depth)
		{
			delay_write = 0;
		}


    	else if (pog_value == 1)
    	{
    		delay_read_double[0] = delay_read_double[0] + 1;
        	pog_signal = delay_buffer[delay_read[0]];
    	} 
                
		else if (pog_value == 2)
    	{
    		delay_read_double[1] = delay_read_double[1] + 1.25; //goes to the fourth
        	pog_signal = delay_buffer[delay_read[1]];
    	}
                
		else if (pog_value == 3)
    	{
    		delay_read_double[2] = delay_read_double[2] + 1.3333; //goes to the fifth
        	pog_signal = delay_buffer[delay_read[2]];
    	}
                
		else if (pog_value == 4)
    	{
    		delay_read_double[3] = delay_read_double[3] + 1.5; // goes to the seventh
        	pog_signal = delay_buffer[delay_read[3]];
    	}
                
		else if (pog_value == 5)
    	{
    	delay_read_double[4] = delay_read_double[4] + 1.6666; // goes to the ninth
        pog_signal = delay_buffer[delay_read[4]];
    	}
                
		else if (pog_value == 6)
    	{
    		delay_read_double[5] = delay_read_double[5] + 2; // goes to octave
        	pog_signal = delay_buffer[delay_read[5]];
    	}
                
		if (pog_value == 7)
    	{
    		delay_read_double[0] = delay_read_double[0] + 1;
        	delay_read_double[1] = delay_read_double[1] + 1.25; // goes to octave
        	pog_signal = (delay_buffer[delay_read[0]] + delay_buffer[delay_read[1]]) >> 1;
    	}
                
		if (pog_value == 8)
    	{
    		delay_read_double[0] = delay_read_double[0] + 1;
        	delay_read_double[2] = delay_read_double[2] + 1.3333; // goes to octave
        	pog_signal = (delay_buffer[delay_read[0]] + delay_buffer[delay_read[2]]) >> 1;
    	}
                 
		if (pog_value == 9)
    	{
    		delay_read_double[0] = delay_read_double[0] + 1;
        	delay_read_double[3] = delay_read_double[3] + 1.5; // goes to octave
        	pog_signal = (delay_buffer[delay_read[0]] + delay_buffer[delay_read[3]]) >> 1;
    	}
                 
		if (pog_value == 10)
    	{
    		delay_read_double[0] = delay_read_double[0] + 1;
        	delay_read_double[4] = delay_read_double[4] + 1.6666; // goes to octave
        	pog_signal = (delay_buffer[delay_read[0]] + delay_buffer[delay_read[4]]) >> 1;
    	}
                  
		if (pog_value == 11)
    	{
    		delay_read_double[0] = delay_read_double[0] + 1;
        	delay_read_double[5] = delay_read_double[5] + 2; // goes to octave
        	pog_signal = (delay_buffer[delay_read[0]] + delay_buffer[delay_read[5]]) >> 1;
    	}
                  
		if (pog_value == 12)
    	{
    		delay_read_double[0] = delay_read_double[0] + 1;
        	delay_read_double[1] = delay_read_double[1] + 1.25;
        	delay_read_double[2] = delay_read_double[2] + 1.333;
        	delay_read_double[3] = delay_read_double[3] + 1.5;
        	delay_read_double[4] = delay_read_double[4] + 1.6666; // goes to octave
        	pog_signal = (delay_buffer[delay_read[3]] + delay_buffer[delay_read[4]]) >> 1;
        	pog_signal = (pog_signal + delay_buffer[delay_read[0]]) >> 1;
        	pog_signal = (pog_signal + delay_buffer[delay_read[2]]) >> 1;
    	}
                        
		for (int i = 0; i < 6; i++)
    	{
    		if(delay_read_double[i] >= delay_depth) 
        	{
        		delay_read_double[i] = 0;
        	}
      	
			delay_read[i] = (int) delay_read_double[i];
    	}	
        

    	rad_signal = dry_signal * sin(dry_signal*val) * (dry_signal * cos(dry_signal*val));
    	rad_signal += dry_signal;
    	rad_signal = (uint32_t) rad_signal;
    	rad_signal = (int) rad_signal;

    	loop_read++;                
           
    	if (loop_cue == 1)
    	{
    		if (loop_read >= MAX_LOOP)
        	{
        		loop_read = 0;
        	}
     	
			loop_buffer[loop_read] = dry_signal;
     		phase_loop_buffer[loop_read] = phased_signal;
     		flang_loop_buffer[loop_read] = flang_signal;
     		pog_loop_buffer[loop_read] = pog_signal;
     		rad_loop_buffer[loop_read] = rad_signal;
    	}
        
    	else if (loop_cue == 2)
        {
        	if (loop_read == final_loop_read)
            {
            	loop_read = 0;
            }                
        }

        while (bcm2835_gpio_lev(TOGGLE_SWITCH))
        {
        	PUSH1_val= bcm2835_gpio_lev(PUSH1);
            PUSH2_val= bcm2835_gpio_lev(PUSH2);
            if (PUSH2_val == 0)
            {
            	if (signal_effect < 4)
                {
                	signal_effect++;
                }
                
				else if (signal_effect >= 4)
                {
                	signal_effect = 0;
                }
                printf("\n\nsignal_effect = %d\n\n", signal_effect);
             }
                
			if (PUSH1_val == 0)
            {
            	if (loop_effect < 4)
                {
                	loop_effect++;
                }
                
				else if (loop_effect >= 4)
                {
                	loop_effect = 0;
                }
                printf("\n\nloop_effect = %d\n\n", loop_effect);
            }
                
			switch(signal_effect)
    		{
    			case 0: //one blink
    			bcm2835_delay(400);
    			bcm2835_gpio_write(LED,0);
    			bcm2835_delay(200);
    			bcm2835_gpio_write(LED,1);
				break;
 
    			case 1: //two blinks
        		bcm2835_delay(400);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1); 
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
    			break;
 
    			case 2:
        		bcm2835_delay(400);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);     
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
    			break;
 
    			case 3:
    			bcm2835_delay(400);
    			bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
    			break;
         
				case 4:
    			bcm2835_delay(400);
    			bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(200);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		break;
            }

          	switch(loop_effect)
    		{
    			case 0: //one blink
    			bcm2835_delay(300);
    			bcm2835_gpio_write(LED,0);
    			bcm2835_delay(150);
    			bcm2835_gpio_write(LED,1);
    			break;
 
    			case 1: //two blinks
        		bcm2835_delay(300);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1); 
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
    			break;
 
    			case 2:
        		bcm2835_delay(300);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);     
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
    			break;
 
    			case 3:
    			bcm2835_delay(300);
    			bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
       			bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
    			break;
         
				case 4:
    			bcm2835_delay(300);
    			bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,0);
        		bcm2835_delay(150);
        		bcm2835_gpio_write(LED,1);
        		bcm2835_delay(200);
        		break;
    		}
             
		}
        
		if (signal_effect == 0)
        {
        	no_loop_output = dry_signal;
        }

        else if (signal_effect == 1)
        {
        	no_loop_output = phased_signal;
        }

        else if (signal_effect == 2)
        {
        	no_loop_output = flang_signal;
        }

        else if (signal_effect == 3)
        {
        	no_loop_output = pog_signal;
        }

        else if (signal_effect == 4)
        {
        	no_loop_output = rad_signal;
        }

        if (loop_effect == 0)
        {
        	with_loop_output = (loop_buffer[loop_read] + no_loop_output) >> 1;
        }

        else if (loop_effect == 1)
        {
        	with_loop_output = (phase_loop_buffer[loop_read] + no_loop_output) >> 1;
        }

        else if (loop_effect == 2)
        {
        	with_loop_output = flang_loop_buffer[loop_read] + no_loop_output;
        }

        else if (loop_effect == 3)
        {
        	with_loop_output = pog_loop_buffer[loop_read] + no_loop_output;
        }

        else if (loop_effect == 4)
        {
        	with_loop_output = (rad_loop_buffer[loop_read] + no_loop_output) >> 1;
        }
        
        if (loop_cue == 0 || loop_cue == 1)
        {
        	bcm2835_pwm_set_data(1,no_loop_output & 0x3F);
            bcm2835_pwm_set_data(0,no_loop_output >> 6);
        }

        else
        {
        	bcm2835_pwm_set_data(1, with_loop_output & 0x3F);
             bcm2835_pwm_set_data(0, with_loop_output >> 6);
        }
    //generate output PWM signal 6 bits
 
 }
        //close all and exit
        bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
