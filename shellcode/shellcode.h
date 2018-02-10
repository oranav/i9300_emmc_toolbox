/*
 * shellcode.h
 * Copyright (C) 2017 Oran Avraham (contact@oranav.me)
 *
 * Distributed under terms of the GPLv3 license.
 */

#ifndef _SHELLCODE_H_
#define _SHELLCODE_H_

/* These are auto-replaced by the exploit itself with the correct values */
#define MAGIC_USB_WRITE	(0x8776D35A)
#define MAGIC_USB_READ	(0xB9F9EAD9)

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
typedef int _before();
#define before ((_before*)0x43E0B8E0)

typedef void _display(int, int, int, int, const char *, ...);
#define display ((_display*)0x43E14B38)

typedef void _sleep(int);
#define sleep ((_sleep*)0x43E046E8)

typedef void _usb_write(const void *, unsigned);
#define usb_write ((_usb_write*)MAGIC_USB_WRITE)

typedef void _usb_read(void *, unsigned);
#define usb_read ((_usb_read*)MAGIC_USB_READ)

typedef void _s5c_mshc_init(void *);
#define s5c_mshc_init ((_s5c_mshc_init*)0x43E1E718)

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

#define mmc_host (void*)0x43EF5790
#define mmc_dev (void*)0x43EF4840

#endif
