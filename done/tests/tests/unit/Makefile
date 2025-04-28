# ======================================================================
# Useful for feedback

CC ?= clang

TARGETS = hashtable
TARGETS += args
TARGETS += client
TARGETS += node-list
TARGETS += node
TARGETS += ring

CFLAGS += -g

CPPFLAGS += -fsanitize=address
LDFLAGS  += -fsanitize=address

EXECS=$(foreach name,$(TARGETS),unit-test-$(name))

.SILENT:
.PHONY: unit-tests all $(TARGETS) execs

## removed undesired feature
## # first creates all exec, **then** launch them
## all: execs unit-tests
all: unit-tests

unit-tests: $(TARGETS)

execs: $(EXECS)

# target shortcuts : compile & run the tests
$(TARGETS): %: unit-test-%
	@./$^ && echo "==== " $< " SUCCEEDED =====" || { echo "==== " $< " FAILED ====="; false; }
	@printf '\n'

# ======================================================================
DATA_DIR ?= ../data/
SRC_DIR  ?= ../../done
CFLAGS  += '-I$(SRC_DIR)' -DCS202_TEST -DDATA_DIR='"$(DATA_DIR)"'
LDFLAGS += '-L$(SRC_DIR)'

CFLAGS  += -fsanitize=address
LDFLAGS += -fsanitize=address

LDLIBS += -lcheck -lm -lrt -pthread -lsubunit -lcrypto

OBJS = $(addprefix $(SRC_DIR)/,$(SRC_OBJS))
ifneq (,$(wildcard $(SRC_DIR)/fake_network.c))
OBJS += $(SRC_DIR)/fake_network.o
endif

## OBJS += 

# ======================================================================
unit-test-%.o: unit-test-%.c $(SRC_DIR)/*.h
unit-test-%: unit-test-%.o $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS) $(LDFLAGS)


.PHONY: clean dist-clean reset

clean::
	-$(RM) *.o *~

dump-clean:
	-$(RM) $(DATA_DIR)/dump*

dist-clean: clean dump-clean
	-$(RM) $(foreach T,$(TARGETS),unit-test-$(T))

reset: dist-clean all
