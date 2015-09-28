
ifeq ($(strip $(CTRULIB)),)
$(error "Please set CTRULIB in your environment: $ export CTRULIB=<path to ctrulib>")
endif

VERSION := 1.0.0

BUGREPORT := https://github.com/cpp3ds/gl3ds

include $(DEVKITARM)/base_rules

LIBDIRS	:=  $(CTRULIB)

export INCLUDE :=  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                   -I$(CURDIR)/include


ARCH    :=  -march=armv6k -mtune=mpcore -mfloat-abi=hard

CFLAGS  :=  -g -Wall -O2 \
            -fno-strict-aliasing -ffunction-sections -fdata-sections \
            $(ARCH) $(INCLUDE)

ASFLAGS	:=	-g $(ARCH)

DEFINITIONS := -DPACKAGE_VERSION=\"$(VERSION)\" -DPACKAGE_BUGREPORT=\"$(BUGREPORT)\"

CFILES  :=  $(wildcard src/**/*.c)  $(wildcard src/**.c)

export LD      :=  $(CC)
export OFILES  :=  $(CFILES:src/%.c=build/%.o)

.PHONY: all clean

all: dir lib/libgl3ds.a

dir:
	@mkdir -p build/c11
	@mkdir -p build/math
	@mkdir -p build/util
	@mkdir -p build/drivers
	@mkdir -p lib

clean:
	@rm -rf build lib
	@echo "Successfully cleaned."

build/%.o: src/%.c
	$(CC) -MMD -MP -MF build/$*.d $(CFLAGS) $(DEFINITIONS) -c $< -o $@

lib/libgl3ds.a: $(OFILES)
