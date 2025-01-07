STAFF_OBJS += staff-mbr.o

PROGS := main.c

CFLAGS_EXTRA = -Iexternal-code

COMMON_SRC += pi-sd.c mbr-helpers.c external-code/unicode-utf8.c external-code/emmc.c external-code/mbox.c

TTYUSB =
BOOTLOADER = $(CS140E_2024_PATH)/bin/pi-install.linux
RUN = 1

EXCLUDE ?= grep -v simple_boot
GREP_STR := 'HASH:\|ERROR:\|PANIC:\|PASS:\|TEST:'
include $(CS140E_2024_PATH)/libpi/mk/Makefile.template-v3

clean::
	rm -f tests/*.list tests/*.o tests/*.elf tests/*~ tests/*.bin
	make -C external-code clean
