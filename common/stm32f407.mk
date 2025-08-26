OPENCM3_DIR =../libopencm3

DEVICE=stm32f407vg
LIBNAME= opencm3_stm32f4
ARCH_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

DEFINES=-DSTM32F4 -DSTM32F407VG

CFLAGS		+= -O3\
		   -Wall -Wextra -Wimplicit-function-declaration \
		   -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
		   -Wundef -Wshadow \
		   -I$(OPENCM3_DIR)/include \
		   -I../common \
		   -fno-common $(ARCH_FLAGS) -MD $(DEFINES)

LDSCRIPT = ../common/stm32f405x6.ld

LDLIBS += -l$(LIBNAME)
LIBDEPS += $(OPENCM3_DIR)/lib/lib$(LIBNAME).a

LDFLAGS += -L$(OPENCM3_DIR)/lib
LDFLAGS += \
	--specs=nosys.specs \
	-nostartfiles \
	-Wl,--no-warn-rwx-segments \
	-ffreestanding \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LINKDEPS += obj/../common/hal-stm32f4.c.o $(PROJECT_OBJS)

all: bin/stm32f407.bin

$(OPENCM3_DIR)/lib/lib$(LIBNAME).a:
	$(MAKE) -C $(OPENCM3_DIR)

obj/../common/hal-stm32f4.c.o: $(OPENCM3_DIR)/lib/lib$(LIBNAME).a

elf/stm32f407.elf: $(PROJECT_OBJS) obj/../common/hal-stm32f4.c.o $(OPENCM3_DIR)/lib/lib$(LIBNAME).a obj/_ELFNAME_stm32f407.elf.o

# Flash target for STM32F407 board using st-flash (requires st-link tools)
flash-stm32: bin/stm32f407.bin
	@echo "Flashing STM32F407 board..."
	st-flash write bin/stm32f407.bin 0x8000000
