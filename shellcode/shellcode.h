/*
 * shellcode.h
 * Copyright (C) 2017 Oran Avraham (contact@oranav.me)
 *
 * Distributed under terms of the GPLv3 license.
 */

#ifndef _SHELLCODE_H_
#define _SHELLCODE_H_

/* Structs and typedefs */
struct mmc_cmd {
	unsigned short cmdidx;
	unsigned int resp_type;
	unsigned int cmdarg;
	unsigned int response[4];
	unsigned int flags;
};

struct mmc_data {
	union {
		char *dest;
		const char *src;
	};
	unsigned flags;
	unsigned blocks;
	unsigned blocksize;
};

#define MMC_DEV_SIZE 200
#define MMC_HOST_SIZE 56

/* Constants from actual firmware - sboot version XXELLA */
typedef int _memset(void *, unsigned char, unsigned);
#define memset ((_memset*)0x43E02450)

typedef int _before();
#define before ((_before*)0x43E0B8E0)

typedef void _display(int, int, int, int, const char *, ...);
#define display ((_display*)0x43E14B38)

typedef int _reboot();
#define reboot ((_reboot*)0x43E060B0)

typedef void _sleep(int);
#define sleep ((_sleep*)0x43E046E8)

typedef void _usb_write(const void *, unsigned);
#define usb_write ((_usb_write*)0x43E24C2C)

typedef void _usb_read(void *, unsigned);
#define usb_read ((_usb_read*)0x43E24C64)

typedef void _s5c_mshc_init(void *);
#define s5c_mshc_init ((_s5c_mshc_init*)0x43E1E718)

typedef void _emmc_poweroff();
#define emmc_poweroff ((_emmc_poweroff*)0x43E1BD70)

typedef void _emmc_poweron();
#define emmc_poweron ((_emmc_poweron*)0x43E1BBC8)

typedef void _clk1(void *, int);
#define clk1 ((_clk1*)0x43E1ABEC)

typedef void _clk2(void* ,int);
#define clk2 ((_clk2*)0x43E1AC6C)

typedef int _mmc_startup(void *);
#define mmc_startup ((_mmc_startup*)0x43E1B75C)

typedef int _mmc_send_op_cond(void*);
#define mmc_send_op_cond ((_mmc_send_op_cond*)0x43E18AA0)

typedef void _call_init_functions();
#define call_init_functions ((_call_init_functions*)0x43E016D8)

typedef int _mmc_initialize();
#define mmc_initialize ((_mmc_initialize*)0x43E1BBE8)

typedef int _boot();
#define boot ((_boot*)0x43E016A4)

typedef void _restart_handler();
#define restart_handler ((_restart_handler*)0x43E00020)

typedef void _setenv(unsigned cmd, unsigned val_int, const char *val_str,
		int commit);
#define setenv ((_setenv*)0x43E0AA9C)

typedef void _saveenv();
#define saveenv ((_saveenv*)0x43E0A8FC)

#define mmc_host (void*)0x43EF5790
#define mmc_dev (void*)0x43EF4840

#endif
