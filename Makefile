.PHONY: all clean run-nearest run-range run-dbscan run-all benchmark visualize

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
	./$(TARGET) Tests/Circle_clean_xyz.csv -kd_range 4.5,-0.8,-0.2 5.5,0.8,0.2

run-dbscan: $(TARGET)
	./$(TARGET) Tests/Two_clusters_noise_xyz.csv -dbscan 0.8 5

run-all: run-nearest run-range run-dbscan

benchmark: $(TARGET)
	./$(TARGET) Tests/Circle_clean_xyz.csv -kd_nearest 5,0,0
	./$(TARGET) Tests/Sphere_xyz.csv -kd_nearest 3,3,3
	./$(TARGET) Tests/Circle_clean_xyz.csv -kd_range 4.5,-0.8,-0.2 5.5,0.8,0.2
	./$(TARGET) Tests/Sphere_xyz.csv -kd_range 2,2,2 4,4,4

dbscan-examples: $(TARGET)
	./$(TARGET) Tests/Two_clusters_noise_xyz.csv -dbscan 0.8 5
	./$(TARGET) Tests/Spiral_xyz.csv -dbscan 0.5 5
	./$(TARGET) Tests/Circle_clean_xyz.csv -dbscan 0.3 5

visualize: $(TARGET)
	rm -rf Results
	./$(TARGET) Tests/Circle_clean_xyz.csv -kd_range 4.5,-0.8,-0.2 5.5,0.8,0.2
	./$(TARGET) Tests/Two_clusters_noise_xyz.csv -dbscan 0.8 5
	python3 scripts/plot_points_2D.py Tests/Circle_clean_xyz.csv
	python3 scripts/plot_nearest_2D.py Tests/Circle_clean_xyz.csv 5,0,0
	python3 scripts/plot_range_2D.py Tests/Circle_clean_xyz.csv Results/CSV/Circle_clean_xyz_range.csv
	python3 scripts/plot_dbscan_2D.py Results/CSV/Two_clusters_noise_xyz_dbscan.csv