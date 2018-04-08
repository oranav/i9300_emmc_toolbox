/*
 * helloworld.c
 * Copyright (C) 2017 Oran Avraham <contact@oranav.me>
 *
 * Distributed under terms of the GPLv3 license.
 */

#include "shellcode.h"
#include "common.h"

__asm__(
".global _start\n"
"_start:\n"
);


int main() {
	/* Tell host we're alive! */
	usb_write("oranav", 6);
	screen_init();
	print(CLR_GOOD, "Hello, world!");
	print(CLR_INFO, "sboot exploit by @oranav");
	usb_write("DO", 2);

	sleep(5000);
	reboot();
}
