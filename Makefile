CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS = -lm

TARGET = robot_spatial

SRC = src/main.c \
      src/kd_tree.c \
      src/io_utils.c \
      src/bruteforce_utils.c \
      src/dbscan.c \
      src/grid_index.c \
      src/cli_handlers.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run-nearest: $(TARGET)
	./$(TARGET) Tests/Circle_clean_xyz.csv -kd_nearest 5,0,0

run-range: $(TARGET)
	./$(TARGET) Tests/Circle_clean_xyz.csv -kd_range 0,0,0 3

run-dbscan: $(TARGET)
	./$(TARGET) Tests/Two_clusters_noise_xyz.csv -dbscan 1.5 3