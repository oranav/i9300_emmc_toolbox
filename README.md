# Samsung GT-I9300 eMMC toolbox

A collection of tools used to revive and experiment with Samsung GT-I9300 eMMC chips (VTU00M).

*This project is experimental in its nature and I take absolutely no responsiblity for anything that might happen to your device.*

**Caution: You will lose your EFS partition, but there's not much to do about it.**

## Instructions

### Reviving a dead Samsung Galaxy S3 (GT-I9300)

Prerequesities (for Debian stretch, should work on other apt-based distributions as well):
```shell
$ sudo apt install python3 python3-libusb1 libusb-1.0-0 gcc-arm-none-eabi binutils-arm-none-eabi
$ make -C shellcode
```

1. Obtain a VTU00M firmware dump (either 0xF1 or 0xF7) preferably from a donor 0xF7 device, or just dump your own 0xF1 device. Reference SHA256 sums are given [below](#reference-vtu00m-firmware-sha256-sums). This step is explained in detail [here](#obtaining-a-firmware-dump).
2. [Boot to download mode (sboot) on your device](#booting-into-download-mode).
3. Run the following command in order to low-level format the eMMC and install the new firmware:
	`exploit/sboot_exploit.py --shellcode shellcode/write_fw.bin -e FWDUMP`,
    where FWDUMP is the firmware dump obtained in step #1.
4. Your device no longer has sboot installed in its boot partition, so you must prepare a recovery SD card with sboot XXELLA if not already done before. Insert it into your device and boot into download mode as usual (you'll see nothing on screen, but device will show under `lsusb`).
5. Resize the boot partition:
	`exploit/sboot_exploit.py --shellcode shellcode/change_boot_partition_size.bin`
6. eMMC should be working right now. You can use a normal recovery SD card [as described here](https://forum.xda-developers.com/galaxy-s3/general/galaxy-s-iii-gt-i9300-hard-brick-fix-t1916796). This will install sboot on your eMMC, using *SDCARD mode*.
7. eMMC is functional! You can use Odin or [Heimdall](https://github.com/Benjamin-Dobell/Heimdall) to flash a new ROM to your device (you also need to repartition, i.e. [flash a new PIT](https://forum.xda-developers.com/galaxy-s3/help/solved-pit-bricked-dead-problem-gt-t2535367)). Flashing a new PIT may not work with Heimdall, so use Odin if this is the case. Afterwards, flash some recovery (e.g. TWRP) and format all the user partitions to ext4 (/cache, /data, /preload, /sdcard). Then flash some firmware and your phone is good to go (except for EFS which is missing now).

### Obtaining a firmware dump

#### Obtaining the firmware from a donor device (0xF7)

This is the preferred method, as your device will no longer suffer from the eMMC bug.

* First option: use a patched kernel as described [here](https://forum.xda-developers.com/showpost.php?p=37936242&postcount=72). Once you obtained mmcram.bin, strip it to contain only the firmware: `dd if=mmcram.bin of=0xf7.bin bs=4K skip=64 count=32`.
* Second option: dump using download mode. You're adviced to use sboot XXELLA since the shellcode is only guaranteed to work against it, but your mileage may vary. Enter download mode on your device, then run `exploit/sboot_exploit.py --shellcode shellcode/dump_fw.bin -o 0xf7.bin`.


#### Obtaining the firmware from your bricked device (0xF1)

This will unbrick your device, but your device will still suffer from the eMMC bug; the brick might happen again in the future. It's much better to obtain firmware 0xF7.

[Boot into download mode](#booting-into-download-mode), and run `exploit/sboot_exploit.py --shellcode shellcode/dump_fw_bootrom.bin -o 0xf1.bin`.

### Booting into download mode

If your device can boot into download mode normally, you have some version of sboot installed on your device. The shellcode should theoretically work on any sboot version, but it is only guaranteed to work on sboot XXELLA. If it doesn't work, it might need some adaptations for your sboot version; be advised that some reverse engineering skill is required. If you don't know which sboot version you have, [dump sboot from your eMMC](#dumping-sboot).

If your device cannot boot into download mode (i.e. it's entirely dead - you see nothing), you first need a charged battery. Then, proceed to [prepare a recovery SD card with sboot XXELLA](#preparing-a-recovery-sd-card-with-sboot-xxella). Once it's ready, just insert the SD card to your device, insert the battery, press power+volume down+home buttons for approx. 2-3 seconds, release all of them and press the volume up button once. Plug in the device over USB to your computer and it should be visible under `lsusb`. You might see some funky stuff on the phone's screen -- just ignore it.

#### Preparing a recovery SD card with sboot XXELLA

Prerequisites: obtain the firmware I9300XXELLA_I9300OXAELLA_BTU.zip. Extract the zip file, then extract the tar.md5 file inside (which is just a tar), and grab sboot.bin. This is the relevant sboot version. Copy it inside the sdcard folder.

Then run the following commands (replace DEV with the sdcard device on your computer, e.g. /dev/sdc. Be careful!):

```shell
$ cd sdcard
$ ./create_recovery_sdcard.py -o recovery_sdcard.bin -s sboot.bin
$ dd if=recovery_sdcard.bin of=DEV
$ sync
```

### Dumping sboot

In case you ever need to obtain sboot from your device (in case eMMC's boot partition is still intact), you can use `exploit/sboot_exploit.py --dump -o SBOOT`, and the exploit will dump sboot to a file named `SBOOT`. It actually dumps a 1MB chunk starting at address 0x43e00000, so the binary's loading address is 0x43e00000.

### Reference VTU00M firmware SHA256 sums

* 0xF1 firmware (buggy): 4cd6e829fa9370c8bc74523e181467382444892770370daa9163ecda6499d228
* 0xF7 firmware (bug fixed): 1c3ce3148704e21749c6b59ff88c7a133d385024f70038615806ae9e95798dbf

# Files

## Exploit

The `exploit/` folder contains the sboot exploit in order to run code in the sboot environment.

* sboot_exploit.py: sboot exploit using PIT file packets vulnerabilities; should work on all GT-I9300 sboot versions
* odin.py: Quick and dirty approach for implementing Odin's protocol in Python

## Shellcodes

The `shellcode/` folder contains a collection of sboot shellcodes for interaction with the eMMC.

* shellcode.h: Addresses for linking against sboot.
* common.c, common.h: Common functions for shellcode operation.
* write_fw.c: Writes a new firmware to the eMMC.
* dump_fw.c Dumps the firmware from a working (non-brick) eMMC.
* dump_fw_bootrom.c: Dumps the firmware from any eMMC (even bricked ones).
* change_boot_partition_size.c: Resizes the eMMC boot partition using its own mechanism (only for working eMMC devices).
* demo.c: Shellcode for eMMC talk demo purposes (please, don't use it!).
* helloworld.c: Prints a "Hello, world!" message on sboot's screen.
