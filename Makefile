TARGET = cshot
CC = gcc

SRC_PATH = src/
OBJ_PATH = obj/

SRC = $(wildcard $(SRC_PATH)*.c)
OBJ = $(patsubst $(SRC_PATH)%.c, $(OBJ_PATH)%.o, $(SRC))

CFLAGS = -Wall -g -O1
LDFLAGS = -lX11

$(TARGET): $(OBJ)
	$(LINK.c) $< -o $@

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	$(COMPILE.c) -o $@ $<

clean:
	$(RM) $(OBJ_PATH)*.o $(TARGET)
