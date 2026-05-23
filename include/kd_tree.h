// Базовые структуры и функции для работы с 3D KD-деревом.
// Используются для вставки точек, поиска ближайшего соседа и диапазонных запросов.
#ifndef KD_TREE_H
#define KD_TREE_H

// Точка в трёхмерном пространстве.
typedef struct Point {
    double x, y, z;  // Для 3D-данных
} Point;

// Динамический массив точек, загружаемых из CSV-файла.
typedef struct {
    Point *points;
    int count;
} PointArray;

// Узел KD-дерева: хранит координаты точки, её индекс в исходном массиве
// и ссылки на левое и правое поддерево.
typedef struct Node {
    Point point;
    int index;
    struct Node *left, *right;
} Node;

// Функции построения и обхода KD-дерева.
Node* insert(Node* root, Point point, int index, int depth);
// Поиск ближайшего соседа в 3D KD-дереве.
Point nearest_neighbor(Node* root, Point target, int depth);

// Поиск точек внутри 3D-диапазона.
void range_query(Node* root, Point lower, Point upper, int depth, Point* result, int* count);

void free_tree(Node* root);

#endif
