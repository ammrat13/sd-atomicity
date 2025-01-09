# SD Card Atomicity Test

According to Google and ChatGPT, SD Cards don't guarantee atomic sector writes,
but there is conflicting information. This repository has code to practically
test whether the RPi as a whole provides this atomicity.

The RPi populates a sector with known "old" data. It then signals an external
processor - an Arduino in this case - before overwriting the old data with
(known) new data. Once the Arduino receives the signal, it will wait some time
before cutting power to the RPi, possibly interrupting the write. Once the RPi
reboots, it will check if the sector has the old data, the new data, or
something else entirely.

## Running

### RPi

The code for the Raspberry Pi Zero uses the CS 140E build system, specifically
[dddrrreee/cs140e-24win](https://github.com/dddrrreee/cs140e-24win). The
`arm-none-eabi` cross-compiler needs to be installed, along with cross-`newlib`.
Further, the `libpi/Makefile` must be modified to include
`./staff-objs/kmalloc.o` in the `STAFF_OBJS` variable, as documented in
`labs/10-debug-hw/README.md`.

To build, run `make all` in the `rpi/` subdirectory of this project, with the
`CS140E_2024_PATH` environment variable set to the path of the checked-out and
modified [dddrrreee/cs140e-24win](https://github.com/dddrrreee/cs140e-24win)
repository. Use the bootloader provided by the build system to program the
device with `pi-install`.

The RPi drives the `SIGNAL` net using the `SIGNAL_PIN` defined in `rpi/main.c`.
By default, this is GPIO 4 / Header Pin 7.

### Arduino

Use the Arduino IDE. This code was tested on an Arduino MEGA. The Arduino reads
the `SIGNAL` net on the `SIGNAL_PIN`, and it cuts the power with the `SET` net
via the `SET_PIN`. By default, the pins are 2 and 3 respectively.

### Circuit

The `SIGNAL_PIN`s of the RPi and the Arduino are connected together to form the
`SIGNAL` net, which needs an external pull-down resistor. The `SET` net is
connected to the gate of a P-Channel MOSFET. The source of the MOSFET is at +5V,
and the drain powers the RPi via Header Pin 2. The gate and drain of the MOSFET
also have pull-down resistors.

This code was tested with all the pull-down resistors measuring 10K.

## Results

Binary-search was used to find the delay that results in a 50-50 chance of
reading back the old or the new data. At this delay, 300-1 writes were run. None
of them resulted in "garbage" being read back - it was always either the old or
the new block.

However, it appears that the supply voltage on the RPi after the Arduino cuts
power doesn't go all the way down to 0V. Instead, it remains around 2V. While
unlikely, it is possible that this is enough to power the SD Card - a quick
Google search shows that some SD Cards can function at 1.8V. Further tests are
needed.

It is unknown why the voltage isn't going all the way down to 0V. Maybe I mixed
up the source and drain?
