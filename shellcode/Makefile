CROSS_COMPILE=arm-none-eabi-

CC=${CROSS_COMPILE}gcc
LD=${CROSS_COMPILE}gcc
OBJCOPY=${CROSS_COMPILE}objcopy

CFLAGS += -fno-stack-protector -fno-common -fomit-frame-pointer -fPIE -static -O0
LDFLAGS += -nostdlib

all: helloworld.bin demo.bin write_fw.bin change_boot_partition_size.bin dump_fw.bin dump_fw_bootrom.bin

%.bin: %.elf
	${OBJCOPY} -j .text -j .rodata -j .data -O binary $< $@

%.elf: %.o common.o
	${LD} ${LDFLAGS} -o $@ $^

%.o: %.c
	${CC} -c ${CFLAGS} -o $@ $<

clean:
	rm -f *.bin
	rm -f *.elf
	rm -f *.o
