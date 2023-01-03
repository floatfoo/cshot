TARGET = cshot
CC = gcc

SRC_PATH = src/
OBJ_PATH = obj/

SRC = $(wildcard $(SRC_PATH)*.c)
OBJ = $(patsubst $(SRC_PATH)%.c, $(OBJ_PATH)%.o, $(SRC))

CFLAGS = -Wall -Werror -v -pg -g -O3 -std=c99
LDFLAGS = -lX11 -lpng

$(TARGET): $(OBJ)
	$(LINK.c) $^ -o $@

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	$(COMPILE.c) -o $@ $<

install:
	cp $(TARGET) $(DESTDIR)$(EXEC_PREFIX)/bin
	chmod 755 $(DESTDIR)$(EXEC_PREFIX)$(TARGET)

uninstall:
	$(RM) /usr/local/bin/$(TARGET)

clean:
	$(RM) $(OBJ_PATH)*.o $(TARGET) *.png
