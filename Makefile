CC:=avr-gcc
CCFLAGS:=-mmcu=atmega128 -std=c99 -O2

ifeq ($(DEBUG),1)
	CCFLAGS+=-DDEBUG
endif

LD:=$(CC)
LDFLAGS:=$(CCFLAGS)

OPENOCD:=openocd
OPENOCDFLAGS:=-c "set CPUTAPID 0x7970203F" \
             -f ../scripts/interface/altera-usb-blaster.cfg -c "usb_blaster_pin pin6 s" -c "reset_config srst_only srst_nogate" \
             -f ../scripts/target/atmega128.cfg \
             -c "program firmware.bin reset"

CCFILES:=$(wildcard *.c)
OBJECTS:=$(CCFILES:.c=.o)

all: firmware.bin

flash:
	$(OPENOCD) $(OPENOCDFLAGS)

clean:
	rm -f $(OBJECTS) firmware.bin

firmware.bin: $(OBJECTS)
	$(LD) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CCFLAGS)
