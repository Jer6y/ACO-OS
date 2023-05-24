SRC:= ./src
BUILD:= ./build
PREFIX:= riscv64-unknown-elf-

QEMU:=qemu-system-riscv64
QEMUFLAGS:=\
-smp 1\
-bios none\
-machine virt\
-nographic\
-m 128M


CC:= $(PREFIX)gcc
CCFLAGS:=\
-nostdlib -nostdinc\
-ffreestanding\
-O0 -ggdb\
-march=rv64g\
-mcmodel=medany\
-I./src/include \
-fno-stack-protector

LD:=$(PREFIX)ld
LDFLAGS:= -z max-page-size=4096

OBJDUMP:=$(PREFIX)objdump
OBJCOPY:=$(PREFIX)objcopy

NM:=$(PREFIX)nm
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CCFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CCFLAGS += -fno-pie -nopie
endif


OBJECTS:= \
$(BUILD)/boot/boot.o\
$(BUILD)/kernel/start.o\
$(BUILD)/kernel/riscv.o\
$(BUILD)/kernel/main.o\
$(BUILD)/kernel/kernelvec.o\
$(BUILD)/kernel/uart.o\
$(BUILD)/kernel/console.o\
$(BUILD)/kernel/spinlock.o\
$(BUILD)/kernel/proc.o\
$(BUILD)/kernel/kalloc.o\
$(BUILD)/kernel/string.o\
$(BUILD)/kernel/plic.o\
$(BUILD)/kernel/trap.o\
$(BUILD)/kernel/vm.o


default:compile


compile:\
$(BUILD)/kernel.bin\
$(BUILD)/system.s\
$(BUILD)/system.map

qemu:$(BUILD)/kernel.bin\
		$(BUILD)/system.s\
		$(BUILD)/system.map\
		$(BUILD)/Disk.img
	$(QEMU) $(QEMUFLAGS) -hda $(BUILD)/Disk.img -kernel $<

qemuDbg:$(BUILD)/kernel.bin\
		$(BUILD)/system.s\
		$(BUILD)/system.map\
		$(BUILD)/Disk.img
	$(QEMU) $(QEMUFLAGS) -hda $(BUILD)/Disk.img -s -S -kernel $<

$(BUILD)/kernel.bin:\
	$(OBJECTS) kernel.ld
	$(shell mkdir -p $(dir $@))
	$(LD) $(LDFLAGS) -T kernel.ld $(OBJECTS) -o $@

$(BUILD)/boot/%.o:$(SRC)/boot/%.s
	$(shell mkdir -p $(dir $@))
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD)/kernel/%.o:$(SRC)/kernel/%.s
	$(shell mkdir -p $(dir $@))
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD)/kernel/%.o:$(SRC)/kernel/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) $(CCFLAGS) -c $< -o $@


$(BUILD)/Disk.img:
	$(shell mkdir -p $(dir $@))
	qemu-img create $@ 20M

$(BUILD)/system.s:$(BUILD)/kernel.bin
	$(shell mkdir -p $(dir $@))
	$(OBJDUMP) -d $< > $@

$(BUILD)/system.map:$(BUILD)/kernel.bin
	$(shell mkdir -p $(dir $@))
	$(NM) $< |sort > $@

.PHONY:clean
clean:
	rm -rf build
