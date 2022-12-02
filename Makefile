TARGET = cshot
CC = gcc

SRC_PATH = src/
OBJ_PATH = obj/

SRC = $(wildcard $(SRC_PATH)*.c)
OBJ = $(patsubst $(SRC_PATH)%.c, $(OBJ_PATH)%.o, $(SRC))

FLAGS = -Wall -g -O3
LINK = -lxcb

$(TARGET): $(OBJ)
	$(CC) $(FLAGS) $(LINK) $(OBJ) -o $(TARGET)

$(OBJ): $(SRC)
	$(CC) -c $(FLAGS) $(LINK) $(SRC) -o $(OBJ_PATH)main.o

clean:
	rm $(TARGET) $(OBJ_PATH)*.o
