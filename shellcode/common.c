/*
 * common.c
 *
 * Library functions for sboot shellcodes.
 *
 * Copyright (C) 2017 Oran Avraham <contact@oranav.me>
 *
 * Distributed under terms of the GPLv3 license.
 */
#include "shellcode.h"
#include "common.h"

void *memcpy(void *dst, const void *src, size_t n)
{
	char *dst_chr = (char *)dst;
	const char *src_chr = (const char *)src;

	while (n--)
		*dst_chr++ = *src_chr++;

	return dst;
}

size_t strlen(const char *s)
{
	unsigned n;
	for (n = 0; *s++; ++n);
	return n;
}

int mmc_send_cmd(void *host, struct mmc_cmd *cmd, struct mmc_data *data)
{
	int (*func_addr)(void *, struct mmc_cmd *, struct mmc_data *);
	func_addr = (int (*)(void *, struct mmc_cmd *, struct mmc_data *))
		(*(long int*)host);
	return func_addr(host, cmd, data);
}

int prepare_mmc(int bootrom)
{
	int ret;
	struct mmc_cmd cmd;

	emmc_poweroff();
	memset(mmc_dev, 0, 272);
	memset(mmc_host, 0, 56);
	sleep(2000);

	emmc_poweron();
	s5c_mshc_init(mmc_dev);
	clk1(mmc_dev, 1);
	clk2(mmc_dev, 0);
	sleep(10);
	if (bootrom) {
		cmd.cmdidx = 1;
		cmd.resp_type = 1;
		cmd.cmdarg = 0x69FF87A9;
		cmd.flags = 0;
		mmc_send_cmd(mmc_dev, &cmd, 0);
		sleep(10);
	} else {
		sleep(1000);
	}

	if ((ret = mmc_send_op_cond(mmc_dev)) < 0) return ret;
	if ((ret = mmc_startup(mmc_dev)) < 0) return ret;

	return 0;
}

int mmc_enter_read_ram()
{
	int ret;
	struct mmc_cmd cmd;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac62ec;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0x10210002;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_enter_write_ram()
{
	int ret;
	struct mmc_cmd cmd;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac62ec;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0x10210001;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_enter_read_dword()
{
	int ret;
	struct mmc_cmd cmd;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac62ec;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0x10210003;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_enter_write_dword()
{
	int ret;
	struct mmc_cmd cmd;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac62ec;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0x10210000;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_enter_jump()
{
	int ret;
	struct mmc_cmd cmd;

	cmd.cmdidx = 60;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac60fc;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	cmd.cmdidx = 60;
	cmd.resp_type = 29;
	cmd.cmdarg = 0x10210010;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_exit_cmd62()
{
	int ret;
	struct mmc_cmd cmd;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac62ec;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	cmd.cmdidx = 62;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xdeccee;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_enter_firmware_upgrade()
{
	int ret;
	struct mmc_cmd cmd;

	if ((ret = mmc_activate_cmd60()) < 0) return ret;

	cmd.cmdidx = 60;
	cmd.resp_type = 29;
	/*cmd.cmdarg = 0x1bfc3360;*/
	cmd.cmdarg = 0xcbad1160;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_activate_cmd60()
{
	int ret;
	struct mmc_cmd cmd;

	cmd.cmdidx = 60;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xefac60fc;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_firmware_activate(unsigned type)
{
	int ret;
	struct mmc_cmd cmd;

	if ((ret = mmc_activate_cmd60()) < 0) return ret;

	cmd.cmdidx = 60;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xabcd1280 + type;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return 0;
}

int mmc_start_timer()
{
	int ret;
	struct mmc_cmd cmd;

	if ((ret = mmc_activate_cmd60()) < 0) return ret;

	cmd.cmdidx = 60;
	cmd.resp_type = 29;
	cmd.cmdarg = 0xabcd1240;
	cmd.flags = 0;
	if ((ret = mmc_send_cmd(mmc_dev, &cmd, 0)) < 0) return ret;

	return cmd.response[0];
}

void print(unsigned color, const char *s)
{
	size_t sz = strlen(s);
	display(10, before(), color, 0, s);
	usb_write("TX", 2);
	usb_write(&color, sizeof color);
	usb_write(&sz, sizeof sz);
	usb_write(s, sz);
}
