ARCHES := 64bit 32bit

SOURCES := src/main.c
BINARY := lumpystuff
CC_FLAGS := -I./compat-headers -Wall -std=c11 -fpic
CC_FLAGS_Debug := -g -O0
CC_FLAGS_Release := -O3 -DNDEBUG
CC_FLAGS_64bit := -m64
CC_FLAGS_32bit := -m32
LD_FLAGS := -fpie
ifeq ($(OS)/$(CC),Windows_NT/gcc)
# Fix wmain entry point with MinGW.
LD_FLAGS += -municode
endif
LD_FLAGS_Debug :=
LD_FLAGS_Release :=
LD_FLAGS_64bit := -m64
LD_FLAGS_32bit := -m32

DEFAULT_CONFIG      := Release
DEFAULT_USE_MSBUILD := 1

BIN_SUFFIX :=
OBJ_SUFFIX := .o
ifeq ($(OS),Windows_NT)
    BIN_SUFFIX := .exe
endif

ifeq ($(CONFIG),)
    CONFIG := $(DEFAULT_CONFIG)
endif

ifeq ($(filter $(CONFIG),Debug Release),)
$(error "CONFIG must be one of: Debug, Release")
endif

ifeq ($(OS),Windows_NT)
    ifeq ($(USE_MSBUILD),)
        # TODO: Try to find MSBUILD on the PATH and only use it if it's there.
        USE_MSBUILD := $(DEFAULT_USE_MSBUILD)
    endif
endif

# http://blog.jgc.org/2007/06/escaping-comma-and-space-in-gnu-make.html
, := ,
SPACE :=
SPACE := $(SPACE) $(SPACE)
$(SPACE) := $(SPACE)

help:
	@echo "Usage:"
	@echo "    $(MAKE) [flags] [options] <arches>"
	@echo "Where <arches> can be one or more of: $(subst $ ,$(,)$ ,$(ARCHES))"
	@echo "Recommended flags:"
	@echo "    -jN -k"
	@echo "Default options:"
ifeq ($(OS),Windows_NT)
	@echo "    USE_MSBUILD=$(DEFAULT_USE_MSBUILD)"
endif
	@echo "    CONFIG=$(DEFAULT_CONFIG) (supported: Release, Debug)"
	@echo "Example:"
	@echo "    $(MAKE) -j4 -k 32bit CONFIG=Debug" 

MS_PLAT_64bit := x64
MS_PLAT_32bit := x86

ifneq ($(filter $(MAKEFLAGS),-j),)
    # TODO support passing parallel flag down to MSBuild?
endif

# This simply wraps MSBuild for the entire job of building.
define MSRULE
# Param: arch
$(1):
	@msbuild /v:q /nologo /p:Platform=$(MS_PLAT_$(1)) /p:Configuration=$(CONFIG) lumpystuff.sln
endef

define DIRRULE
# Param: arch
mkdirs_$1:
ifeq ($(OS),Windows_NT)
	-@cmd /c md $(foreach file,$(SOURCES),build\int\$(CONFIG)\$1\$(subst /,\,$(file))\..)
	-@cmd /c md build\$(CONFIG)\$1
else
	@mkdir -p $(foreach file,$(SOURCES),build/int/$(CONFIG)/$1/$(dir $(file)))
	@mkdir -p build/$(CONFIG)/$1
endif
.PHONY: mkdirs_$1
endef

define CCRULE
# Params: arch, file
build/int/$(CONFIG)/$1/$(2:.c=$(OBJ_SUFFIX)): $2
	@$(CC) $(CC_FLAGS) $(CC_FLAGS_$(CONFIG)) $(CC_FLAGS_$1) -MMD -MP -o build/int/$(CONFIG)/$1/$(2:.c=$(OBJ_SUFFIX)) -c $2
-include build/int/$(CONFIG)/$1/$(2:.c=.d)
endef

define LDRULE
# Param: arch
build/$(CONFIG)/$1/lumpystuff$(BIN_SUFFIX): $(foreach file,$(SOURCES),build/int/$(CONFIG)/$1/$(file:.c=$(OBJ_SUFFIX)))
	@$(CC) $(LD_FLAGS) $(LD_FLAGS_$(CONFIG)) $(LD_FLAGS_$1) $(foreach file,$(SOURCES),build/int/$(CONFIG)/$1/$(file:.c=$(OBJ_SUFFIX))) -o build/$(CONFIG)/$1/lumpystuff$(BIN_SUFFIX)
$1: mkdirs_$1 build/$(CONFIG)/$1/lumpystuff$(BIN_SUFFIX)
endef

# Only define relevant rules.
ifeq ($(USE_MSBUILD),1)
    $(foreach arch,$(ARCHES),$(eval $(call MSRULE,$(arch))))
else
    # TODO why does that not work!?
    #$(eval $(call CHECKRULES))
    $(foreach arch,$(ARCHES),$(eval $(call DIRRULE,$(arch))))
    $(foreach arch,$(ARCHES),$(foreach file,$(SOURCES),$(eval $(call CCRULE,$(arch),$(file)))))
    $(foreach arch,$(ARCHES),$(eval $(call LDRULE,$(arch))))
endif

.PHONY: $(ARCHES) help
