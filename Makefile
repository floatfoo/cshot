TARGET = cshot
CC = gcc

RELEASE = release
DEBUG = debug

ifndef $(BUILD)
BUILD = $(RELEASE)
endif

SRC_PATH = src/

ifeq ($(BUILD), $(RELEASE))
BUILD_PATH = $(RELEASE)/
else
BUILD_PATH = $(DEBUG)/
endif

OBJ_PATH = $(BUILD_PATH)obj/

SRC = $(wildcard $(SRC_PATH)*.c)
OBJ = $(patsubst $(SRC_PATH)%.c, $(OBJ_PATH)%.o, $(SRC))

ifeq ($(BUILD), $(DEBUG))
CFLAGS = -Wall -Wextra -Werror -v -O3 -std=c11
else
CFLAGS = -Wall -Wextra -Werror -v -O1 -std=c11 -g -pg
endif
LDFLAGS = -lX11 -lpng


$(TARGET): $(OBJ)
	$(LINK.c) $^ -o $(BUILD_PATH)$@

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	$(COMPILE.c) -o $@ $<

install:
	cp $(BUILD_PATH)$(TARGET) $(DESTDIR)$(EXEC_PREFIX)/bin/
	chmod 755 $(DESTDIR)$(EXEC_PREFIX)/bin/$(TARGET)

uninstall:
	$(RM) $(DESTDIR)$(EXEC_PREFIX)/bin/$(TARGET)

clean:
	$(RM) release/obj/*.o release/$(TARGET) *.png
	$(RM) debug/obj/*.o debug/$(TARGET) *.png

