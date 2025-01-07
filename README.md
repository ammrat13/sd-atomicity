# SD Card Atomicity Test

Test for atomic sector writes for the SD Card on the RPi Zero.

## Building RPi Code

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
