SRC:= ./src
BUILD:= ./build
PREFIX:= /opt/buildtools/riscv/bin/riscv64-unknown-elf-

QEMU:=qemu-system-riscv64
QEMUFLAGS:=\
-smp 4\
-bios none\
-machine virt\
-nographic\
-m 128M

SRC_FILES:= $(shell find $(SRC) -name *.c)
SRC_FILES+= $(shell find $(SRC) -name *.s)
SRC_FILES+= $(shell find $(SRC) -name *.S)

OBJECTS:= $(foreach it,$(SRC_FILES),$(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(it)))
OBJECTS:= $(foreach it,$(OBJECTS),$(patsubst $(SRC)/%.s,$(BUILD)/%.o,$(it)))
OBJECTS:= $(foreach it,$(OBJECTS),$(patsubst $(SRC)/%.S,$(BUILD)/%.o,$(it)))

INCLUDE_FILES:= $(shell find $(SRC) -name *.h)
INCLUDE_FILES:= $(foreach it,$(INCLUDE_FILES),-I$(dir $(it)))
INCLUDE_FILES:= $(sort $(INCLUDE_FILES))

CC:= $(PREFIX)gcc
CCFLAGS:=\
-nostdlib -nostdinc\
-O0 -ggdb\
-march=rv64g\
-mcmodel=medany\
-fno-stack-protector $(INCLUDE_FILES)	\
-static \
-ffreestanding	\
-Wno-pointer-to-int-cast


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






default:compile


compile:\
$(BUILD)/kernel.elf\
$(BUILD)/system.s\
$(BUILD)/system.map

qemu:$(BUILD)/kernel.elf\
		$(BUILD)/system.s\
		$(BUILD)/system.map\
		$(BUILD)/Disk.img
	$(QEMU) $(QEMUFLAGS) -hda $(BUILD)/Disk.img -kernel $<
qemu_test:$(BUILD)/kernel.elf\
		$(BUILD)/system.s\
		$(BUILD)/system.map
	$(QEMU) $(QEMUFLAGS) -kernel $<

qemuDbg:$(BUILD)/kernel.elf\
		$(BUILD)/system.s\
		$(BUILD)/system.map\
		$(BUILD)/Disk.img
	$(QEMU) $(QEMUFLAGS) -hda $(BUILD)/Disk.img -s -S -kernel $<

$(BUILD)/kernel.elf:\
	$(OBJECTS) kernel.ld
	$(shell mkdir -p $(dir $@))
	$(LD) $(LDFLAGS) -T kernel.ld $(OBJECTS) -o $@

$(BUILD)/%.o:$(SRC)/%.s
	$(shell mkdir -p $(dir $@))
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD)/%.o:$(SRC)/%.S
	$(shell mkdir -p $(dir $@))
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD)/%.o:$(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD)/Disk.img:
	$(shell mkdir -p $(dir $@))
	qemu-img create $@ 20M

$(BUILD)/system.s:$(BUILD)/kernel.elf
	$(shell mkdir -p $(dir $@))
	$(OBJDUMP) -d $< > $@

$(BUILD)/system.map:$(BUILD)/kernel.elf
	$(shell mkdir -p $(dir $@))
	$(NM) $< |sort > $@

debug:
	@echo $(OBJECTS)
.PHONY:clean debug
clean:
	rm -rf build
