# Makefile for CS-202 course - spring 2025
# Author: Ludovic Mermod, Jean-Cédric Chappelier & Édouard Bugnion
# Date: March 2025

# main targets

# 	style: astyle formatting
# 	doc: create documentation
#       feedback: execute tests within a container from source repo
#       check: local tests

# Note: builds with address sanitizer by default

TEST_DIR = $(PWD)/../done/tests

## You can change the tag to run week07, week08, etc.
IMAGE := chappeli/cs202-feedback:latest

#########################################################################
# DO NOT EDIT BELOW THIS LINE
#

# ======================================================================
# Compiler and Link

export CC = gcc

CFLAGS += -g -fanalyzer
CFLAGS += -pedantic -Wall
CFLAGS += -Wextra -Wfloat-equal -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings \
-Wconversion -Wunreachable-code -Wcast-qual -W -Wformat=2 -Winit-self -Wuninitialized

CFLAGS += -Wcast-align

## may require: export ASAN_OPTIONS=allocator_may_return_null=1
#               export ASAN_OPTIONS=verify_asan_link_order=0
# different -fsanitize options are available, including -fsanitize=memory
CPPFLAGS += -fsanitize=address
LDFLAGS  += -fsanitize=address

#ifdef CS212_STAFF
#TODO: restore: CPPFLAGS += -DCS212_STAFF -DWEEK=15
CFLAGS += -DCS212_STAFF -DWEEK=8
#endif

ifdef DEBUG
# add the debug flag, may need to comment this line when doing make feedback
#TODO : Make feedback should build with -UDEBUG
CPPFLAGS += -DDEBUG
endif

.PHONY: all all-deferred

# list of C files the .o of which shall not be automatically added to OBJS list
EXCLUDE_SRCS = udp-test-client.c udp-test-server.c network.c fake_network.c dkvs-client.c

SRCS = $(filter-out $(EXCLUDE_SRCS), $(wildcard *.c))

LDLIBS += -lm -lssl -lcrypto

OBJS=$(subst .c,.o,$(SRCS))

all:: all-deferred

fake-dkvs-client: $(OBJS) dkvs-client.o fake_network.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

FAKE_HEADERS := $(wildcard fake*.h)
FAKE_TARGETS := $(patsubst fake%.h,fake-dkvs-client-%,$(FAKE_HEADERS))

fakes: $(FAKE_TARGETS)

fake_network_%.o: fake_network.c fake%.h
	$(CC) -c $(CFLAGS) $(LDFLAGS) -o $@ $< -include $(word 2,$^)

fake-dkvs-client-%: dkvs-client.o fake_network_%.o  $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Computes the valid targets for `all`
ifneq (,$(wildcard ./*dkvs-client.c))
TARGETS = fake-dkvs-client
TARGETS += $(FAKE_TARGETS)

endif

ifneq (,$(wildcard ./udp-test-*.c))
TARGETS += udp udp-test-client udp-test-server
endif

all-deferred:: $(TARGETS)


.PHONY: depend clean new static-check check release doc

# automatically generate the dependencies
# including .h dependencies !

depend: .depend

.depend: $(SRCS)
	bash -c "cat $(foreach SRC,$(SRCS),<($(CC) $(CFLAGS) -MM -MT $(SRC:.c=.o) $(SRC))) > ./.depend"

ifneq ($(MAKECMDGOALS),clean)
-include .depend
endif

clean::
	-@/bin/rm -f *.o *~  .depend $(TARGETS) fake-dkvs-client-*
	$(MAKE) -C $(TEST_DIR)/unit dist-clean
	$(MAKE) -C $(TEST_DIR)/end-to-end clean

new: clean all

static-check:
	CCC_CC=$(CC) scan-build -analyze-headers --status-bugs -maxloop 64 make -j1 new

style:
	astyle -n -o -A8 -xt0 *.[ch]

unit-tests:
	$(MAKE) SRC_DIR=$${PWD} SRC_OBJS="$(OBJS)" -B -C $(TEST_DIR)/unit

all-unit-tests:
	$(MAKE) -i SRC_DIR=$${PWD} SRC_OBJS="$(OBJS)" -B -C $(TEST_DIR)/unit

test-%:
	$(MAKE) SRC_DIR=$${PWD} SRC_OBJS="$(OBJS)" -B -C $(TEST_DIR)/unit $*

$(TEST_DIR)/unit/%:
	$(MAKE) SRC_DIR=$${PWD} SRC_OBJS="$(OBJS)" -B -C $(TEST_DIR)/unit unit-test-$*



dbg: $(TEST_DIR)/unit/$(EXE)
ifndef TEST
	$(error Missing test name)
endif
ifndef EXE
	$(error Missing executable name)
endif
	gdb -ex 'set debuginfod enabled off' \
	    -ex 'cd $(TEST_DIR)/unit/'       \
	    -ex 'set environment CK_FORK=no' \
	    -ex 'break $(TEST)'              \
	    -ex 'run'                        \
	    -ex 'layout src'                 \
	    '$(TEST_DIR)/unit/unit-test-$(EXE)'

define e2e_test
$(MAKE) SRC_DIR=$${PWD} SRC_OBJS="$(OBJS)" -B -C $(TEST_DIR)/end-to-end $1 ;
endef
end2end-tests: $(TARGETS)
	$(call e2e_test,week02)
	$(call e2e_test,week03-find)
	$(call e2e_test,week03-substr)
	$(call e2e_test,week03-cat)
 

all-end2end-tests: $(TARGETS)
	- $(call e2e_test,week02)
	- $(call e2e_test,week03-find)
	- $(call e2e_test,week03-substr)
	- $(call e2e_test,week03-cat)

check: end2end-tests unit-tests
all-tests: all-end2end-tests all-unit-tests

## --------------------------------------------------
# target to do all checks before releasing a new version by staff
release: new check style static-check clean

## --------------------------------------------------
# target to make documentation
doc: Doxyfile
	doxygen Doxyfile

## ======================================================================
## Feedback

REPO := $(shell git config --get remote.origin.url)
SSH_DIR := $(HOME)/.ssh

PLATFORM := linux/amd64 # Set your platform; currently supported: linux/amd64, linux/arm/v7, linux/arm64; use x86_64 for ARM hosts
PLATFORM_FLAG = --platform=$(PLATFORM)

#DOCKER_OPTIONS += --privileged
DOCKER_OPTIONS += -e REPO=$(REPO) -v $(SSH_DIR):/opt/.ssh $(PLATFORM_FLAG) $(IMAGE)

.PHONY: feedback feedback-pull feedback-debug feedback-VM-CO feedback-debug-VM-CO clone-ssh clean-fake-ssh

feedback-pull:
	@echo Will use $(REPO) inside container
	@docker pull $(PLATFORM_FLAG) $(IMAGE)
feedback: feedback-pull
	@docker run -it --rm $(DOCKER_OPTIONS)
feedback-debug: feedback-pull
	@docker run -it --rm --entrypoint=./debugmode.sh $(DOCKER_OPTIONS)

clone-ssh:
	@-$(eval SSH_DIR := $(HOME)/.$(shell date "+%s;$$"|sha256sum|cut -c-32))
	@cp -r $(HOME)/.ssh/. $(SSH_DIR)

clean-fake-ssh:
	@case $(SSH_DIR) in $(HOME)/\.????????????????????????????????) $(RM) -fr $(SSH_DIR) ;; *) echo "Dare not remove \"$(SSH_DIR)\"" ;; esac

feedback-VM-CO: clone-ssh feedback clean-fake-ssh

feedback-debug-VM-CO: clone-ssh feedback-debug clean-fake-ssh

## ======================================================================
## Submit

SUBMIT_SCRIPT=../provided/submit.sh

.PHONY: submit1 submit

submit1: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 1

submit: submit1
