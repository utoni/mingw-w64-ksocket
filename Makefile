ifndef DPP_ROOT
$(error DPP_ROOT is undefined)
endif

include $(DPP_ROOT)/Makefile.inc

SOCKET_HEADERS = ksocket/berkeley.h ksocket/helper.hpp ksocket/ksocket.h ksocket/wsk.h
SOCKET_OBJECTS = ksocket/ksocket.o ksocket/berkeley.o

PROTOB_HEADERS = protobuf-c/protobuf-c.h
PROTOB_OBJECTS = protobuf-c/protobuf-c.o

CRYPTO_HEADERS =
CRYPTO_OBJECTS = crypto/aes.o crypto/hmac.o crypto/sha1.o crypto/pkcs7_padding.o

STATIC_LIB_OBJS = $(SOCKET_OBJECTS) $(PROTOB_OBJECTS) $(CRYPTO_OBJECTS)

DRIVER0_NAME = driver
DRIVER0_OBJECTS = examples/$(DRIVER0_NAME).o $(SOCKET_OBJECTS)
DRIVER0_TARGET = $(DRIVER0_NAME).sys

DRIVER1_NAME = driver-protobuf-c
DRIVER1_OBJECTS = examples/$(DRIVER1_NAME).o $(PROTOB_OBJECTS) examples/example.pb-c.o
DRIVER1_TARGET = $(DRIVER1_NAME).sys

DRIVER2_NAME = driver-protobuf-c-tcp
DRIVER2_OBJECTS = examples/$(DRIVER2_NAME).o $(SOCKET_OBJECTS) $(PROTOB_OBJECTS) examples/example.pb-c.o
DRIVER2_TARGET = $(DRIVER2_NAME).sys

USERSPACE0_NAME = userspace_client
USERSPACE0_OBJECTS = examples/$(USERSPACE0_NAME).o
USERSPACE0_TARGET = $(USERSPACE0_NAME).exe

USERSPACE1_NAME = userspace_client_protobuf
USERSPACE1_OBJECTS = examples/$(USERSPACE1_NAME).o $(PROTOB_OBJECTS) examples/example.pb-c.o
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

package: $(SOCKET_HEADERS) $(PROTOB_HEADERS) $(STATIC_LIB_OBJS)
	$(call INSTALL_HEADERS,ksocket,$(SOCKET_HEADERS),package)
	$(call INSTALL_HEADERS,protobuf-c,$(PROTOB_HEADERS),package)
	$(call PACKAGE,ksocket,$(STATIC_LIB_OBJS),package)

clean:
	rm -f $(STATIC_LIB_OBJS)
	rm -f $(DRIVER0_OBJECTS) $(DRIVER1_OBJECTS) $(DRIVER2_OBJECTS)
	rm -f $(DRIVER0_TARGET) $(DRIVER0_TARGET).map \
		$(DRIVER1_TARGET) $(DRIVER1_TARGET).map \
		$(DRIVER2_TARGET) $(DRIVER2_TARGET).map
	rm -f $(USERSPACE0_OBJECTS) $(USERSPACE1_OBJECTS)
	rm -f $(USERSPACE0_TARGET) $(USERSPACE1_TARGET)

.NOTPARALLEL: clean
.PHONY: all install package clean
.DEFAULT_GOAL := all
