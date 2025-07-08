WITH_PREFIX     ?= /usr/install
WITH_TRIPLE     ?= riscv64-unknown-elf
WITH_MARCH      ?= rv64gc
WITH_MABI       ?= lp64d
WITH_DRAM_BASE  ?= 0x80000000
WITH_STACK_SIZE ?= 0x01000000
WITH_HEAP_SIZE  ?= 0x10000000
WITH_BOOT_HART  ?= 0x0

PYTHON ?= python
ENVSUBST ?= envsubst
RM ?= rm
INSTALL ?= install

libperch_specs := perch.specs

libperch_c_srcs := \
	bp_trap.c \
	bp_l2_cache.c \
	emulation.c \
	bsg_newlib_intf.c \
	bp_utils.c

libperch_asm_srcs := \
	atomics.S \
	exception.S \
	muldiv.S

libperch_hdrs := \
	bp_asm.h \
	bp_aviary.h \
	bp_config.h \
	bp_emulation.h \
	bp_host.h \
	bp_l2_cache.h \
	bp_trap.h \
	bp_utils.h

libperch_lib := libperch.a
libperch_data := $(libperch_specs)

libperch_c_objs := $(libperch_c_srcs:.c=.o)
libperch_asm_objs := $(libperch_asm_srcs:.S=.o)

PERCH_PREFIX := $(WITH_PREFIX)/$(WITH_TRIPLE)
PERCH_CC  := $(WITH_TRIPLE)-gcc
PERCH_CXX := $(WITH_TRIPLE)-g++
PERCH_CFLAGS := -march=$(WITH_MARCH) -mabi=$(WITH_MABI)
PERCH_CFLAGS += -mcmodel=medany -mstrict-align -nostartfiles
PERCH_CFLAGS += -I.

$(libperch_asm_objs): %.o: %.S
	$(PERCH_CC) $(PERCH_CFLAGS) -o $@ -c $<

$(libperch_c_objs): %.o: %.c
	$(PERCH_CC) $(PERCH_CFLAGS) -o $@ -c $<

$(libperch_specs): %.specs: %.specs.template
	$(ENVSUBST) < $< > $@

$(libperch_lib): $(libperch_c_objs) $(libperch_asm_objs)
	$(AR) rcs $@ $^

install_data: $(libperch_lib) $(libperch_data)
	$(INSTALL) -m644 -p -D -t $(PERCH_PREFIX)/lib $^

install_hdrs: $(libperch_hdrs)
	$(INSTALL) -m644 -p -D -t $(PERCH_PREFIX)/include $^

all: $(libperch_lib) $(libperch_data)

install: install_hdrs install_data

clean:
	$(RM) -f $(libperch_c_objs)
	$(RM) -f $(libperch_asm_objs)
	$(RM) -f $(libperch_libs)
	$(RM) -f $(libperch_specs)

