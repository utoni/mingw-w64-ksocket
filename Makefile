ifndef DPP_ROOT
$(error DPP_ROOT is undefined)
endif

include $(DPP_ROOT)/Makefile.inc

DRIVER0_NAME = driver
DRIVER0_OBJECTS = $(DRIVER0_NAME).o ksocket.o berkeley.o
DRIVER0_TARGET = $(DRIVER0_NAME).sys
DRIVER0_CFLAGS = -I. -Wl,--exclude-all-symbols -DNDEBUG

DRIVER1_NAME = driver-protobuf-c
DRIVER1_OBJECTS = $(DRIVER1_NAME).o protobuf-c/protobuf-c.o protobuf-c/example.pb-c.o
DRIVER1_TARGET = $(DRIVER1_NAME).sys
DRIVER1_CFLAGS = -I. -Iprotobuf-c -Wl,--exclude-all-symbols -DNDEBUG

USERSPACE_NAME = userspace_client
USERSPACE_OBJECTS = $(USERSPACE_NAME).o
USERSPACE_TARGET = $(USERSPACE_NAME).exe

# mingw-w64-dpp related
CFLAGS_protobuf-c/protobuf-c.o = -Wno-unused-but-set-variable
CUSTOM_CFLAGS = $(DRIVER0_CFLAGS)
DRIVER_LIBS += -lnetio
USER_LIBS += -lws2_32

all: $(DRIVER0_TARGET) $(DRIVER1_TARGET) $(USERSPACE_TARGET)

%.o: %.cpp
	$(call BUILD_CPP_OBJECT,$<,$@)

%.o: %.c
	$(call BUILD_C_OBJECT,$<,$@)

$(DRIVER0_TARGET): $(DRIVER0_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER0_OBJECTS),$@)

$(DRIVER1_TARGET): $(DRIVER1_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER1_OBJECTS),$@)

$(USERSPACE_TARGET): $(USERSPACE_OBJECTS)
	$(call LINK_CPP_USER_TARGET,$(USERSPACE_OBJECTS),$@)

install: $(DRIVER0_TARGET) $(DRIVER1_TARGET) $(USERSPACE_TARGET)
	$(call INSTALL_EXEC_SIGN,$(DRIVER0_TARGET))
	$(call INSTALL_EXEC_SIGN,$(DRIVER1_TARGET))
	$(call INSTALL_EXEC,$(USERSPACE_TARGET))
	$(INSTALL) '$(DRIVER0_NAME).bat' '$(DESTDIR)/'
	$(INSTALL) '$(DRIVER1_NAME).bat' '$(DESTDIR)/'

clean:
	rm -f $(DRIVER0_OBJECTS) $(DRIVER1_OBJECTS)
	rm -f $(DRIVER0_TARGET) $(DRIVER0_TARGET).map $(DRIVER1_TARGET) $(DRIVER1_TARGET).map
	rm -f $(USERSPACE_OBJECTS)
	rm -f $(USERSPACE_TARGET)

.NOTPARALLEL: clean
.PHONY: all install clean
.DEFAULT_GOAL := all
