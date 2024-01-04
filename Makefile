TARGET = cshot
CC = gcc

RELEASE = release
DEBUG = debug

ifndef $(BUILD)
BUILD = $(RELEASE)
endif

SRC_PATH = src/
APP_PATH = app/
TEST_PATH = test/

ifeq ($(BUILD), $(RELEASE))
BUILD_PATH = $(RELEASE)/
else
BUILD_PATH = $(DEBUG)/
endif

OBJ_PATH = $(BUILD_PATH)obj/
OBJ_APP_PATH = $(OBJ_PATH)app/
OBJ_TEST_PATH = $(OBJ_PATH)test/

SRC = $(wildcard $(SRC_PATH)*.c)
APP = $(wildcard $(APP_PATH)*.c)
TEST = $(wildcard $(TEST_PATH)*.c)
OBJ = $(patsubst $(SRC_PATH)%.c, $(OBJ_PATH)%.o, $(SRC))
OBJ_APP = $(patsubst $(APP_PATH)%.c, $(OBJ_APP_PATH)%.o, $(APP))
OBJ_TEST = $(patsubst $(TEST_PATH)%.c, $(OBJ_TEST_PATH)%.o, $(TEST))

ifeq ($(BUILD), $(DEBUG))
CFLAG = -Wall -Wextra -Werror -O3 -std=c11 -v -fpie
else
CFLAGS = -Wall -Wextra -Werror -O1 -std=c11 -v -fpie -g -pg -fprofile-arcs -ftest-coverage
endif
LDFLAGS = -lX11 -lpng


$(TARGET): $(OBJ) $(OBJ_APP)
	$(LINK.c) $^ -o $(BUILD_PATH)$@

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(@D)
	$(COMPILE.c) -o $@ $<

$(OBJ_APP_PATH)%.o: $(APP_PATH)%.c
	@mkdir -p $(@D)
	$(COMPILE.c) -o $@ $<

install:
	cp $(BUILD_PATH)$(TARGET) $(DESTDIR)$(EXEC_PREFIX)/bin/
	chmod 755 $(DESTDIR)$(EXEC_PREFIX)/bin/$(TARGET)

uninstall:
	$(RM) $(DESTDIR)$(EXEC_PREFIX)/bin/$(TARGET)

clean:
	$(RM) -r release/
	$(RM) -r debug/

format:
	clang-format --sort-includes -i src/*.{h,c}

all_test: $(OBJ) $(OBJ_TEST)
	$(LINK.c) $(OBJ) $(OBJ_TEST) -o $(BUILD_PATH)$@

$(OBJ_TEST_PATH)%.o: $(TEST_PATH)%.c
	@mkdir -p $(@D)
	$(COMPILE.c) -o $@ $<


.PHONY: install unintall clean format test

