ifndef DPP_ROOT
$(error DPP_ROOT is undefined)
endif

include $(DPP_ROOT)/Makefile.inc

DRIVER0_NAME = driver
DRIVER0_OBJECTS = examples/$(DRIVER0_NAME).o ksocket/ksocket.o ksocket/berkeley.o
DRIVER0_TARGET = $(DRIVER0_NAME).sys

DRIVER1_NAME = driver-protobuf-c
DRIVER1_OBJECTS = examples/$(DRIVER1_NAME).o protobuf-c/protobuf-c.o examples/example.pb-c.o
DRIVER1_TARGET = $(DRIVER1_NAME).sys

DRIVER2_NAME = driver-protobuf-c-tcp
DRIVER2_OBJECTS = examples/$(DRIVER2_NAME).o ksocket/ksocket.o ksocket/berkeley.o protobuf-c/protobuf-c.o examples/example.pb-c.o
DRIVER2_TARGET = $(DRIVER2_NAME).sys

USERSPACE0_NAME = userspace_client
USERSPACE0_OBJECTS = examples/$(USERSPACE0_NAME).o
USERSPACE0_TARGET = $(USERSPACE0_NAME).exe

USERSPACE1_NAME = userspace_client_protobuf
USERSPACE1_OBJECTS = examples/$(USERSPACE1_NAME).o protobuf-c/protobuf-c.o examples/example.pb-c.o
USERSPACE1_TARGET = $(USERSPACE1_NAME).exe

# mingw-w64-dpp related
CFLAGS_protobuf-c/protobuf-c.o = -Wno-unused-but-set-variable
CUSTOM_CFLAGS = -I. -Wl,--exclude-all-symbols -DNDEBUG
DRIVER_LIBS += -lnetio
USER_LIBS += -lws2_32

all: $(DRIVER0_TARGET) $(DRIVER1_TARGET) $(DRIVER2_TARGET) $(USERSPACE0_TARGET) $(USERSPACE1_TARGET)

%.o: %.cpp
	$(call BUILD_CPP_OBJECT,$<,$@)

%.o: %.c
	$(call BUILD_C_OBJECT,$<,$@)

$(DRIVER0_TARGET): $(DRIVER0_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER0_OBJECTS),$@)

$(DRIVER1_TARGET): $(DRIVER1_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER1_OBJECTS),$@)

$(DRIVER2_TARGET): $(DRIVER2_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER2_OBJECTS),$@)

$(USERSPACE0_TARGET): $(USERSPACE0_OBJECTS)
	$(call LINK_CPP_USER_TARGET,$(USERSPACE0_OBJECTS),$@)

$(USERSPACE1_TARGET): $(USERSPACE1_OBJECTS)
	$(call LINK_CPP_USER_TARGET,$(USERSPACE1_OBJECTS),$@)

generate:
	@echo '=========================================='
	@echo '= You need protobuf-c to make this work! ='
	@echo '=========================================='
	@echo 'Get it here: https://github.com/protobuf-c/protobuf-c'
	@echo
	protoc-c --c_out=. examples/example.proto

install: $(DRIVER0_TARGET) $(DRIVER1_TARGET) $(DRIVER2_TARGET) $(USERSPACE0_TARGET) $(USERSPACE1_TARGET)
	$(call INSTALL_EXEC_SIGN,$(DRIVER0_TARGET))
	$(call INSTALL_EXEC_SIGN,$(DRIVER1_TARGET))
	$(call INSTALL_EXEC_SIGN,$(DRIVER2_TARGET))
	$(call INSTALL_EXEC,$(USERSPACE0_TARGET))
	$(call INSTALL_EXEC,$(USERSPACE1_TARGET))
	$(INSTALL) 'examples/$(DRIVER0_NAME).bat' '$(DESTDIR)/'
	$(INSTALL) 'examples/$(DRIVER1_NAME).bat' '$(DESTDIR)/'
	$(INSTALL) 'examples/$(DRIVER2_NAME).bat' '$(DESTDIR)/'

clean:
	rm -f $(DRIVER0_OBJECTS) $(DRIVER1_OBJECTS) $(DRIVER2_OBJECTS)
	rm -f $(DRIVER0_TARGET) $(DRIVER0_TARGET).map \
		$(DRIVER1_TARGET) $(DRIVER1_TARGET).map \
		$(DRIVER2_TARGET) $(DRIVER2_TARGET).map
	rm -f $(USERSPACE0_OBJECTS) $(USERSPACE1_OBJECTS)
	rm -f $(USERSPACE0_TARGET) $(USERSPACE1_TARGET)

.NOTPARALLEL: clean
.PHONY: all install clean
.DEFAULT_GOAL := all
