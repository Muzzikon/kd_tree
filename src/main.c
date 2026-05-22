// Главный файл программы.
// Отвечает за загрузку точек, запуск режимов KD-дерева и DBSCAN, а также замер времени.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#include "kd_tree.h"
#include "dbscan.h"
#include "io_utils.h"
#include "bruteforce_utils.h"
#include "cli_handlers.h"

static void print_usage(const char *program_name) {
    printf("Использование:\n");
    printf("  %s <файл.csv> -kd_insert <x,y[,z]>\n", program_name);
    printf("  %s <файл.csv> -kd_nearest <x,y[,z]>\n", program_name);
    printf("  %s <файл.csv> -kd_range <x1,y1[,z1]> <x2,y2[,z2]>\n", program_name);
    printf("  %s <файл.csv> -dbscan <eps> <min_pts>\n", program_name);
}

static int finish_with_cleanup(Node *root, PointArray *data, int code) {
    free_tree(root);
    free(data->points);
    return code;
}

// Точка входа в программу.
// Поддерживает режимы: построение KD-дерева, nearest, range и DBSCAN.
int main(int argc, char *argv[]) {
	if (argc < 3) {
		print_usage(argv[0]);
		return 1;
	}

	printf("Файл: %s\n", argv[1]);
	printf("Операция: %s\n", argv[2]);

    // Сначала загружаем все точки в память, затем по ним строим KD-дерево.
    PointArray data = load_points_array_from_csv(argv[1]);
    if (data.count <= 0) {
        printf("Не удалось загрузить точки из CSV-файла: %s\n", argv[1]);
        printf("Проверьте, что файл существует, находится в папке Tests/ или содержит точки формата x,y,z\n");
        return 1;
    }

    Node *root = NULL;
    for (int i = 0; i < data.count; i++) {
        root = insert(root, data.points[i], i, 0);
    }

    printf("Загружено точек: %d\n", data.count);

    if (strcmp(argv[2], "-kd_insert") == 0) {
        int rc = handle_kd_insert(argv[1], argc, argv, &root, &data);
        return finish_with_cleanup(root, &data, rc);
    }
    // Режим поиска ближайшего соседа с сравнением KD-Tree и brute force.
    else if (strcmp(argv[2], "-kd_nearest") == 0) {
        int rc = handle_kd_nearest(root, &data, argc, argv);
        return finish_with_cleanup(root, &data, rc);
    }
    // Режим диапазонного поиска с проверкой совпадения результатов.
    else if (strcmp(argv[2], "-kd_range") == 0) {
        int rc = handle_kd_range(argv[1], root, &data, argc, argv);
        return finish_with_cleanup(root, &data, rc);
    }
    // Режим кластеризации DBSCAN с выводом краткой статистики и времени работы.
    else if (strcmp(argv[2], "-dbscan") == 0) {
        int rc = handle_dbscan(argv[1], &data, argc, argv);
        return finish_with_cleanup(root, &data, rc);
    }
    else {
        printf("Неизвестная операция: %s\n", argv[2]);
        print_usage(argv[0]);
        return finish_with_cleanup(root, &data, 1);
    }

    // Освобождаем память перед завершением программы.
    return finish_with_cleanup(root, &data, 0);
}
