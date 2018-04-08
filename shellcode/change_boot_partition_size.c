/*
 * change_boot_partition_size.c
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
	int ret;
	struct mmc_cmd cmd;
	struct mmc_data data;
	char *buf = (char *)0x45300000;

#define FAIL_IF_NEG(x) if ((ret = (x)) < 0) { goto fail; }

	/* Tell host we're alive! */
	usb_write("oranav", 6);
	screen_init();
	memset(buf, 0x0a, BLOCK_SIZE);
	print(CLR_GOOD, "Shellcode started");

	/* Initialize mmc_dev */
	FAIL_IF_NEG(mmc_dev_init());
	print(CLR_INFO, "Found MMC device address");

	/* Change boot partition knock code */
	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac62ec;
	cmd.flags = 0;
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));
	usb_write("AC", 2);

	sleep(100);

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xcbaea7;
	cmd.flags = 0;
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));
	usb_write("AC", 2);

	sleep(100);

	/* Set boot partition size to 4MB */
	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 4;
	cmd.flags = 0;
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));
	usb_write("AC", 2);

	/* Wait for repartition operation to finish */
	sleep(60000);

	/* Done! */
	print(CLR_GOOD, "Shellcode is done! Rebooting...");
	usb_write("DO", 2);

	sleep(5000);
	reboot();

fail:
	usb_write("ER", 2);
	usb_write(&ret, 4);
	sleep(1000);
	reboot();
}
