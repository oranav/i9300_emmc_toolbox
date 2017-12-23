/*
 * demo.c
 * Copyright (C) 2017 Oran Avraham <contact@oranav.me>
 *
 * Distributed under terms of the GPLv3 license.
 */

/* Only for eMMC talk demo purposes -- please don't use! */

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

	memset(buf, 0x0a, BLOCK_SIZE);

	/* Tell host we're alive! */
	usb_write("oranav", 6);
	print(CLR_GOOD, "Shellcode started");

	/* Reboot eMMC into BootROM recovery mode */
	FAIL_IF_NEG(prepare_mmc(TRUE));
	print(CLR_GOOD, "Rebooted eMMC into bootrom mode");

	sleep(2000);

	/* Write descriptor to RAM */
	unsigned char fw_descr[68] = {
		0x56, 0x48, 0x58, 0x31, 0x26, 0x07, 0x11, 0x20, 0x00, 0x00, 0x5E, 0x00, 0x00, 0x00, 0x11, 0x20,
		0xCE, 0xFA, 0xBE, 0xBA, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x44, 0x00, 0x04, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x05, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
	};

	FAIL_IF_NEG(mmc_enter_write_dword());

	for (int i = 0; i < sizeof fw_descr; i += 4) {
		cmd.cmdidx = 35;
		cmd.resp_type = 21;
		cmd.cmdarg = 0x60194 + i;
		cmd.flags = 0;
		FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));

		cmd.cmdidx = 36;
		cmd.resp_type = 21;
		cmd.cmdarg = *((unsigned*)(&fw_descr[i]));
		cmd.flags = 0;
		FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));

		cmd.cmdidx = 38;
		cmd.resp_type = 21;
		cmd.cmdarg = 0;
		cmd.flags = 0;
		FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));
		usb_write("AC", 2);
		sleep(10);
	}

	FAIL_IF_NEG(mmc_exit_cmd62());

	/* Validate written descriptor */
	FAIL_IF_NEG(mmc_enter_read_ram());

	/* Address to read */
	cmd.cmdidx = 35;
	cmd.resp_type = 21;
	cmd.cmdarg = 0x60194;
	cmd.flags = 0;
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));

	/* Length to read */
	cmd.cmdidx = 36;
	cmd.resp_type = 21;
	cmd.cmdarg = BLOCK_SIZE;
	cmd.flags = 0;
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));

	/* Perform read RAM operation */
	cmd.cmdidx = 38;
	cmd.resp_type = 29;
	cmd.cmdarg = 0;
	cmd.flags = 0;
	memset(cmd.response, 0, 16);
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));

	/* Read the data from the eMMC */
	cmd.cmdidx = 17;
	cmd.resp_type = 21;
	cmd.cmdarg = 0;
	cmd.flags = 0;
	data.dest = buf;
	data.blocks = 1;
	data.blocksize = BLOCK_SIZE;
	data.flags = 1;
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, &data));

	FAIL_IF_NEG(mmc_exit_cmd62());

	for (int i = 0; i < sizeof fw_descr; ++i) {
		if (buf[i] != fw_descr[i]) {
			ret = 0x12345678;
			goto fail;
		}
	}

	FAIL_IF_NEG(mmc_enter_firmware_upgrade());
	print(CLR_INFO, "Firmware upgrade mode!");

	sleep(2000);

	/* Read new firmware from host */
	for (int i = 0; i < NUM_BLOCKS(FW_SIZE); ++i) {
		usb_write("GE", 2);
		usb_read(&buf[i*BLOCK_SIZE], BLOCK_SIZE);
	}

	/* Write the new firmware! */
	print(CLR_INFO, "Got firmware from host. Upgrading, hold tight...");
	cmd.cmdidx = 25;
	cmd.resp_type = 21;
	cmd.cmdarg = 0;  /* Both blocks */
	cmd.flags = 0;
	data.dest = buf;
	data.blocks = NUM_BLOCKS(FW_SIZE);
	data.blocksize = BLOCK_SIZE;
	data.flags = 2;
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, &data));
	usb_write("AC", 2);

	/* Wait for write operation to finish */
	sleep(20000);
	usb_write("AC", 2);
	print(CLR_GOOD, "Firmware has been written!");

	/* Update FW descriptor */
	print(CLR_INFO, "Updating descriptor...");
	cmd.cmdidx = 28;
	cmd.resp_type = 29;
	cmd.cmdarg = 0;  /* Both blocks */
	cmd.flags = 0;
	FAIL_IF_NEG(mmc_send_cmd(mmc_dev, &cmd, 0));
	usb_write("AC", 2);

	/* Wait for descriptor operation to finish */
	sleep(5000);
	print(CLR_GOOD, "Descriptor has been updated!");

	/* Reboot into firmware :-) */
	print(CLR_INFO, "Rebooting into firmware...");
	prepare_mmc(FALSE);
	sleep(5000);

	/* Resize boot partition */
	print(CLR_INFO, "Resizing boot partition...");

	/* Change boot partition knock code */
	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac62ec;
	cmd.flags = 0;
	mmc_send_cmd(mmc_dev, &cmd, 0);
	usb_write("AC", 2);

	sleep(100);

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xcbaea7;
	cmd.flags = 0;
	mmc_send_cmd(mmc_dev, &cmd, 0);
	usb_write("AC", 2);

	sleep(100);

	/* Set boot partition size to 4MB */
	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 4;
	cmd.flags = 0;
	mmc_send_cmd(mmc_dev, &cmd, 0);
	usb_write("AC", 2);

	/* Wait for repartition operation to finish */
	sleep(5000);

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
