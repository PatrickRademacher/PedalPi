# Multi-Effects Looper
Copyright © 2019 Patrick Rademacher

This is a multi-effects looper for the
[PedalPi](https://www.electrosmash.com/pedal-pi).

## Build

This program is set up to be cross-compiled for the PedalPi.

Dependencies:

* <https://github.com/raspberrypi/tools>

  Clone this repository and place the 
  `arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin`
  subdirectory  somehwere in your path.

* <https://www.airspayce.com/mikem/bcm2835/>

  Download the distribution tarball
  <http://www.airspayce.com/mikem/bcm2835/bcm2835-1.58.tar.gz>.
  Unpack it and say

          ./configure --host arm-linux-gnueabihf
          make
          sudo make install

  (Yes, this should ideally be `--target`, but not so much
  for this package.)

When the dependencies are installed, build `melo` by saying
`make`.

## License

This code is based on code Copyright © 2019 Electrosmash,
used under a Creative Commons license.

This code is made available under the "MIT License". Please
see the file `LICENSE` in this distribution for license terms.
