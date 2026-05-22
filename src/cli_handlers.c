// Обработчики CLI-команд.
// Здесь собрана логика режимов вставки, поиска и кластеризации.
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include "cli_handlers.h"
#include "dbscan.h"
#include "io_utils.h"
#include "bruteforce_utils.h"

// Печатает результат поиска ближайшей точки в едином формате.
static void print_nearest_result(const char *label, Point target, Point result) {
    if (result.x == DBL_MAX && result.y == DBL_MAX && result.z == DBL_MAX) {
        printf("%s: ближайший сосед не найден\n", label);
    }
    else {
        printf("%s: ближайший сосед к точке (%lf, %lf, %lf) — это точка (%lf, %lf, %lf)\n", label, target.x, target.y, target.z, result.x, result.y, result.z);
    }
}

static int same_point(Point a, Point b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static int parse_positive_double(const char *text, double *value) {
    char *end;

    errno = 0;
    *value = strtod(text, &end);

    return text != end && *end == '\0' && errno != ERANGE && *value > 0.0;
}

static int parse_positive_int(const char *text, int *value) {
    char *end;
    long parsed;

    errno = 0;
    parsed = strtol(text, &end, 10);

    if (text == end || *end != '\0' || errno == ERANGE || parsed <= 0 || parsed > INT_MAX) {
        return 0;
    }

    *value = (int)parsed;
    return 1;
}

// Обрабатывает вставку новой точки в массив и KD-дерево.
int handle_kd_insert(const char *input_filename, int argc, char *argv[], Node **root, PointArray *data) {
    Point new_point;
    Point *tmp_points;
    char *output_filename;

    if (argc < 4) {
        printf("Для -kd_insert нужно передать точку, например 1.0,2.0 или 1.0,2.0,3.0\n");
        return 1;
    }

    if (!parse_query_point(argv[3], &new_point)) {
        printf("Неверный формат точки: %s\n", argv[3]);
        return 1;
    }

    // Расширяем массив точек перед вставкой новой записи.
    tmp_points = realloc(data->points, (data->count + 1) * sizeof(Point));
    if (tmp_points == NULL) {
        printf("Не удалось выделить память для вставки новой точки\n");
        return 1;
    }

    data->points = tmp_points;
    data->points[data->count] = new_point;
    *root = insert(*root, new_point, data->count, 0);
    data->count++;

    printf("Точка (%.6f, %.6f, %.6f) успешно добавлена\n", new_point.x, new_point.y, new_point.z);
    printf("Теперь в наборе точек: %d\n", data->count);

    // Сохраняем обновлённый набор точек в отдельный CSV-файл.
    output_filename = make_insert_output_filename(input_filename);
    if (output_filename == NULL) {
        printf("Не удалось сформировать имя выходного файла после вставки\n");
        return 1;
    }

    if (save_points_csv(output_filename, data->points, data->count)) {
        printf("Обновлённый набор точек сохранён в %s\n", output_filename);
    }
    else {
        printf("Не удалось сохранить обновлённый набор точек\n");
    }

    free(output_filename);
    return 0;
}

// Обрабатывает nearest neighbor и сравнивает результат с brute force.
int handle_kd_nearest(Node *root, PointArray *data, int argc, char *argv[]) {
    Point target;
    Point nearest;
    Point brute;
    clock_t kd_start, kd_end;
    clock_t brute_start, brute_end;
    double kd_time;
    double brute_time;

    if (argc < 4) {
        printf("Для -kd_nearest нужно передать точку-запрос, например 1.0,2.0 или 1.0,2.0,3.0\n");
        return 1;
    }

    if (!parse_query_point(argv[3], &target)) {
        printf("Неверный формат точки: %s\n", argv[3]);
        return 1;
    }

    // Замеряем отдельно время KD-Tree и полного перебора.
    kd_start = clock();
    nearest = nearest_neighbor(root, target, 0);
    kd_end = clock();

    brute_start = clock();
    brute = brute_force_nearest(data->points, data->count, target);
    brute_end = clock();

    kd_time = ((double)(kd_end - kd_start)) / CLOCKS_PER_SEC;
    brute_time = ((double)(brute_end - brute_start)) / CLOCKS_PER_SEC;

    print_nearest_result("KD-Tree", target, nearest);
    print_nearest_result("Brute force", target, brute);

    if (same_point(nearest, brute)) {
        printf("Результаты KD-Tree и Brute force совпадают\n");
    }
    else {
        printf("Результаты KD-Tree и Brute force различаются\n");
    }

    printf("Время KD-Tree: %.6f сек.\n", kd_time);
    printf("Время Brute force: %.6f сек.\n", brute_time);

    return 0;
}

// Обрабатывает диапазонный поиск и сравнивает результаты двух подходов.
int handle_kd_range(const char *input_filename, Node *root, PointArray *data, int argc, char *argv[]) {
    Point lower;
    Point upper;
    Point *kd_result;
    Point *brute_result;
    int kd_count = 0;
    int brute_count = 0;
    clock_t kd_start, kd_end;
    clock_t brute_start, brute_end;
    double kd_time;
    double brute_time;
    char *output_filename;

    if (argc < 5) {
        printf("Для -kd_range нужно передать диапазон, например: 1.0,2.0 4.0,5.0 или 1.0,2.0,3.0 4.0,5.0,6.0\n");
        return 1;
    }

    // Буферы для результатов KD-Tree и brute force.
    kd_result = malloc(data->count * sizeof(Point));
    brute_result = malloc(data->count * sizeof(Point));

    if (kd_result == NULL || brute_result == NULL) {
        printf("Не удалось выделить память для результатов диапазонного поиска\n");
        free(kd_result);
        free(brute_result);
        return 1;
    }

    if (!parse_query_point(argv[3], &lower) || !parse_query_point(argv[4], &upper)) {
        printf("Неверный формат диапазона\n");
        free(kd_result);
        free(brute_result);
        return 1;
    }

    if (lower.x > upper.x || lower.y > upper.y || lower.z > upper.z) {
        printf("Неверный диапазон: нижняя граница должна быть меньше или равна верхней\n");
        free(kd_result);
        free(brute_result);
        return 1;
    }

    // Запускаем оба варианта поиска и сравниваем время работы.
    kd_start = clock();
    range_query(root, lower, upper, 0, kd_result, &kd_count);
    kd_end = clock();

    brute_start = clock();
    brute_count = brute_force_range(data->points, data->count, lower, upper, brute_result);
    brute_end = clock();

    kd_time = ((double)(kd_end - kd_start)) / CLOCKS_PER_SEC;
    brute_time = ((double)(brute_end - brute_start)) / CLOCKS_PER_SEC;

    printf("KD-Tree: найдено точек в диапазоне: %d\n", kd_count);
    printf("Brute force: найдено точек в диапазоне: %d\n", brute_count);
    printf("Время KD-Tree: %.6f сек.\n", kd_time);
    printf("Время Brute force: %.6f сек.\n", brute_time);

    // При равном количестве точек сравниваем сами результаты поэлементно.
    if (kd_count == brute_count) {
        qsort(kd_result, kd_count, sizeof(Point), compare_points);
        qsort(brute_result, brute_count, sizeof(Point), compare_points);

        if (points_equal(kd_result, brute_result, kd_count)) {
            printf("Результаты KD-Tree и Brute force полностью совпадают\n");
        }
        else {
            printf("Количество совпадает, но сами точки различаются\n");
        }
    }
    else {
        printf("Количество найденных точек различается\n");
    }

    // Сохраняем результат диапазонного поиска в отдельный CSV-файл.
    output_filename = make_range_output_filename(input_filename);
    if (output_filename == NULL) {
        printf("Не удалось сформировать имя выходного файла для диапазонного поиска\n");
        free(kd_result);
        free(brute_result);
        return 1;
    }

    if (save_points_csv(output_filename, kd_result, kd_count)) {
        printf("Результат диапазонного поиска сохранён в %s\n", output_filename);
    }
    else {
        printf("Не удалось сохранить результат диапазонного поиска в CSV\n");
    }

    free(output_filename);
    free(kd_result);
    free(brute_result);
    return 0;
}

// Обрабатывает запуск DBSCAN и сохранение результата кластеризации.
int handle_dbscan(const char *input_filename, PointArray *data, int argc, char *argv[]) {
    double eps;
    int min_pts;
    DBSCANResult result;
    clock_t dbscan_start, dbscan_end;
    double dbscan_time;
    char *output_filename;

    if (argc < 5) {
        printf("Для -dbscan нужно передать eps и min_pts, например: 0.5 5\n");
        return 1;
    }

    if (!parse_positive_double(argv[3], &eps)) {
        printf("Неверный параметр eps: нужно положительное число\n");
        return 1;
    }

    if (!parse_positive_int(argv[4], &min_pts)) {
        printf("Неверный параметр min_pts: нужно положительное целое число\n");
        return 1;
    }

    // Замеряем время полной кластеризации.
    dbscan_start = clock();
    result = dbscan(data->points, data->count, eps, min_pts);
    dbscan_end = clock();

    if (result.labels == NULL) {
        printf("Не удалось выполнить DBSCAN\n");
        return 1;
    }

    dbscan_time = ((double)(dbscan_end - dbscan_start)) / CLOCKS_PER_SEC;

    print_dbscan_summary(&result, data->count);
    printf("Время DBSCAN: %.6f сек.\n", dbscan_time);

    // Сохраняем размеченный результат кластеризации в CSV.
    output_filename = make_dbscan_output_filename(input_filename);
    if (output_filename == NULL) {
        printf("Не удалось сформировать имя выходного файла\n");
        free_dbscan_result(&result);
        return 0;
    }

    if (save_dbscan_result_csv(output_filename, data->points, data->count, &result)) {
        printf("Результат DBSCAN сохранён в %s\n", output_filename);
    }
    else {
        printf("Не удалось сохранить результат DBSCAN в CSV\n");
    }

    free(output_filename);
    free_dbscan_result(&result);
    return 0;
}