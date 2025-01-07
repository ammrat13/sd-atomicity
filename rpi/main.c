//! \file main.c
//! \brief A program to test the atomicity of SD card writes.
//! \author Ammar Ratnani <ammrat13@gmail.com>
//!
//! Ideally, our implementation of FAT32 should be crash-consistent. To do that,
//! we need to be able to guarantee that sectors are written atomically. This
//! program tests that. First, it will reset a sector to a known value. It will
//! set a GPIO pin high, then start writing that sector. Once the pin goes high,
//! an external device will cut power to the device after some time. On the next
//! reboot, the program will check if we got the old data, the new data, or
//! garbage. For debugging, we'll also time how long it takes to write the old
//! data, as a proxy for how long it takes to write the new data.
//!
//! This program expects the SD card to have two partitions. The first partition
//! is for booting, while the second should be unformatted. The first sector of
//! the second partition will be used for testing. It also assumes 512B sectors.

#include "rpi.h"
#include "cycle-count.h"

#include "mbr.h"

//! \brief The GPIO pin to signal the start of the write.
#define SIGNAL_PIN (4u)

//! \brief Data for the old sector. Initialized in `do_init`.
uint8_t OLD_DATA[512];
//! \brief Data for the new sector. Initialized in `do_init`.
uint8_t NEW_DATA[512];

void do_init(void) {

    // Do initialization, then phone home.
    kmalloc_init();
    pi_sd_init();
    gpio_set_output(SIGNAL_PIN);
    printk("Starting SD card atomicity test...\n");

    // Initialize the old and new data. The new data is the bitwise negation of
    // the old data. The old data is bytes increasing from 0 to 255.
    for (size_t i = 0u; i < 512u; i++) {
        uint8_t v = (uint8_t)i;
        OLD_DATA[i] = v;
        NEW_DATA[i] = ~v;
    }
}

mbr_partition_ent_t get_test_partition(void) {

    // Read the MBR.
    mbr_t *mbr = mbr_read();
    mbr_check(mbr);

    // Get the test partition. Make sure it has space.
    mbr_partition_ent_t part = mbr_get_partition(mbr, 1);
    mbr_partition_print("Test partition", &part);
    assert(part.lba_start != 0);
    assert(part.nsec >= 1);

    return part;
}

void check_sector(uint32_t sec) {

    // Read the sector.
    uint8_t *data = pi_sec_read(sec, 1);
    assert(data);

    // Check if the data is the old data, the new data, or garbage.
    int old_cmp = memcmp(data, OLD_DATA, 512);
    int new_cmp = memcmp(data, NEW_DATA, 512);
    if (old_cmp == 0) {
        printk("SDTEST-RESULT: OLD\n");
    } else if (new_cmp == 0) {
        printk("SDTEST-RESULT: NEW\n");
    } else {
        printk("SDTEST-RESULT: GARBAGE\n");
    }
}

void write_old_sector(uint32_t sec) {
    // Write the old data to the sector. For debugging, we'll time how long this
    // takes. This is the baseline for the atomic write.
    uint32_t start = cycle_cnt_read();
    pi_sd_write(OLD_DATA, sec, 1);
    uint32_t end = cycle_cnt_read();
    printk("SDTEST-TIME: %u\n", end - start);
}

__attribute__((noreturn))
void notmain(void) {

    do_init();

    mbr_partition_ent_t part = get_test_partition();
    uint32_t sec = part.lba_start;

    check_sector(sec);
    write_old_sector(sec);

    // Kill the monitor by making it think we're done.
    putk("DONE!!!\n");

    // Signal the write of the new data.
    gpio_set_on(SIGNAL_PIN);
    pi_sd_write(NEW_DATA, sec, 1);

    // Wait for the power to be cut if it hasn't already.
    while (1)
        ;
}
