/*
 * common.h
 * Copyright (C) 2017 Oran Avraham <contact@oranav.me>
 *
 * Distributed under terms of the GPLv3 license.
 */
#ifndef _COMMON_H_
#define _COMMON_H_

typedef unsigned size_t;
typedef int ssize_t;

#define FALSE 0
#define TRUE 1

#define CLR_BAD  0xff0000
#define CLR_GOOD 0x00ff00
#define CLR_INFO 0xffffff

void print(unsigned color, const char *s);


#define BLOCK_SIZE 512
#define NUM_BLOCKS(x) (((x)+(BLOCK_SIZE)-1)/(BLOCK_SIZE))

#define FW_SIZE 0x20000

#define BOOTROM_ADDR 0
#define FW_ADDR 0x40000
#define LOAD_FW_ADDR (0x8020|1)
#define MMC_HANDLERS_ADDR (0x60030)

void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
size_t strlen(const char *s);
int mmc_send_cmd();
int prepare_mmc(int bootrom);
int mmc_enter_read_ram();
int mmc_enter_write_ram();
int mmc_enter_read_dword();
int mmc_enter_write_dword();
int mmc_enter_jump();
int mmc_exit_cmd62();
int mmc_activate_cmd60();
int mmc_enter_firmware_upgrade();
int mmc_firmware_activate(unsigned type);
int mmc_start_timer();
void emmc_poweroff();
void reboot();

#endif
