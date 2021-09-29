

zroxy:
clean:

CC = gcc
CFLAGS = -g -Wall -Werror
INCLUDE_DIRS := -I/usr/local/include/luajit-2.1/ -I/usr/local/include
LDFLAGS := -L/usr/local/lib 
LIBS := -lluajit-5.1 -llua -ldl -lm  -lssl -lcrypto

src_root := src
src_subdirs := 
build_root := build

o_suffix := .o

# Build list of sources. Iterate every subfolder from $(src_subdirs) list 
# and fetch all existing files with suffixes matching the list.
source_suffixes := .c .cpp .cxx
sources := $(foreach d,$(addprefix $(src_root)/,$(src_subdirs)),$(wildcard $(addprefix $d/*,$(source_suffixes))))

# If src_subdirs make variable is unset, use 'find' command to build list of sources.
# Note that we use the same list of suffixes but tweak them for use with 'find'
ifeq ($(src_subdirs),)
  sources := $(shell find $(src_root) -type f $(foreach s,$(source_suffixes),$(if $(findstring $s,$(firstword $(source_suffixes))),,-o) -name '*$s'))
endif

$(info sources=(INCLUDE_DIRS))

# Build source -> object file mapping.
# We want map $(src_root) -> $(build_root) and copy directory structure 
# of source tree but populated with object files.
objects := $(addsuffix $(o_suffix),$(basename $(patsubst $(src_root)%,$(build_root)%,$(sources))))
$(info objects=$(objects))

# Generate rules for every .o file to depend exactly on corresponding source file.
$(foreach s,$(sources),$(foreach o,$(filter %$(basename $(notdir $s)).o,$(objects)),$(info New rule: $o: $s)$(eval $o: $s)))

# This is how we compile sources:
# First check if directory for the target file exists. 
# If it doesn't run 'mkdir' command.
$(objects): ; $(if $(wildcard $(@D)),,mkdir -p $(@D) &&) $(CC) -c $< -o $@ $(INCLUDE_DIRS) $(CFLAGS)

# Compile all sources.
build: $(objects)

zroxy:
	mkdir -p $(build_root)
	$(CC) $(sources) $(CFLAGS) $(INCLUDE_DIRS) $(LDFLAGS) $(LIBS) -o $(build_root)/$@ $^
	
clean: ; rm -rf $(build_root)

.PHONY: clean zroxy