/*
 * dump_fw_bootrom.c
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

	memset(buf, 0x0a, BLOCK_SIZE);

	/* Tell host we're alive! */
	usb_write("oranav", 6);
	print(CLR_GOOD, "Shellcode started");

	/* Initialize mmc_dev */
	FAIL_IF_NEG(mmc_dev_init());
	print(CLR_INFO, "Found MMC device address");

	/* Reboot eMMC into BootROM recovery mode */
	FAIL_IF_NEG(prepare_mmc(TRUE));
	print(CLR_INFO, "Rebooted eMMC into bootrom mode");
	sleep(2000);

	/* Load firmware... */
	print(CLR_INFO, "Reading eMMC firmware to eMMC RAM");
	FAIL_IF_NEG(mmc_firmware_activate(0));
	sleep(1000);

	/* ... maybe it's on the other block ... */
	FAIL_IF_NEG(mmc_firmware_activate(1));
	sleep(1000);

	/* Send host our firmware */
	print(CLR_INFO, "Enter eMMC RAM reading backdoor");
	FAIL_IF_NEG(mmc_enter_read_ram());

	print(CLR_INFO, "Reading eMMC firmware...");

	for (int i = 0; i < NUM_BLOCKS(FW_SIZE); ++i) {
		/* Address to read */
		cmd.cmdidx = 35;
		cmd.resp_type = 21;
		cmd.cmdarg = FW_ADDR + i*BLOCK_SIZE;
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

		/* Send data to host */
		usb_write("OU", 2);
		usb_write(buf, BLOCK_SIZE);
	}

	/* Exit backdoor */
	FAIL_IF_NEG(mmc_exit_cmd62());

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
