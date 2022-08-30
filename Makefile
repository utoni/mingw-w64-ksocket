ifndef DPP_ROOT
$(error DPP_ROOT is undefined)
endif

include $(DPP_ROOT)/Makefile.inc

DRIVER_NAME = driver
DRIVER_OBJECTS = $(DRIVER_NAME).o ksocket.o berkeley.o
DRIVER_TARGET = $(DRIVER_NAME).sys
DRIVER_CFLAGS = -I. -Wl,--exclude-all-symbols

USERSPACE_NAME = userspace_client
USERSPACE_OBJECTS = $(USERSPACE_NAME).o
USERSPACE_TARGET = $(USERSPACE_NAME).exe

# mingw-w64-ddk-template related
CFLAGS += $(DRIVER_CFLAGS)
DRIVER_LIBS += -lnetio
USER_LIBS += -lws2_32

all: $(DRIVER_TARGET) $(USERSPACE_TARGET)

%.o: %.cpp
	$(call BUILD_CPP_OBJECT,$<,$@)

%.o: %.c
	$(call BUILD_C_OBJECT,$<,$@)

$(DRIVER_TARGET): $(DRIVER_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER_OBJECTS),$@)

$(USERSPACE_TARGET): $(USERSPACE_OBJECTS)
	$(call LINK_CPP_USER_TARGET,$(USERSPACE_OBJECTS),$@)

install: $(DRIVER_TARGET) $(USERSPACE_TARGET)
	$(call INSTALL_EXEC_SIGN,$(DRIVER_TARGET))
	$(call INSTALL_EXEC,$(USERSPACE_TARGET))
	$(INSTALL) '$(DRIVER_NAME).bat' '$(DESTDIR)/'

clean:
	rm -f $(DRIVER_OBJECTS)
	rm -f $(DRIVER_TARGET) $(DRIVER_TARGET).map
	rm -f $(USERSPACE_OBJECTS)
	rm -f $(USERSPACE_TARGET)

.NOTPARALLEL: clean
.PHONY: all install clean
.DEFAULT_GOAL := all
