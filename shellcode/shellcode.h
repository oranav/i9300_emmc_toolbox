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
#define MAGIC_SLEEP	(0x934ED462)
#define MAGIC_DISPLAY	(0x3D486FAB)
#define MAGIC_CLK1	(0x3300A7FB)
#define MAGIC_CLK2	(0x3F392D79)

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

#define SBOOT_START	0x43E00000
#define SBOOT_END	0x43F00000

/* Constants from actual firmware - sboot version XXELLA */
typedef void _display(int x, int y, int color, int, const char *fmt, ...);
#define display ((_display*)MAGIC_DISPLAY)

typedef void _sleep(int);
#define sleep ((_sleep*)MAGIC_SLEEP)

typedef void _usb_write(const void *, unsigned);
#define usb_write ((_usb_write*)MAGIC_USB_WRITE)

typedef void _usb_read(void *, unsigned);
#define usb_read ((_usb_read*)MAGIC_USB_READ)

typedef void _emmc_poweron();
#define emmc_poweron ((_emmc_poweron*)0x43E1BBC8)

typedef void _clk1(void *, int);
#define clk1 ((_clk1*)MAGIC_CLK1)

typedef void _clk2(void* ,int);
#define clk2 ((_clk2*)MAGIC_CLK2)

typedef int _mmc_startup(void *);
#define mmc_startup ((_mmc_startup*)0x43E1B75C)

#endif
