# Makefile for Home Environment Monitor - CC3220SF + FreeRTOS
#
# Requires:
#   - ARM GNU Toolchain (arm-none-eabi-gcc)
#   - TI SimpleLink CC32xx SDK v7.10
#   - FreeRTOS Kernel source

# -------- Toolchain & SDK Paths (override on command line or env) --------
GCC_ARMCOMPILER  ?= /Users/ry/tools/arm-gnu-toolchain-15.2.rel1-darwin-arm64-arm-none-eabi
SDK_INSTALL_DIR  ?= /Applications/ti/simplelink_cc32xx_sdk_7_10_00_13
FREERTOS_KERNEL  ?= /Users/ry/repos/FreeRTOS-Kernel

CC  = $(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc
LD  = $(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc
HEX = $(GCC_ARMCOMPILER)/bin/arm-none-eabi-objcopy
SZ  = $(GCC_ARMCOMPILER)/bin/arm-none-eabi-size

# -------- Directories --------
SRC_DIR  = firmware
BUILD    = build
SDK_SRC  = $(SDK_INSTALL_DIR)/source

# -------- Target --------
TARGET   = env_monitor
LINKER   = $(SDK_SRC)/ti/boards/cc32xxsf/cc32xxsf_freertos.lds

# -------- Compiler Flags --------
CFLAGS  = -DDeviceFamily_CC3220
CFLAGS += -I$(SRC_DIR)
CFLAGS += -I$(SDK_SRC)
CFLAGS += -I$(SDK_SRC)/ti/posix/gcc
CFLAGS += -I$(SDK_INSTALL_DIR)/kernel/freertos
CFLAGS += -I$(SDK_INSTALL_DIR)/kernel/freertos/builds/cc32xx/release/pregenerated_configuration
CFLAGS += -I$(FREERTOS_KERNEL)/include
CFLAGS += -I$(FREERTOS_KERNEL)/portable/GCC/ARM_CM3
CFLAGS += -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=soft
CFLAGS += -std=c99 -D_REENT_SMALL
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -O2 -g

# -------- Linker Flags --------
LFLAGS  = -Wl,-T,$(LINKER) -Wl,-Map,$(BUILD)/$(TARGET).map
LFLAGS += -L$(SDK_SRC)
LFLAGS += -l:ti/drivers/lib/gcc/m4/drivers_cc32xx.a
LFLAGS += -l:ti/drivers/net/wifi/gcc/rtos/simplelink.a
LFLAGS += -l:ti/net/mqtt/lib/gcc/m4/mqtt_release.a
LFLAGS += -l:ti/net/lib/gcc/m4/slnetsock_release.a
LFLAGS += -l:ti/devices/cc32xx/driverlib/gcc/Release/driverlib.a
LFLAGS += -l:ti/display/lib/gcc/m4/display_cc32xx.a
LFLAGS += -l:ti/log/lib/gcc/m4/log_cc32xx.a
LFLAGS += -march=armv7e-m -mthumb -nostartfiles -static
LFLAGS += -Wl,--gc-sections
LFLAGS += -L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v7e-m/nofp
LFLAGS += -lgcc -lc -lm -lnosys --specs=nano.specs

# -------- Application Sources --------
APP_SRCS = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/main_freertos.c \
	$(SRC_DIR)/ti_drivers_config.c \
	$(SRC_DIR)/sensor_bme280.c \
	$(SRC_DIR)/sensor_sgp30.c \
	$(SRC_DIR)/sensor_bh1750.c \
	$(SRC_DIR)/sensor_bmv080.c \
	$(SRC_DIR)/sensor_mq7.c \
	$(SRC_DIR)/sensor_mic.c \
	$(SRC_DIR)/co_alarm.c \
	$(SRC_DIR)/wifi_mqtt.c \
	$(SRC_DIR)/sl_event_handlers.c

# -------- SDK Startup --------
STARTUP_SRC = $(SDK_INSTALL_DIR)/kernel/freertos/startup/startup_cc32xx_gcc.c

# -------- FreeRTOS Kernel Sources --------
FREERTOS_SRCS = \
	$(FREERTOS_KERNEL)/tasks.c \
	$(FREERTOS_KERNEL)/queue.c \
	$(FREERTOS_KERNEL)/list.c \
	$(FREERTOS_KERNEL)/timers.c \
	$(FREERTOS_KERNEL)/event_groups.c \
	$(FREERTOS_KERNEL)/stream_buffer.c \
	$(FREERTOS_KERNEL)/portable/GCC/ARM_CM3/port.c \
	$(FREERTOS_KERNEL)/portable/MemMang/heap_4.c

# -------- TI DPL (Driver Porting Layer) for FreeRTOS --------
DPL_DIR = $(SDK_INSTALL_DIR)/kernel/freertos/dpl
DPL_SRCS = \
	$(DPL_DIR)/ClockP_freertos.c \
	$(DPL_DIR)/DebugP_freertos.c \
	$(DPL_DIR)/HwiPCC32XX_freertos.c \
	$(DPL_DIR)/MutexP_freertos.c \
	$(DPL_DIR)/PowerCC32XX_freertos.c \
	$(DPL_DIR)/QueueP_freertos.c \
	$(DPL_DIR)/SemaphoreP_freertos.c \
	$(DPL_DIR)/StaticAllocs_freertos.c \
	$(DPL_DIR)/SwiP_freertos.c \
	$(DPL_DIR)/SystemP_freertos.c \
	$(DPL_DIR)/TaskP_freertos.c \
	$(DPL_DIR)/TimestampPCC32XX_freertos.c \
	$(DPL_DIR)/AppHooks_freertos.c

# -------- TI POSIX for FreeRTOS (GCC) --------
# Note: PTLS.c is for TI compiler, Mtx.c is for IAR — excluded for GCC.
POSIX_DIR = $(SDK_SRC)/ti/posix/freertos
POSIX_SRCS = \
	$(POSIX_DIR)/clock.c \
	$(POSIX_DIR)/memory.c \
	$(POSIX_DIR)/mqueue.c \
	$(POSIX_DIR)/pthread.c \
	$(POSIX_DIR)/pthread_barrier.c \
	$(POSIX_DIR)/pthread_cond.c \
	$(POSIX_DIR)/pthread_mutex.c \
	$(POSIX_DIR)/pthread_rwlock.c \
	$(POSIX_DIR)/sched.c \
	$(POSIX_DIR)/semaphore.c \
	$(POSIX_DIR)/sleep.c \
	$(POSIX_DIR)/timer.c \
	$(POSIX_DIR)/aeabi_portable.c

# -------- All Sources --------
ALL_SRCS = $(APP_SRCS) $(STARTUP_SRC) $(FREERTOS_SRCS) $(DPL_SRCS) $(POSIX_SRCS)

# -------- Object Files (all go into build/) --------
ALL_OBJS = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(ALL_SRCS)))

# -------- VPATH for finding source files --------
VPATH = $(sort $(dir $(ALL_SRCS)))

# -------- Rules --------
.PHONY: all clean size

all: $(BUILD)/$(TARGET).bin size

$(BUILD)/%.o: %.c | $(BUILD)
	@echo "CC  $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).out: $(ALL_OBJS)
	@echo "LD  $@"
	@$(LD) $(ALL_OBJS) $(LFLAGS) -o $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).out
	@echo "BIN $@"
	@$(HEX) -O binary $< $@

size: $(BUILD)/$(TARGET).out
	@$(SZ) $<

$(BUILD):
	@mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

# -------- Syntax-only check (no link) --------
.PHONY: check
check:
	@echo "Checking syntax..."
	@for f in $(APP_SRCS); do \
		echo "  $$f"; \
		$(CC) $(CFLAGS) -fsyntax-only $$f || exit 1; \
	done
	@echo "All files OK"
