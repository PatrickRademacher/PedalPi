# Copyright © 2019 Patrick Rademacher
# [This program is licensed under the "MIT License"]
# Please see the file LICENSE in the source
# distribution of this software for license terms.

PIDIR = /usr/local/src/raspberrypi-tools
ARMDIR = $(PIDIR)/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf
PILIB = $(ARMDIR)/lib

CC = $(ARMDIR)/bin/arm-linux-gnueabihf-gcc
CFLAGS = -O4 -std=c11 -I/usr/local/include
LDFLAGS = -L$(PILIB) -L/usr/local/lib
LIBS = -lbcm2835 -lm

OBJS = melo.o delaylines.o

melo: $(OBJS)
	$(CC) $(CFLAGS) -o melo $(OBJS) $(LDFLAGS) $(LIBS)

$(OBJS): delaylines.h

clean:
	-rm -f $(OBJS) melo
