
ifeq ($(strip $(CTRULIB)),)
$(error "Please set CTRULIB in your environment: $ export CTRULIB=<path to ctrulib>")
endif

include $(DEVKITARM)/base_rules

PROJECT :=  libgl3ds

LIBDIRS	:=  $(CTRULIB)

export INCLUDE :=  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                   -I$(CURDIR)/include


ARCH    :=  -march=armv6k -mtune=mpcore -mfloat-abi=hard

CFLAGS  :=  -g -Wall -O2 -mword-relocations \
            -fno-strict-aliasing \
            -fomit-frame-pointer -ffast-math \
            $(ARCH) $(INCLUDE)

ASFLAGS	:=	-g $(ARCH)

CFILES  :=  $(wildcard src/*.c)

export LD      :=  $(CC)
export OFILES  :=  $(CFILES:src/%.c=build/%.o)

.PHONY: all clean

all: dir lib/$(PROJECT).a

dir:
	@mkdir -p build
	@mkdir -p lib

clean:
	@rm -rf build lib
	@echo "Successfully cleaned."

build/%.o: src/%.c
	$(CC) -MMD -MP -MF build/$*.d $(CFLAGS) -c $< -o $@

lib/$(PROJECT).a: $(OFILES)
