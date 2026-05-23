# Пространственные структуры данных: k-d tree

## О проекте

`Пространственные структуры данных` — учебный проект на языке **C** по теме **пространственных структур данных**.

В основе проекта лежит **3D k-d tree** для работы с наборами точек из CSV.  
Проект ориентирован на задачи, близкие к обработке пространственных данных и робототехнике.

Реализованы:
- загрузка 3D-точек из CSV;
- построение 3D `KD-Tree`;
- вставка новой точки;
- поиск ближайшего соседа;
- диапазонный поиск;
- сравнение `KD-Tree` и `Brute force` по корректности и времени;
- кластеризация `DBSCAN`;
- сохранение результатов в отдельные CSV-файлы.

---

## Что уже реализовано

В проекте есть:

- загрузка точек из CSV в память;
- полноценное **3D KD-дерево**;
- режимы:
  - `-kd_insert`
  - `-kd_nearest`
  - `-kd_range`
  - `-dbscan`
- проверка корректности `nearest` и `range` через **brute force**;
- сравнение времени работы `KD-Tree` и `Brute force`;
- `DBSCAN` для 3D-точек;
- ускорение `DBSCAN` через **пространственную сетку (`grid index`)**;
- сохранение результатов `range query`, `DBSCAN` и вставки в отдельные CSV-файлы;
- 2D-визуализация исходных точек, ближайшего соседа, диапазонного поиска и результата `DBSCAN` через Python-скрипты.

---

## Структура проекта

```text
C_Project_Muz/
├── include/
│   ├── bruteforce_utils.h
│   ├── cli_handlers.h
│   ├── dbscan.h
│   ├── grid_index.h
│   ├── io_utils.h
│   └── kd_tree.h
├── src/
│   ├── bruteforce_utils.c
│   ├── cli_handlers.c
│   ├── dbscan.c
│   ├── grid_index.c
│   ├── io_utils.c
│   ├── kd_tree.c
│   └── main.c
├── scripts/
│   ├── plot_dbscan_2D.py
│   ├── plot_nearest_2D.py
│   ├── plot_points_2D.py
│   └── plot_range_2D.py
├── Tests/
│   ├── Circle_xyz.csv
│   ├── Circle_clean_xyz.csv
│   ├── Sphere_xyz.csv
│   ├── Spiral_xyz.csv
│   ├── Two_clusters_noise_xyz.csv
│   └── Room_sample_xyz.csv
├── README.md
└── .gitignore
```

---

## Архитектура

### `src/main.c`

Главный файл программы.

Отвечает за:
- проверку аргументов командной строки;
- загрузку массива точек;
- построение `KD-Tree`;
- выбор нужного CLI-режима;
- освобождение памяти перед завершением программы.

### `src/kd_tree.c` / `include/kd_tree.h`

Модуль 3D `KD-Tree`.

Реализованы:
- `insert`
- `nearest_neighbor`
- `range_query`
- `range_query_indices`
- `free_tree`

Узел дерева хранит:
- точку `Point`;
- индекс точки в исходном массиве.

### `src/dbscan.c` / `include/dbscan.h`

Модуль `DBSCAN`.

Алгоритм работает **по массиву точек**, без зависимости от `KD-Tree`.

Реализованы:
- `dbscan(...)`
- `free_dbscan_result(...)`
- `print_dbscan_summary(...)`
- `save_dbscan_result_csv(...)`

### `src/grid_index.c` / `include/grid_index.h`

Модуль пространственной сетки.

Используется для ускорения поиска соседей внутри `DBSCAN`.

### `src/io_utils.c` / `include/io_utils.h`

Модуль ввода-вывода и разбора точек.

Содержит:
- загрузку точек из CSV;
- разбор точки из CLI;
- генерацию имён выходных файлов;
- сохранение точек в CSV.

### `src/bruteforce_utils.c` / `include/bruteforce_utils.h`

Модуль эталонных brute force-проверок.

Содержит:
- brute force nearest neighbor;
- brute force range query;
- сортировку точек;
- сравнение результатов `KD-Tree` и `Brute force`.

### `src/cli_handlers.c` / `include/cli_handlers.h`

Модуль обработчиков CLI-команд.

Содержит обработку режимов:
- `-kd_insert`
- `-kd_nearest`
- `-kd_range`
- `-dbscan`

Здесь находится логика пользовательских команд, вывода результатов и сохранения выходных CSV-файлов.

---

## Сборка проекта

Для сборки используется `Makefile`.

```bash
make
```

После успешной сборки появится исполняемый файл:

```bash
./robot_spatial
```

Для очистки объектных файлов и исполняемого файла используется команда:

```bash
make clean
```

---

## Формат входных данных

Основной ожидаемый формат — CSV, где каждая строка содержит одну 3D-точку:

```text
x,y,z
```

Пример:

```text
0,0,0
1,2,3
-1.5,4.2,0.8
```

### Важно

Если исходный CSV содержит дополнительные столбцы, его нужно предварительно преобразовать к формату `x,y,z`.

В проекте используются два типа тестовых данных:

- реальные или подготовленные облака точек;
- синтетические тестовые наборы, созданные для наглядной проверки отдельных случаев.

Синтетические файлы не заменяют реальные данные, а используются как контролируемые примеры: круг, сфера, кластеры с шумом и спираль.

---

## Поддерживаемые режимы

Для аргументов командной строки точки можно передавать как в формате `x,y,z`,
так и в формате `x,y`. Во втором случае координата `z` автоматически считается равной `0`.

### 1. Вставка новой точки

```bash
./robot_spatial <file.csv> -kd_insert x,y,z
```

Пример:

```bash
./robot_spatial Tests/Circle_clean_xyz.csv -kd_insert 1234.5,2345.5,3456.5
```

Что делает:
- загружает точки из CSV;
- строит `KD-Tree`;
- добавляет новую точку в массив и дерево;
- сохраняет обновлённый набор точек в отдельный CSV-файл.

Примеры имён выходных файлов:

```text
Results/CSV/Circle_clean_xyz_inserted.csv
Results/CSV/Sphere_xyz_inserted.csv
Results/CSV/Two_clusters_noise_xyz_inserted.csv
```

Каталог `Results/CSV` создаётся автоматически при первом сохранении.
Если файл уже существует, создаётся новый с нумерацией.

---

### 2. Поиск ближайшего соседа

```bash
./robot_spatial <file.csv> -kd_nearest x,y,z
```

Пример:

```bash
./robot_spatial Tests/Sphere_xyz.csv -kd_nearest 5,0,0
```

Что делает:
- ищет ближайшую точку через `KD-Tree`;
- ищет ближайшую точку через `Brute force`;
- сравнивает результаты;
- измеряет время работы обоих способов.

---

### 3. Диапазонный поиск

```bash
./robot_spatial <file.csv> -kd_range x1,y1,z1 x2,y2,z2
```

Пример:

```bash
./robot_spatial Tests/Circle_clean_xyz.csv -kd_range 4.5,-0.8,-0.2 5.5,0.8,0.2
```

Что делает:
- выполняет диапазонный поиск через `KD-Tree`;
- выполняет такой же поиск через `Brute force`;
- сравнивает количество найденных точек;
- сортирует и сравнивает результаты;
- сохраняет найденные точки в отдельный CSV-файл.

Примеры имён выходных файлов:

```text
Results/CSV/Circle_clean_xyz_range.csv
Results/CSV/Sphere_xyz_range.csv
Results/CSV/Two_clusters_noise_xyz_range.csv
```

Если файл уже существует, создаётся новый с нумерацией.

---

### 4. DBSCAN

```bash
./robot_spatial <file.csv> -dbscan <eps> <min_pts>
```

Пример:

```bash
./robot_spatial Tests/Two_clusters_noise_xyz.csv -dbscan 0.8 5
```

Что делает:
- запускает `DBSCAN` для 3D-точек;
- ищет соседей через `grid index`;
- выводит число кластеров;
- выводит число шумовых точек;
- измеряет время работы;
- сохраняет результат в CSV.

Формат выходного CSV:

```text
x,y,z,cluster
```

Примеры имён выходных файлов:

```text
Results/CSV/Two_clusters_noise_xyz_dbscan.csv
Results/CSV/Sphere_xyz_dbscan.csv
Results/CSV/Spiral_xyz_dbscan.csv
```

Если такой файл уже существует, создаётся новый с нумерацией.

---

## Команды Makefile

В проекте добавлены команды для сборки, запуска проверок и генерации визуализаций.

### Основная сборка

```bash
make
```

### Очистка проекта

```bash
make clean
```

### Запуск основных режимов

```bash
make run-all
```

Команда запускает:

- поиск ближайшего соседа;
- диапазонный поиск;
- DBSCAN-кластеризацию.

### Сравнительные проверки

```bash
make benchmark
```

Команда запускает несколько проверок `KD-Tree` против `Brute force` для:

- nearest neighbor;
- range query.

Для каждого запуска программа выводит время работы и проверяет совпадение результатов.

### Генерация 2D-визуализаций

```bash
make visualize
```

Команда создаёт папку `Results/` и генерирует PNG-файлы с визуализациями.

### Полная проверка проекта

```bash
make check
```

Команда выполняет:

- очистку проекта;
- сборку;
- запуск основных режимов;
- сравнительные проверки;
- генерацию визуализаций;
- проверку ограничения 300 строк для файлов `src/*.c` и `include/*.h`.

---

## Проверенные сценарии

Основные тестовые CSV для проверки сценариев находятся в папке `Tests/`.

- `Tests/Circle_xyz.csv`
- `Tests/Circle_clean_xyz.csv`
- `Tests/Sphere_xyz.csv`
- `Tests/Spiral_xyz.csv`
- `Tests/Two_clusters_noise_xyz.csv`
- `Tests/Room_sample_xyz.csv`

Для этих файлов были проверены режимы:
- `-kd_nearest`
- `-kd_range`
- `-dbscan`
- `-kd_insert`

Программа принимает либо полный путь к CSV-файлу, либо короткое имя файла.
Если передано только имя без пути, программа сначала ищет файл по указанному имени,
а затем автоматически пробует открыть его из папки `Tests/`.
---

## Подтверждённые результаты

### `KD-Tree`: nearest neighbor

Для проверки ближайшего соседа использовались разные тестовые CSV-файлы.

```bash
./robot_spatial Tests/Circle_clean_xyz.csv -kd_nearest 5,0,0
./robot_spatial Tests/Sphere_xyz.csv -kd_nearest 5,0,0
./robot_spatial Tests/Two_clusters_noise_xyz.csv -kd_nearest -3,0,0
./robot_spatial Tests/Spiral_xyz.csv -kd_nearest 0.2,0,0
./robot_spatial Tests/Room_sample_xyz.csv -kd_nearest 1.17,1.02,0.96
```

Во всех случаях программа выводит результат `KD-Tree`, результат `Brute force`, сравнение корректности и время работы обоих способов.

### `KD-Tree`: range query

Для проверки диапазонного поиска используются следующие команды:

```bash
./robot_spatial Tests/Circle_clean_xyz.csv -kd_range 4.5,-0.8,-0.2 5.5,0.8,0.2
./robot_spatial Tests/Sphere_xyz.csv -kd_range -2,-2,-5.5 2,2,5.5
./robot_spatial Tests/Two_clusters_noise_xyz.csv -kd_range -4.5,-1.5,-1.5 -1.5,1.5,1.5
./robot_spatial Tests/Spiral_xyz.csv -kd_range -1.5,-1.5,-0.2 1.5,1.5,1.2
./robot_spatial Tests/Room_sample_xyz.csv -kd_range 1.1,0.8,0.9 2.0,1.2,1.3
```

Для каждого запуска программа сравнивает результат `KD-Tree` с `Brute force` и сохраняет найденные точки в `Results/CSV`.

### `DBSCAN`

Для проверки кластеризации используются следующие команды:

```bash
./robot_spatial Tests/Circle_clean_xyz.csv -dbscan 0.15 4
./robot_spatial Tests/Sphere_xyz.csv -dbscan 0.55 5
./robot_spatial Tests/Two_clusters_noise_xyz.csv -dbscan 0.7 8
./robot_spatial Tests/Spiral_xyz.csv -dbscan 0.28 5
./robot_spatial Tests/Room_sample_xyz.csv -dbscan 0.05 8
```

Проверяются простые формы, 3D-сфера, два кластера с шумом, сложная вытянутая форма и более тяжёлое облако точек.

### `KD-Tree`: insert

Для проверки вставки использовалась команда:

```bash
./robot_spatial Tests/Circle_xyz.csv -kd_insert 1234.5,2345.5,3456.5
```

После вставки количество точек увеличивается на 1, а обновлённый набор сохраняется в отдельный CSV-файл.

---

## 2D-визуализации

В проекте есть несколько Python-скриптов для построения 2D-визуализаций по осям X-Y.

### 1. Исходные точки

```bash
python3 scripts/plot_points_2D.py Tests/Circle_clean_xyz.csv
```

Пример результата:

```bash
Results/PNG/Circle_clean_xyz_points_2D.png
```

### 2. Ближайший сосед

```bash
python3 scripts/plot_nearest_2D.py Tests/Sphere_xyz.csv 5,0,0
```

Пример результата:

```bash
Results/PNG/Sphere_xyz_nearest_2D.png
```

### 3. Range query

```bash
python3 scripts/plot_range_2D.py Tests/Circle_clean_xyz.csv Results/CSV/Circle_clean_xyz_range.csv
```

Пример результата:

```bash
Results/PNG/Circle_clean_xyz_range_2D.png
```

### 4. DBSCAN

```bash
python3 scripts/plot_dbscan_2D.py Results/CSV/Two_clusters_noise_xyz_dbscan.csv
```

Пример результата:

```bash
Results/PNG/Two_clusters_noise_xyz_dbscan_2D.png
```

Каталог Results/PNG создаётся автоматически.
Если matplotlib не установлен, можно использовать виртуальное окружение:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install matplotlib
```

---

## Ограничения проекта

На текущем этапе есть несколько известных ограничений:

1. Основной входной формат — CSV вида `x,y,z`.
2. CSV с дополнительными столбцами нужно предварительно преобразовать.
3. Крупные внешние облака точек могут отсутствовать в репозитории, если они используются только локально.
4. На больших наборах данных `DBSCAN` работает заметно дольше, чем операции `nearest` и `range`.

---

## Что уже закрыто по обязательной части

На текущем этапе проект закрывает основные обязательные пункты по теме `k-d tree`:

- вставка в `KD-Tree`;
- поиск ближайшего соседа;
- диапазонный поиск;
- сравнение `KD-Tree` и `Brute force` по корректности для `nearest neighbor`;
- сравнение `KD-Tree` и `Brute force` по корректности для `range search`;
- сравнение `KD-Tree` и `Brute force` по времени;
- пространственные запросы для `DBSCAN`;
- сохранение результатов в CSV;
- 2D-визуализации исходных точек, результатов запросов, кластеров и шумовых точек.

---

## Пример полной проверки

Для проверки требований к проекту можно выполнить одну команду

```bash
make check
```

Эта команда выполняет:

- очистку проекта;
- сборку;
- запуск основных режимов;
- сравнительные проверки KD-Tree и Brute force;
- генерацию 2D-визуализаций;
- проверку ограничения 300 строк для файлов `src/*.c` и `include/*.h`.

Если команда завершилась без ошибок, все требования выполнены

---

## Автор

Степанов Илья Николаевич