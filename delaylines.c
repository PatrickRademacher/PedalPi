/*
 * Copyright © 2019 Patrick Rademacher
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

#include "delaylines.h"
#define sample_delay 10
double * all_pass_filter(double * input_signal, double * output_signal, double gain, double ** IST, double ** OST, int * ISC, int * OSC, int index)
{
     int converter = 0;
     *output_signal = *input_signal * gain;
     *output_signal = *output_signal + IST[index][ISC[index]];
     *output_signal = *output_signal - (OST[index][ISC[index]] * gain);
     IST[index][ISC[index]++] = *input_signal;
     OST[index][OSC[index]++] = *output_signal;
     if (ISC[index] >= sample_delay)
     {
          
          ISC[index] -= sample_delay;
     }
     if (OSC[index] >= sample_delay)
     {
          OSC[index] -= sample_delay;
     }
     ISC[index] %= sample_delay;
     OSC[index] %= sample_delay;

     return output_signal;
}



double * flanger(double * input_signal, double * output_signal, double gain, double * flang, int * ISC, int flang_delay, int index)
{
     *output_signal = *input_signal;
     int converter = (int) (*output_signal + (gain * flang[ISC[index]]));
     converter = converter >> 1;
     *output_signal = (double) converter;
     flang[ISC[index]++] = *input_signal;
     if (ISC[index] >= flang_delay)
     {
          ISC[index] -= flang_delay;
     }
     ISC[index] %= flang_delay;
    return output_signal;
}
