PREFIX=/usr/local
SRC_DIR=.
BUILD_DIR=.

ADDITIONAL_INCLUDE_PATH:=
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_S),Darwin)
    XCRUN := $(shell xcrun --show-sdk-path >/dev/null 2>&1 && echo yes || echo no)
    ifeq ($(XCRUN),yes)
      ADDITIONAL_INCLUDE_PATH := $(shell xcrun --show-sdk-path)/usr/include
    endif
endif

LDFLAGS =
OBJO=-o #trailing space is important
EXEO=-o #trailing space is important
ifeq ($(OS),Windows_NT)
  EXE=.exe
  ifeq ($(CC),cc)
    ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
      CC=gcc
    else
      CC=cl
    endif
  endif
  ifeq ($(CC),gcc)
    CFLAGS += -fPIC -g -std=gnu11 -Wno-abi -fsigned-char
    CFLAGS += -fno-tree-sra
    COPTFLAGS = -O3 -DNDEBUG
    CDEBFLAGS =
    CDEB2FLAGS = -Wall -Wextra -g3 -dwarf4 -fsanitize=address -fsanitize=undefined -fno-sanitize=alignment
    CFLAGS += $(COPTFLAGS)
    LDFLAGS=-Wl,--stack,8388608
    LD2FLAGS= $(LDFLAGS)
    MIR_LIBS=-lm -lkernel32 -lpsapi
  else ifeq ($(CC),cl)
    COPTFLAGS = -O2 -DNDEBUG
    CDEBFLAGS = -Od -Z7
    CDEB2FLAGS = $(CDEBFLAGS)
    CFLAGS += -nologo $(COPTFLAGS)
    LDFLAGS= -nologo -F 8388608
    LD2FLAGS= $(LDFLAGS)
    MIR_LIBS=
    OBJO=-Fo:
    EXEO=-Fe:
  endif

  CPPFLAGS = -I$(SRC_DIR)
  LDLIBS   = $(MIR_LIBS)
  COMPILE = $(CC) $(CPPFLAGS) $(CFLAGS)
  ifeq ($(CC),gcc)
    COMPILE += -MMD -MP
  endif
  LINK = $(CC) $(LDFLAGS)
  COMPILE_AND_LINK = $(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)

else
  EXE=
  CC=gcc
  CFLAGS += -fPIC -g -std=gnu11 -Wno-abi -fsigned-char
  ifneq ($(ADDITIONAL_INCLUDE_PATH),)
    CFLAGS += -DADDITIONAL_INCLUDE_PATH=\"$(ADDITIONAL_INCLUDE_PATH)\"
  endif

  ifeq ($(shell $(CC) -v 2>&1 | grep -c "clang version"), 0)
    ifeq ($(shell $(CC) -fno-tree-sra 2>&1 | grep -c 'fno-tree-sra'), 0)
      CFLAGS += -fno-tree-sra
    endif
    ifeq ($(shell $(CC) -fno-ipa-cp-clone 2>&1 | grep -c 'fno-ipa-cp-clone'), 0)
      CFLAGS += -fno-ipa-cp-clone
    endif
  endif

  MIR_LIBS=-lm -ldl
  COPTFLAGS = -O3 -DNDEBUG
  CDEBFLAGS =
  CDEB2FLAGS = -Wall -Wextra -Wshadow -g3 -fsanitize=address -fsanitize=undefined -fno-sanitize=alignment
  LD2FLAGS =  -fsanitize=address -fsanitize=undefined  -fno-sanitize=alignment
  CFLAGS += $(COPTFLAGS)
  CPPFLAGS = -I$(SRC_DIR)
  LDLIBS   = $(MIR_LIBS)
  COMPILE = $(CC) $(CPPFLAGS) -MMD -MP $(CFLAGS)
  LINK = $(CC) $(LDFLAGS)
  COMPILE_AND_LINK = $(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)
endif

API_VERSION=1
MAJOR_VERSION=0
MINOR_VERSION=1
GITCOMMIT:= $(shell cd $(SRC_DIR) && git log -1 --pretty='%H')
CFLAGS += -DGITCOMMIT=$(GITCOMMIT)

ifeq ($(CC),cl)
  OBJSUFF=obj
  LIBSUFF=lib
  SOLIB=libmir.dll
else
  OBJSUFF=o
  LIBSUFF=a
  ifeq ($(OS),Windows_NT)
    ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
      SONAME=libmir.so.$(API_VERSION)
      SOLIBFLAGS=-shared -Wl,-soname,$(SONAME)
      SOLIB=$(SONAME).$(MAJOR_VERSION).$(MINOR_VERSION)
    else
      SOLIB=libmir.dll
    endif
  else
    ifeq ($(UNAME_S),Darwin)
      SOLIBVERSION=$(API_VERSION).$(MAJOR_VERSION)
      SOLIB=libmir.$(API_VERSION).dylib
      SOLIBFLAGS=-dynamiclib -install_name "$(SOLIB)" -current_version $(SOLIBVERSION).$(MINOR_VERSION) -compatibility_version $(SOLIBVERSION)
    else
      SONAME=libmir.so.$(API_VERSION)
      SOLIBFLAGS=-shared -Wl,-soname,$(SONAME)
      SOLIB=$(SONAME).$(MAJOR_VERSION).$(MINOR_VERSION)
    endif
  endif
endif

C2M_BOOTSTRAP_FLAGS = -DMIR_BOOTSTRAP
C2M_BOOTSTRAP_FLAGS0 := $(C2M_BOOTSTRAP_FLAGS)
ifeq ($(shell sh $(SRC_DIR)/check-threads.sh), ok)
  ifneq ($(CC),cl)
    MIR_LIBS += -lpthread
    CFLAGS += -DC2MIR_PARALLEL
    C2M_BOOTSTRAP_FLAGS += -DC2MIR_PARALLEL
  endif
endif

L2M_EXE=
L2M_TEST=
ifneq ($(shell test -f /usr/include/llvm-c/Core.h|echo 1), 1)
L2M_EXE += $(BUILD_DIR)/l2m$(EXE)
L2M_TEST += l2m-test$(EXE)
endif

EXECUTABLES=$(BUILD_DIR)/c2m$(EXE) $(BUILD_DIR)/m2b$(EXE) $(BUILD_DIR)/b2m$(EXE) $(BUILD_DIR)/b2ctab$(EXE) $(L2M_EXE) $(BUILD_DIR)/mir-bin-run$(EXE)

Q=@

# Entries should be used for building and installation
.PHONY: all debug install uninstall clean test bench

all: $(BUILD_DIR)/libmir.$(LIBSUFF) $(BUILD_DIR)/$(SOLIB) $(EXECUTABLES)

debug: CFLAGS:=$(subst $(COPTFLAGS),$(CDEBFLAGS),$(CFLAGS))
debug: $(BUILD_DIR)/libmir.$(LIBSUFF) $(BUILD_DIR)/$(SOLIB) $(EXECUTABLES)

debug2: CFLAGS:=$(subst $(COPTFLAGS),$(CDEB2FLAGS),$(CFLAGS))
debug2: LDFLAGS:=$(LD2FLAGS)
debug2: $(BUILD_DIR)/libmir.$(LIBSUFF) $(BUILD_DIR)/$(SOLIB) $(EXECUTABLES)

install: $(BUILD_DIR)/libmir.$(LIBSUFF) $(BUILD_DIR)/$(SOLIB) $(EXECUTABLES) | $(PREFIX)/include $(PREFIX)/lib $(PREFIX)/bin
	install -m a+r $(SRC_DIR)/mir.h $(SRC_DIR)/mir-dlist.h $(SRC_DIR)/mir-varr.h $(SRC_DIR)/mir-htab.h\
		       $(SRC_DIR)/mir-gen.h $(SRC_DIR)/c2mir/c2mir.h $(PREFIX)/include
	install -m a+r $(BUILD_DIR)/libmir.$(LIBSUFF) $(BUILD_DIR)/$(SOLIB) $(PREFIX)/lib
ifeq ($(OS),Windows_NT)
else
    ifeq ($(UNAME_S),Darwin)
	rm -f $(PREFIX)/lib/libmir.dylib
	ln -s $(PREFIX)/lib/$(SOLIB) $(PREFIX)/lib/libmir.dylib
	install_name_tool -change "$(SOLIB)" "$(PREFIX)/lib/$(SOLIB)" $(PREFIX)/lib/$(SOLIB)
    else
	rm -f $(PREFIX)/lib/$(SONAME)
	ln -s $(PREFIX)/lib/$(SOLIB) $(PREFIX)/lib/$(SONAME)
    endif
endif
	install -m a+rx $(EXECUTABLES) $(PREFIX)/bin

$(PREFIX)/include $(PREFIX)/lib $(PREFIX)/bin:
	   mkdir -p $@

uninstall: $(BUILD_DIR)/libmir.$(LIBSUFF) $(BUILD_DIR)/$(SOLIB) $(EXECUTABLES) | $(PREFIX)/include $(PREFIX)/lib $(PREFIX)/bin
	$(RM) $(PREFIX)/include/mir.h $(PREFIX)/include/mir-dlist.h $(PREFIX)/include/mir-varr.h $(PREFIX)/include/mir-htab.h\
		       $(PREFIX)/include/mir-gen.h $(PREFIX)/include/c2mir.h
	$(RM) $(PREFIX)/lib/libmir.$(LIBSUFF) $(PREFIX)/lib/$(SOLIB)
ifeq ($(OS),Windows_NT)
else
    ifeq ($(UNAME_S),Darwin)
	rm -f $(PREFIX)/lib/libmir.dylib
    else
	rm -f $(PREFIX)/lib/$(SONAME)
    endif
endif
	$(RM) $(EXECUTABLES:$(BUILD_DIR)/%=$(PREFIX)/bin/%)
	-rmdir $(PREFIX)/include $(PREFIX)/lib $(PREFIX)/bin
	-rmdir $(PREFIX)

clean: clean-mir clean-c2m clean-utils clean-l2m clean-adt-tests clean-mir-tests clean-mir2c-test clean-bench
	$(RM) $(EXECUTABLES) $(BUILD_DIR)/libmir.$(LIBSUFF) $(BUILD_DIR)/$(SOLIB)

test: readme-example-test mir-bin-run-test c2mir-test basic-test

test-all: adt-test simplify-test io-test scan-test mir2c-test $(L2M-TEST) test

bench: interp-bench gen-bench gen-bench2 io-bench mir2c-bench c2mir-sieve-bench gen-speed c2mir-bench
	@echo ==============================Bench is done

# ------------------ MIR --------------------------
MIR_SRC:=$(SRC_DIR)/mir.c $(SRC_DIR)/mir-gen.c
MIR_BUILD:=$(MIR_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.$(OBJSUFF))

$(BUILD_DIR)/%.$(OBJSUFF): $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(COMPILE) -c $< $(OBJO)$@

.PHONY: clean-mir
clean-mir:
	$(RM) $(MIR_BUILD) $(MIR_BUILD:.$(OBJSUFF)=.d)

-include $(MIR_BUILD:.$(OBJSUFF)=.d)

# ------------------ LIBMIR -----------------------
$(BUILD_DIR)/libmir.$(LIBSUFF): $(BUILD_DIR)/mir.$(OBJSUFF) $(BUILD_DIR)/mir-gen.$(OBJSUFF) $(BUILD_DIR)/c2mir/c2mir.$(OBJSUFF)
ifeq ($(CC),cl)
	lib -nologo $^ -OUT:$@
else
	$(AR) rcs $@ $^
endif

# ------------------ LIBMIR SO --------------------
$(BUILD_DIR)/$(SOLIB): $(BUILD_DIR)/mir.$(OBJSUFF) $(BUILD_DIR)/mir-gen.$(OBJSUFF) $(BUILD_DIR)/c2mir/c2mir.$(OBJSUFF)
ifeq ($(CC),cl)
	$(CC) -nologo -D_USRDLL -D_WINDLL $^ -link -DLL -OUT:$@
else
	$(CC) $(SOLIBFLAGS) -o $@ $^
endif

# ------------------ C2M --------------------------
C2M_SRC:=$(SRC_DIR)/c2mir/c2mir.c $(SRC_DIR)/c2mir/c2mir-driver.c
C2M_BUILD:=$(C2M_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.$(OBJSUFF))

$(BUILD_DIR)/c2mir/%.$(OBJSUFF): $(SRC_DIR)/c2mir/%.c | $(BUILD_DIR)/c2mir
	$(COMPILE) -c $< $(OBJO)$@

$(BUILD_DIR)/c2m$(EXE): $(C2M_BUILD) $(BUILD_DIR)/libmir.$(LIBSUFF) | $(BUILD_DIR)
	$(LINK) $^ $(LDLIBS) $(EXEO)$@

$(BUILD_DIR)/c2mir:
	   mkdir -p $@

.PHONY: clean-c2m
clean-c2m:
	$(RM) $(C2M_BUILD) $(C2M_BUILD:.$(OBJSUFF)=.d)

-include $(C2M_BUILD:.$(OBJSUFF)=.d)

# ------------------ MIR RUN ----------------------

MIR_RUN_SRC:=$(SRC_DIR)/mir-bin-run.c
MIR_RUN_BUILD:=$(MIR_RUN_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.$(OBJSUFF))

$(BUILD_DIR)/mir-bin-run$(EXE): $(MIR_RUN_BUILD) $(BUILD_DIR)/libmir.$(LIBSUFF) | $(BUILD_DIR)
	$(LINK) $^ $(LDLIBS) $(EXEO)$@ $(BUILD_DIR)/libmir.$(LIBSUFF)

.PHONY: clean-mir-bin-run
clean-mir-bin-run:
	$(RM) $(MIR_RUN_BUILD) $(MIR_RUN_BUILD:.$(OBJSUFF)=.d)

-include $(MIR_RUN_BUILD:.$(OBJSUFF)=.d)

# ------------------ L2M --------------------------
L2M_SRC:=$(SRC_DIR)/llvm2mir/llvm2mir.c $(SRC_DIR)/llvm2mir/llvm2mir-driver.c
L2M_BUILD:=$(L2M_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.$(OBJSUFF))

$(BUILD_DIR)/llvm2mir/%.$(OBJSUFF): $(SRC_DIR)/llvm2mir/%.c | $(BUILD_DIR)/llvm2mir
	$(COMPILE) -c $< $(OBJO)$@

$(BUILD_DIR)/l2m$(EXE): $(L2M_BUILD) $(BUILD_DIR)/libmir.$(LIBSUFF) | $(BUILD_DIR)
	$(LINK) $^ $(LDLIBS) -lLLVM $(OBJO)$@ $(BUILD_DIR)/libmir.$(LIBSUFF)

$(BUILD_DIR)/llvm2mir:
	   mkdir -p $@

.PHONY: clean-l2m
clean-l2m:
	$(RM) $(L2M_BUILD) $(L2M_BUILD:.$(OBJSUFF)=.d)

-include $(L2M_BUILD:.$(OBJSUFF)=.d)

# ------------------ Common for utils -------------

$(BUILD_DIR)/mir-utils:
	   mkdir -p $@

$(BUILD_DIR)/mir-utils/%.$(OBJSUFF): $(SRC_DIR)/mir-utils/%.c | $(BUILD_DIR)/mir-utils
	$(COMPILE) -c $< $(OBJO)$@

.PHONY: clean-utils
clean-utils: clean-m2b clean-b2m clean-b2ctab

# ------------------ M2B --------------------------
M2B_SRC:=$(SRC_DIR)/mir-utils/m2b.c
M2B_BUILD:=$(M2B_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.$(OBJSUFF))

$(BUILD_DIR)/m2b$(EXE): $(M2B_BUILD) $(BUILD_DIR)/libmir.$(LIBSUFF) | $(BUILD_DIR)
	$(LINK) $^ $(LDLIBS) $(EXEO)$@ $(BUILD_DIR)/libmir.$(LIBSUFF)

.PHONY: clean-m2b
clean-m2b:
	$(RM) $(M2B_BUILD) $(M2B_BUILD:.$(OBJSUFF)=.d)

-include $(M2B_BUILD:.$(OBJSUFF)=.d)

# ------------------ B2M --------------------------
B2M_SRC:=$(SRC_DIR)/mir-utils/b2m.c
B2M_BUILD:=$(B2M_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.$(OBJSUFF))

$(BUILD_DIR)/b2m$(EXE): $(B2M_BUILD) $(BUILD_DIR)/libmir.$(LIBSUFF) | $(BUILD_DIR)
	$(LINK) $^ $(LDLIBS) $(EXEO)$@ $(BUILD_DIR)/libmir.$(LIBSUFF)

.PHONY: clean-b2m
clean-b2m:
	$(RM) $(B2M_BUILD) $(B2M_BUILD:.$(OBJSUFF)=.d)

-include $(B2M_BUILD:.$(OBJSUFF)=.d)

# ------------------ B2CTAB --------------------------
B2CTAB_SRC:=$(SRC_DIR)/mir-utils/b2ctab.c
B2CTAB_BUILD:=$(B2CTAB_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.$(OBJSUFF))

$(BUILD_DIR)/b2ctab$(EXE): $(B2CTAB_BUILD) $(BUILD_DIR)/libmir.$(LIBSUFF) | $(BUILD_DIR)
	$(LINK) $^ $(LDLIBS) $(EXEO)$@  $(BUILD_DIR)/libmir.$(LIBSUFF)

.PHONY: clean-b2ctab
clean-b2ctab:
	$(RM) $(B2CTAB_BUILD) $(B2CTAB_BUILD:.$(OBJSUFF)=.d)

-include $(B2CTAB_BUILD:.$(OBJSUFF)=.d)

# ------------------ ADT tests --------------------------

.PHONY: clean-adt-tests
.PHONY: adt-test varr-test dlist-test bitmap-test htab-test reduce-test

adt-test: varr-test dlist-test bitmap-test htab-test reduce-test

varr-test: $(BUILD_DIR)/adt-tests
	$(COMPILE_AND_LINK) $(SRC_DIR)/adt-tests/mir-varr-test.c $(EXEO)$(BUILD_DIR)/adt-tests/varr-test$(EXE)
	$(BUILD_DIR)/adt-tests/varr-test$(EXE)

dlist-test: $(BUILD_DIR)/adt-tests
	$(COMPILE_AND_LINK) $(SRC_DIR)/adt-tests/mir-dlist-test.c $(EXEO)$(BUILD_DIR)/adt-tests/dlist-test$(EXE)
	$(BUILD_DIR)/adt-tests/dlist-test$(EXE)

bitmap-test: $(BUILD_DIR)/adt-tests
	$(COMPILE_AND_LINK) $(SRC_DIR)/adt-tests/mir-bitmap-test.c $(EXEO)$(BUILD_DIR)/adt-tests/bitmap-test$(EXE)
	$(BUILD_DIR)/adt-tests/bitmap-test$(EXE)

htab-test: $(BUILD_DIR)/adt-tests
	$(COMPILE_AND_LINK) $(SRC_DIR)/adt-tests/mir-htab-test.c $(EXEO)$(BUILD_DIR)/adt-tests/htab-test$(EXE)
	$(BUILD_DIR)/adt-tests/htab-test$(EXE)

reduce-test: $(BUILD_DIR)/adt-tests
	$(COMPILE_AND_LINK) $(SRC_DIR)/adt-tests/mir-reduce-test.c $(EXEO)$(BUILD_DIR)/adt-tests/reduce-test$(EXE)
	$(BUILD_DIR)/adt-tests/reduce-test$(EXE) $(SRC_DIR)/c2mir/c2mir.c

$(BUILD_DIR)/adt-tests:
	mkdir -p $@

clean-adt-tests:
	$(RM) $(BUILD_DIR)/adt-tests/varr-test$(EXE) $(BUILD_DIR)/adt-tests/dlist-test$(EXE)
	$(RM) $(BUILD_DIR)/adt-tests/bitmap-test$(EXE)
	$(RM) $(BUILD_DIR)/adt-tests/htab-test$(EXE) $(BUILD_DIR)/adt-tests/reduce-test$(EXE)

# ------------------ Common for MIR tests ---------------------

$(BUILD_DIR)/mir-tests:
	mkdir -p $@

$(BUILD_DIR)/run-test$(EXE): $(BUILD_DIR)/mir.$(OBJSUFF) $(BUILD_DIR)/mir-gen.$(OBJSUFF) $(SRC_DIR)/mir-tests/run-test.c | $(BUILD_DIR)/mir-tests
	$(COMPILE_AND_LINK) $^ $(LDLIBS) $(EXEO)$@

.PHONY: clean-mir-tests
clean-mir-tests: clean-mir-utility-tests clean-mir-interp-tests clean-mir-gen-tests clean-readme-example-test
	$(RM) $(BUILD_DIR)/run-test$(EXE)

# ------------------ MIR utility tests ------------------------

.PHONY: simplify-test scan-test io-test clean-mir-utility-tests

simplify-test: $(BUILD_DIR)/mir.$(OBJSUFF) $(SRC_DIR)/mir-tests/simplify.c
	$(COMPILE_AND_LINK) $^ $(EXEO)$(BUILD_DIR)/simplify-test $(LDLIBS) && $(BUILD_DIR)/simplify-test$(EXE)

hello-test: $(BUILD_DIR)/mir.$(OBJSUFF) $(BUILD_DIR)/mir-gen.$(OBJSUFF) $(SRC_DIR)/mir-tests/hello.c
	$(COMPILE_AND_LINK) $^ $(EXEO)$(BUILD_DIR)/hello-test $(LDLIBS) && $(BUILD_DIR)/hello-test$(EXE)

scan-test: $(BUILD_DIR)/mir.$(OBJSUFF) $(SRC_DIR)/mir-tests/scan-test.c
	$(COMPILE_AND_LINK) $^ $(EXEO)$(BUILD_DIR)/scan-test $(LDLIBS) && $(BUILD_DIR)/scan-test$(EXE)

io-test: $(BUILD_DIR)/mir.$(OBJSUFF) $(SRC_DIR)/mir-tests/io.c
	$(COMPILE_AND_LINK) $^ $(EXEO)$(BUILD_DIR)/io-test $(LDLIBS) && $(BUILD_DIR)/io-test$(EXE)

clean-mir-utility-tests:
	$(RM) $(BUILD_DIR)/run-test$(EXE) $(BUILD_DIR)/simplify-test$(EXE)
	$(RM) $(BUILD_DIR)/hello-test$(EXE)
	$(RM) $(BUILD_DIR)/scan-test$(EXE) $(BUILD_DIR)/io-test$(EXE)

# ------------------ BASIC compiler example -------------------
.PHONY: basic-test clean-basic basic-bench

$(BUILD_DIR)/basic/basicc$(EXE): $(BUILD_DIR)/mir.$(OBJSUFF) $(BUILD_DIR)/mir-gen.$(OBJSUFF) \
        $(SRC_DIR)/examples/basic/basicc.c $(SRC_DIR)/examples/basic/basic_runtime.c \
        $(SRC_DIR)/examples/basic/kitty/kitty.c $(SRC_DIR)/examples/basic/kitty/lodepng.c
	mkdir -p $(BUILD_DIR)/basic
	$(COMPILE_AND_LINK) -DBASIC_SRC_DIR=\"$(SRC_DIR)\" $^ -lm $(EXEO)$@

$(BUILD_DIR)/basic/kitty_test$(EXE): \
        $(SRC_DIR)/examples/basic/kitty/kitty_test.c \
        $(SRC_DIR)/examples/basic/kitty/kitty.c \
        $(SRC_DIR)/examples/basic/kitty/lodepng.c
	mkdir -p $(BUILD_DIR)/basic
	$(COMPILE_AND_LINK) $^ -lm $(EXEO)$@

basic-test: $(BUILD_DIR)/basic/basicc$(EXE) $(BUILD_DIR)/basic/kitty_test$(EXE)
	$(BUILD_DIR)/basic/basicc$(EXE) $(SRC_DIR)/examples/basic/hello.bas > $(BUILD_DIR)/basic/hello.out
	diff $(SRC_DIR)/examples/basic/hello.out $(BUILD_DIR)/basic/hello.out
	$(BUILD_DIR)/basic/basicc$(EXE) $(SRC_DIR)/examples/basic/relop.bas > $(BUILD_DIR)/basic/relop.out
	diff $(SRC_DIR)/examples/basic/relop.out $(BUILD_DIR)/basic/relop.out
	printf '2\n3\n' | $(BUILD_DIR)/basic/basicc$(EXE) $(SRC_DIR)/examples/basic/adder.bas > $(BUILD_DIR)/basic/adder.out
	diff $(SRC_DIR)/examples/basic/adder.out $(BUILD_DIR)/basic/adder.out
	printf 'MIR\nY\n' | $(BUILD_DIR)/basic/basicc$(EXE) $(SRC_DIR)/examples/basic/string.bas > $(BUILD_DIR)/basic/string.out
	diff $(SRC_DIR)/examples/basic/string.out $(BUILD_DIR)/basic/string.out
	$(BUILD_DIR)/basic/basicc$(EXE) $(SRC_DIR)/examples/basic/strfuncs.bas > $(BUILD_DIR)/basic/strfuncs.out
	diff $(SRC_DIR)/examples/basic/strfuncs.out $(BUILD_DIR)/basic/strfuncs.out
	$(BUILD_DIR)/basic/basicc$(EXE) $(SRC_DIR)/examples/basic/instr.bas > $(BUILD_DIR)/basic/instr.out
	diff $(SRC_DIR)/examples/basic/instr.out $(BUILD_DIR)/basic/instr.out
	$(BUILD_DIR)/basic/kitty_test$(EXE) > $(BUILD_DIR)/basic/kitty_test.out
	diff $(SRC_DIR)/examples/basic/kitty/kitty_test.out $(BUILD_DIR)/basic/kitty_test.out
