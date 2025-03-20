# ======================================================================
# Useful for feedback

CC = clang

TARGETS = hashtable

CFLAGS += -g

CPPFLAGS += -fsanitize=address
LDFLAGS  += -fsanitize=address

EXECS=$(foreach name,$(TARGETS),unit-test-$(name))

.PHONY: unit-tests all $(TARGETS) execs

## removed undesired feature
## # first creates all exec, **then** launch them
## all: execs unit-tests
all: unit-tests

unit-tests: $(TARGETS)

execs: $(EXECS)

# target shortcuts : compile & run the tests
hashtable: unit-test-hashtable
	@./$^ && echo "==== " $< " SUCCEEDED =====" || { echo "==== " $< " FAILED ====="; false; }
	@printf '\n'
network-fake: unit-test-network-fake
	@./$^ && echo "==== " $< " SUCCEEDED =====" || { echo "==== " $< " FAILED ====="; false; }
	@printf '\n'
args: unit-test-args
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

OBJS = 
OBJS += $(SRC_DIR)/util.o $(SRC_DIR)/error.o

# ======================================================================
unit-test-hashtable.o: unit-test-hashtable.c $(SRC_DIR)/hashtable.h
unit-test-hashtable: unit-test-hashtable.o $(SRC_DIR)/hashtable.o $(SRC_DIR)/error.o

# ======================================================================
unit-test-args.o: unit-test-args.c $(SRC_DIR)/args.h
unit-test-args: unit-test-args.o $(SRC_DIR)/args.o $(SRC_DIR)/error.o

# ======================================================================
unit-test-network-fake: unit-test-network-fake.o $(SRC_DIR)/hashtable.o $(SRC_DIR)/client.o $(SRC_DIR)/node.o $(SRC_DIR)/args.o $(SRC_DIR)/fake_network.o

.PHONY: clean dist-clean reset

clean::
	-$(RM) *.o *~

dump-clean:
	-$(RM) $(DATA_DIR)/dump*

dist-clean: clean dump-clean
	-$(RM) $(foreach T,$(TARGETS),unit-test-$(T))

reset: dist-clean all
