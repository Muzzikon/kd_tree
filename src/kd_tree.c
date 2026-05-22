// Реализация 3D KD-дерева.
// Здесь находятся вставка точек, поиск ближайшего соседа, диапазонный поиск и очистка памяти.
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "kd_tree.h"

static double get_coord(Point point, int axis) {
    if (axis == 0) {
        return point.x;
    }

    if (axis == 1) {
        return point.y;
    }

    return point.z;
}

// Рекурсивная вставка точки в KD-дерево.
// Ось сравнения зависит от глубины: x, y, z, затем снова x.
Node* insert(Node* root, Point point, int index, int depth) {
    int axis;

    if (root == NULL) {
        Node* new_node = (Node*)malloc(sizeof(Node));

        if (new_node == NULL) {
            fprintf(stderr, "Ошибка выделения памяти для узла KD-Tree\n");
            exit(EXIT_FAILURE);
        }

        new_node->point = point;
        new_node->index = index;
        new_node->left = new_node->right = NULL;

        return new_node;
    }

    // На каждой глубине выбираем одну из трёх координат для разбиения пространства.
    axis = depth % 3;

    if (get_coord(point, axis) < get_coord(root->point, axis)) {
        root->left = insert(root->left, point, index, depth + 1);
    }
    else {
        root->right = insert(root->right, point, index, depth + 1);
    }

    return root;
}

// Простой вывод всех точек дерева в глубинном обходе.
void print_tree(Node* root) {
    if (root == NULL) return;
    printf("Point: (%lf, %lf, %lf)\n", root->point.x, root->point.y, root->point.z);
    print_tree(root->left);
    print_tree(root->right);
}

// Квадрат расстояния между двумя точками.
// Используется вместо обычного расстояния, чтобы не вызывать sqrt.
static double distance_squared(Point a, Point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

static int point_in_range(Point point, Point lower, Point upper) {
    return point.x >= lower.x && point.x <= upper.x &&
           point.y >= lower.y && point.y <= upper.y &&
           point.z >= lower.z && point.z <= upper.z;
}

// Внутренняя рекурсивная функция поиска ближайшего соседа.
// Сначала идём в более перспективную ветку, затем при необходимости проверяем вторую.
static void nearest_neighbor_recursive(Node* root, Point target, int depth, Point* best_point, double* best_dist) {
    if (root == NULL) {
        return;
    }

    double current_dist = distance_squared(root->point, target);
    if (current_dist < *best_dist) {
        *best_dist = current_dist;
        *best_point = root->point;
    }

    int axis = depth % 3;
    Node* first_branch;
    Node* second_branch;
    double axis_dist = get_coord(target, axis) - get_coord(root->point, axis);

    // Определяем, какое поддерево просматривать первым по текущей оси.
    if (axis_dist < 0.0) {
        first_branch = root->left;
        second_branch = root->right;
    }
    else {
        first_branch = root->right;
        second_branch = root->left;
    }

    nearest_neighbor_recursive(first_branch, target, depth + 1, best_point, best_dist);

    // Проверяем вторую ветвь только если гиперплоскость может содержать более близкую точку.
    if (axis_dist * axis_dist < *best_dist) {
        nearest_neighbor_recursive(second_branch, target, depth + 1, best_point, best_dist);
    }
}

// Внешняя функция поиска ближайшего соседа.
Point nearest_neighbor(Node* root, Point target, int depth) {
    Point best_point = {DBL_MAX, DBL_MAX, DBL_MAX};
    double best_dist = DBL_MAX;

    if (root == NULL) return best_point;

    // Рекурсивный поиск ближайшего соседа
    nearest_neighbor_recursive(root, target, depth, &best_point, &best_dist);

    return best_point;
}

// Поиск всех точек, попадающих в прямоугольный 3D-диапазон.
void range_query(Node* root, Point lower, Point upper, int depth, Point* result, int* count) {
    if (root == NULL) return;

    int cd = depth % 3;  // Чередуем оси

    // Проверяем, входит ли текущая точка в диапазон
    if (point_in_range(root->point, lower, upper)) {
    result[*count] = root->point;
    (*count)++;
    }

    // Рекурсивно ищем в нужных поддеревьях
    if (get_coord(lower, cd) <= get_coord(root->point, cd)) {
        range_query(root->left, lower, upper, depth + 1, result, count);
    }

    if (get_coord(upper, cd) >= get_coord(root->point, cd)) {
        range_query(root->right, lower, upper, depth + 1, result, count);
    }
}

// Диапазонный поиск, который возвращает индексы точек вместо самих координат.
void range_query_indices(Node* root, Point lower, Point upper, int depth, int* result, int* count) {
    if (root == NULL) return;

    int cd = depth % 3;

    if (point_in_range(root->point, lower, upper)) {
    result[*count] = root->index;
    (*count)++;
    }

    if (get_coord(lower, cd) <= get_coord(root->point, cd)) {
        range_query_indices(root->left, lower, upper, depth + 1, result, count);
    }

    if (get_coord(upper, cd) >= get_coord(root->point, cd)) {
        range_query_indices(root->right, lower, upper, depth + 1, result, count);
    }

}

// Полное освобождение памяти KD-дерева.
void free_tree(Node* root) {
    if (root == NULL) {
        return;
    }

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
