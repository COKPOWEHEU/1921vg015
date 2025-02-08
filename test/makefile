progname := firmware
progdir := res
files := main plic startup strlib system_k1921vg015
#_k1921vg015

HSE_CLOCK := 16000000

srcdir := src
builddir := build
libdir := ../lib

CROSS_COMPILE=riscv64-unknown-elf-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
SIZE=$(CROSS_COMPILE)size

ARCH := rv32im

ifneq (, $(shell which riscv64-unknown-elf-gcc 2>/dev/null))
  CROSS_COMPILE=riscv64-unknown-elf-
  GCCVER=`$(CC) --version | sed -n "1s/.* \([0-9]*[.][0-9]*[.][0-9]*\).*/\1/p"`
  #GCCPATH = -L/usr/lib/gcc/riscv64-unknown-elf/$(GCCVER)/$(ARCH)/ilp32/
  GCCPATH = -L/usr/lib/picolibc/riscv64-unknown-elf/lib/$(ARCH)/ilp32/
else ifneq (, $(shell which riscv64-linux-gnu-gcc 2>/dev/null))
  CROSS_COMPILE=riscv64-linux-gnu-
else
  $(error Unknown RISC-V compiler)
endif


COMMON = -march=$(ARCH)_zicsr
COMMON += -MD -MP -MT $(builddir)/$(*F).o -MF $(builddir)/dep/$(@F).mk
COMMON += -mabi=ilp32 -mcmodel=medany -static -fno-plt -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables -Wno-main -Wstack-usage=2048 -ffreestanding -Wno-unused -fno-builtin-printf

CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -g -DF_HSE=$(HSE_CLOCK)
CFLAGS += -I$(libdir)/Include
CFLAGS += -I/usr/lib/picolibc/riscv64-unknown-elf/include
CFLAGS += -DHSECLK_VAL=16000000 -DSYSCLK_PLL
ASMFLAGS = $(COMMON) -I$(libdir)/Include -I/usr/lib/picolibc/riscv64-unknown-elf/include

LDFLAGS = $(COMMON) -nostdlib -L ../lib/ldscripts -T k1921vg015_flash.ld
#-flto=auto
LDFLAGS += $(GCCPATH) -lc
#LDFLAGS += /usr/lib/picolibc/riscv64-unknown-elf/lib/rv32im/ilp32/libc.a

frmname = $(progdir)/$(progname)
#files += startup sys_init
objs = $(addprefix $(builddir)/,$(addsuffix .o,$(files)))

all: $(frmname).bin $(frmname).hex $(frmname).lss size
	
$(frmname).bin: $(frmname).elf
	$(OBJCOPY) -O binary $^ $@
$(frmname).hex: $(frmname).elf
	$(OBJCOPY) -Oihex $(frmname).elf $(frmname).hex
$(frmname).lss: $(frmname).elf
	$(OBJDUMP) -D -S $(frmname).elf > $(frmname).lss

$(frmname).elf: $(objs) $(LSCRIPT)
	mkdir -p $(progdir)
	@ echo "..linking"
	$(LD) $(LDFLAGS) $(objs) -o $@ 

$(builddir)/%.o: $(srcdir)/%.c
	mkdir -p $(builddir)
	$(CC) -c $(CFLAGS) $< -o $@
$(builddir)/%.o: $(srcdir)/%.S
	mkdir -p $(builddir)
	$(CC) -c $(ASMFLAGS) $< -o $@
	
$(builddir)/startup_k1921vg015.o: ../lib/Source/startup_k1921vg015.S
	mkdir -p $(builddir)
	$(CC) -c $(ASMFLAGS) $< -o $@
$(builddir)/%.o: $(libdir)/Source/%.c
	mkdir -p $(builddir)
	$(CC) -c $(CFLAGS) $< -o $@
#$(builddir)/sys_init.o: ../lib/Source/sys_init.c
#	mkdir -p $(builddir)
#	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(progdir)
	rm -rf $(builddir)
size: $(frmname).elf
	$(SIZE) $(frmname).elf
	
virt = virt2
prog: $(frmname).bin
	scp $(frmname).bin $(virt):/home/user/prog/vg015/rem/firmware.bin
	ssh $(virt) "cd /home/user/prog/vg015/rem ; "\
	"../xpack-openocd-k1921vk-0.12.0-k1921vk/bin/openocd -f ft4232.cfg -f k1921vg015.cfg -c 'init' -c 'reset halt' -c 'program firmware.bin 0x80000000 verify' -c 'reset run' -c 'exit'"
reset:
	ssh $(virt) "cd /home/user/prog/vg015/rem ; "\
	"../xpack-openocd-k1921vk-0.12.0-k1921vk/bin/openocd -f ft4232.cfg -f k1921vg015.cfg -c 'init' -c 'reset halt' -c 'reset run' -c 'exit'"
unbrick:
	echo "Connect SERVEN to VCC, reset controller and press Enter"
	read
	ssh $(virt) "cd /home/user/prog/vg015/rem ; "\
	"../xpack-openocd-k1921vk-0.12.0-k1921vk/bin/openocd -f ft4232.cfg -f k1921vg015.cfg -c 'init' -c 'reset halt' -c 'mww 0x3000F104 0x00000100' -c 'mdw 0x3000F104' -c 'reset run' -c 'exit'"

#/home/user/prog/vg015/xpack-openocd-k1921vk-0.12.0-k1921vk/bin/openocd -f ../openocd/scripts/interface/ftdi/ft4232.cfg -f ../../niiet_riscv_sdk/tools/openocd/scripts/target/k1921vg015.cfg -c "init" -c "reset halt" -c "program firmware.bin 0x80000000 verify" -c "reset run" -c "exit"
	

-include $(shell mkdir -p $(builddir)/dep) $(wildcard $(builddir)/dep/*)

.PHONY: all clean
