/*
 * Copyright © 2019 Patrick Rademacher
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <bcm2835.h>

double * all_pass_filter(double * input_signal, double * output_signal, double gain, double ** IST, double ** OST, int * ISC, int * OSC, int index);

double * flanger(double * input_signal, double * output_signal, double gain, double * flang_table, int * ISC_2, int flang_delay, int index);
