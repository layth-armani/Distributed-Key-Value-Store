.PHONY: test
.SILENT:

CC ?= clang
DATA_DIR ?= ../data/
SRC_DIR  ?= ../../done
CFLAGS  += '-I$(SRC_DIR)' -DCS202_TEST -DDATA_DIR='"$(DATA_DIR)"'
LDFLAGS += '-L$(SRC_DIR)'

CFLAGS += -g

CFLAGS  += -fsanitize=address
LDFLAGS += -fsanitize=address

LDLIBS += -lcheck -lm -lrt -pthread -lsubunit -lcrypto

OBJS = $(addprefix $(SRC_DIR)/,$(SRC_OBJS))
OBJS += $(SRC_DIR)/dkvs-client.o

FAKE_HEADERS := $(wildcard fake*.h)
FAKE_TARGETS := $(patsubst fake%.h,fake-dkvs-client-%,$(FAKE_HEADERS))

PROMPT = '==== end-to-end test'

test: week02 week03-find week03-substr week03-cat

week%: week%.py fakes
	@python $< && echo $(PROMPT) $@ "SUCCEEDED =====" || { echo $(PROMPT) $@ " FAILED ====="; false; }
	@printf '\n'

fakes: $(FAKE_TARGETS)

fake_network_%.o: $(SRC_DIR)/fake_network.c fake%.h
	$(CC) -c $(CFLAGS) -o $@ $< -include $(word 2,$^)

fake-dkvs-client-%: fake_network_%.o  $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f fake-dkvs-client-* fake_network_*.c
