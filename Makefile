ifndef DPP_ROOT
$(error DPP_ROOT is undefined)
endif

include $(DPP_ROOT)/Makefile.inc

PROTOBUF_OBJECT = protobuf-build/protobuf-c.o
PROTOBUF_CFLAGS_PRIVATE = -Wno-unused-but-set-variable

FLATBUFFERS_LIB = flatbuffers-build/lib/libflatccrt.a
FLATBUFFERS_CFLAGS = -Iflatbuffers-build/include -Wno-misleading-indentation
FLATBUFFERS_FLATC = flatbuffers-flatcc-build/bin/flatcc

DRIVER0_NAME = driver
DRIVER0_OBJECTS = examples/$(DRIVER0_NAME).o ksocket/ksocket.o ksocket/berkeley.o
DRIVER0_TARGET = $(DRIVER0_NAME).sys

DRIVER1_NAME = driver-protobuf-c
DRIVER1_OBJECTS = examples/$(DRIVER1_NAME).o examples/example.pb-c.o $(PROTOBUF_OBJECT)
DRIVER1_TARGET = $(DRIVER1_NAME).sys

DRIVER2_NAME = driver-protobuf-c-tcp
DRIVER2_OBJECTS = examples/$(DRIVER2_NAME).o ksocket/ksocket.o ksocket/berkeley.o examples/example.pb-c.o $(PROTOBUF_OBJECT)
DRIVER2_TARGET = $(DRIVER2_NAME).sys

DRIVER3_NAME = driver-flatbuffers
DRIVER3_OBJECTS = examples/$(DRIVER3_NAME).o ksocket/ksocket.o ksocket/berkeley.o $(FLATBUFFERS_LIB)
DRIVER3_TARGET = $(DRIVER3_NAME).sys

USERSPACE0_NAME = userspace_client
USERSPACE0_OBJECTS = examples/$(USERSPACE0_NAME).o
USERSPACE0_TARGET = $(USERSPACE0_NAME).exe

USERSPACE1_NAME = userspace_client_protobuf
USERSPACE1_OBJECTS = examples/$(USERSPACE1_NAME).o examples/example.pb-c.o $(PROTOBUF_OBJECT)
USERSPACE1_TARGET = $(USERSPACE1_NAME).exe

# mingw-w64-dpp related
CFLAGS_$(PROTOBUF_OBJECT) = $(PROTOBUF_CFLAGS_PRIVATE)
CUSTOM_CFLAGS = -I. -Iexamples -Werror $(FLATBUFFERS_CFLAGS) -Wl,--exclude-all-symbols -DNDEBUG
DRIVER_LIBS += -lnetio
USER_LIBS += -lws2_32

all: deps $(DRIVER0_TARGET) $(DRIVER1_TARGET) $(DRIVER2_TARGET) $(DRIVER3_TARGET) $(USERSPACE0_TARGET) $(USERSPACE1_TARGET)

%.o: %.cpp
	$(call BUILD_CPP_OBJECT,$<,$@)

%.o: %.c
	$(call BUILD_C_OBJECT,$<,$@)

$(PROTOBUF_OBJECT): protobuf-c/protobuf-c.c
	mkdir -p '$(dir $(PROTOBUF_OBJECT))'
	$(call BUILD_C_OBJECT,$<,$@)

$(DRIVER0_TARGET): $(DRIVER0_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER0_OBJECTS),$@)

$(DRIVER1_TARGET): $(DRIVER1_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER1_OBJECTS),$@)

$(DRIVER2_TARGET): $(DRIVER2_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER2_OBJECTS),$@)

$(DRIVER3_TARGET): $(FLATBUFFERS_LIB) $(DRIVER3_OBJECTS)
	$(call LINK_CPP_KERNEL_TARGET,$(DRIVER3_OBJECTS),$@)

$(USERSPACE0_TARGET): $(USERSPACE0_OBJECTS)
	$(call LINK_CPP_USER_TARGET,$(USERSPACE0_OBJECTS),$@)

$(USERSPACE1_TARGET): $(USERSPACE1_OBJECTS)
	$(call LINK_CPP_USER_TARGET,$(USERSPACE1_OBJECTS),$@)

deps: $(FLATBUFFERS_LIB) $(FLATBUFFERS_FLATC)

$(FLATBUFFERS_LIB):
	cmake -S flatcc -B flatbuffers-build \
		-DCMAKE_INSTALL_PREFIX=/ \
		-DCMAKE_C_COMPILER="$(realpath $(CC))" \
		-DCMAKE_SYSTEM_NAME="Windows" \
		-DCMAKE_C_FLAGS='$(CFLAGS) $(FLATBUFFERS_CFLAGS_PRIVATE)' \
		-DCMAKE_BUILD_TYPE=Release \
		-DFLATCC_ALLOW_WERROR=ON -DFLATCC_COVERAGE=OFF -DFLATCC_CXX_TEST=OFF \
		-DFLATCC_REFLECTION=OFF -DFLATCC_RTONLY=ON -DFLATCC_TEST=OFF -DFLATCC_INSTALL=ON \
		-DFLATCC_GNU_POSIX_MEMALIGN=OFF
	cmake --build flatbuffers-build
	make -C flatbuffers-build install DESTDIR="$(realpath .)/flatbuffers-build"

$(FLATBUFFERS_FLATC):
	cmake -S flatcc -B flatbuffers-flatcc-build \
		-DCMAKE_INSTALL_PREFIX=/ \
		-DCMAKE_BUILD_TYPE=Release \
		-DFLATCC_ALLOW_WERROR=ON -DFLATCC_COVERAGE=OFF -DFLATCC_CXX_TEST=OFF \
		-DFLATCC_REFLECTION=OFF -DFLATCC_RTONLY=OFF -DFLATCC_TEST=OFF -DFLATCC_INSTALL=ON
	cmake --build flatbuffers-flatcc-build
	make -C flatbuffers-flatcc-build install DESTDIR="$(realpath .)/flatbuffers-flatcc-build"

generate: $(FLATBUFFERS_FLATC)
	@echo 'Generating flatbuffer files..'
	$(FLATBUFFERS_FLATC) -a -o examples examples/monster.fbs
	@echo '=========================================='
	@echo '= You need protobuf-c to make this work! ='
	@echo '=========================================='
	@echo 'Get it here: https://github.com/protobuf-c/protobuf-c'
	@echo
	protoc-c --c_out=. examples/example.proto

install: $(DRIVER0_TARGET) $(DRIVER1_TARGET) $(DRIVER2_TARGET) $(DRIVER3_TARGET) $(USERSPACE0_TARGET) $(USERSPACE1_TARGET)
	$(call INSTALL_EXEC_SIGN,$(DRIVER0_TARGET))
	$(call INSTALL_EXEC_SIGN,$(DRIVER1_TARGET))
	$(call INSTALL_EXEC_SIGN,$(DRIVER2_TARGET))
	$(call INSTALL_EXEC_SIGN,$(DRIVER3_TARGET))
	$(call INSTALL_EXEC,$(USERSPACE0_TARGET))
	$(call INSTALL_EXEC,$(USERSPACE1_TARGET))
	$(INSTALL) 'examples/$(DRIVER0_NAME).bat' '$(DESTDIR)/'
	$(INSTALL) 'examples/$(DRIVER1_NAME).bat' '$(DESTDIR)/'
	$(INSTALL) 'examples/$(DRIVER2_NAME).bat' '$(DESTDIR)/'
	$(INSTALL) 'examples/$(DRIVER3_NAME).bat' '$(DESTDIR)/'

clean:
	rm -f $(DRIVER0_OBJECTS) $(DRIVER1_OBJECTS) $(DRIVER2_OBJECTS) $(DRIVER3_OBJECTS)
	rm -f $(DRIVER0_TARGET) $(DRIVER0_TARGET).map \
		$(DRIVER1_TARGET) $(DRIVER1_TARGET).map \
		$(DRIVER2_TARGET) $(DRIVER2_TARGET).map \
		$(DRIVER3_TARGET) $(DRIVER3_TARGET).map
	rm -f $(USERSPACE0_OBJECTS) $(USERSPACE1_OBJECTS)
	rm -f $(USERSPACE0_TARGET) $(USERSPACE1_TARGET)

distclean: clean
	rm -rf flatbuffers-build flatbuffers-flatcc-build

.NOTPARALLEL: clean distclean
.PHONY: all install clean distclean
.DEFAULT_GOAL := all
