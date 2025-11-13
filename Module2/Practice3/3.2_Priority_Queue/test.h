#pragma once

#include <stdio.h>

// ----------------- Тест 1 -----------------
const char* test1[] = {
    // Приоритеты 1–10, по 5 элементов каждый
    "insert 1_1 1","insert 1_2 1","insert 1_3 1","insert 1_4 1","insert 1_5 1",
    "insert 2_1 2","insert 2_2 2","insert 2_3 2","insert 2_4 2","insert 2_5 2",
    "insert 3_1 3","insert 3_2 3","insert 3_3 3","insert 3_4 3","insert 3_5 3",
    "insert 4_1 4","insert 4_2 4","insert 4_3 4","insert 4_4 4","insert 4_5 4",
    "insert 5_1 5","insert 5_2 5","insert 5_3 5","insert 5_4 5","insert 5_5 5",
    "insert 6_1 6","insert 6_2 6","insert 6_3 6","insert 6_4 6","insert 6_5 6",
    "insert 7_1 7","insert 7_2 7","insert 7_3 7","insert 7_4 7","insert 7_5 7",
    "insert 8_1 8","insert 8_2 8","insert 8_3 8","insert 8_4 8","insert 8_5 8",
    "insert 9_1 9","insert 9_2 9","insert 9_3 9","insert 9_4 9","insert 9_5 9",
    "insert 10_1 10","insert 10_2 10","insert 10_3 10","insert 10_4 10","insert 10_5 10",

    // Приоритеты 11–20, по 3 элемента каждый
    "insert 11_1 11","insert 11_2 11","insert 11_3 11",
    "insert 12_1 12","insert 12_2 12","insert 12_3 12",
    "insert 13_1 13","insert 13_2 13","insert 13_3 13",
    "insert 14_1 14","insert 14_2 14","insert 14_3 14",
    "insert 15_1 15","insert 15_2 15","insert 15_3 15",
    "insert 16_1 16","insert 16_2 16","insert 16_3 16",
    "insert 17_1 17","insert 17_2 17","insert 17_3 17",
    "insert 18_1 18","insert 18_2 18","insert 18_3 18",
    "insert 19_1 19","insert 19_2 19","insert 19_3 19",
    "insert 20_1 20","insert 20_2 20","insert 20_3 20",

    // Высокие приоритеты, по 2 элемента каждый
    "insert 50_1 50","insert 50_2 50",
    "insert 100_1 100","insert 100_2 100",
    "insert 150_1 150","insert 150_2 150",
    "insert 200_1 200","insert 200_2 200",
    "insert 250_1 250","insert 250_2 250",

    // Максимальный приоритет 255, по 5 элементов
    "insert 255_1 255","insert 255_2 255","insert 255_3 255","insert 255_4 255","insert 255_5 255",

    // Финальная команда
    "enqueue first"
};

const int test1_size = sizeof(test1) / sizeof(test1[0]);





// ----------------- Тест 2 -----------------
const char* test2[] = {
    // Приоритеты 1–10, по 5 элементов каждый
    "insert 1_1 1","insert 1_2 1","insert 1_3 1","insert 1_4 1","insert 1_5 1",
    "insert 2_1 2","insert 2_2 2","insert 2_3 2","insert 2_4 2","insert 2_5 2",
    "insert 3_1 3","insert 3_2 3","insert 3_3 3","insert 3_4 3","insert 3_5 3",
    "insert 4_1 4","insert 4_2 4","insert 4_3 4","insert 4_4 4","insert 4_5 4",
    "insert 5_1 5","insert 5_2 5","insert 5_3 5","insert 5_4 5","insert 5_5 5",
    "insert 6_1 6","insert 6_2 6","insert 6_3 6","insert 6_4 6","insert 6_5 6",
    "insert 7_1 7","insert 7_2 7","insert 7_3 7","insert 7_4 7","insert 7_5 7",
    "insert 8_1 8","insert 8_2 8","insert 8_3 8","insert 8_4 8","insert 8_5 8",
    "insert 9_1 9","insert 9_2 9","insert 9_3 9","insert 9_4 9","insert 9_5 9",
    "insert 10_1 10","insert 10_2 10","insert 10_3 10","insert 10_4 10","insert 10_5 10",

    // Приоритеты 11–20, по 5 элементов каждый
    "insert 11_1 11","insert 11_2 11","insert 11_3 11","insert 11_4 11","insert 11_5 11",
    "insert 12_1 12","insert 12_2 12","insert 12_3 12","insert 12_4 12","insert 12_5 12",
    "insert 13_1 13","insert 13_2 13","insert 13_3 13","insert 13_4 13","insert 13_5 13",
    "insert 14_1 14","insert 14_2 14","insert 14_3 14","insert 14_4 14","insert 14_5 14",
    "insert 15_1 15","insert 15_2 15","insert 15_3 15","insert 15_4 15","insert 15_5 15",
    "insert 16_1 16","insert 16_2 16","insert 16_3 16","insert 16_4 16","insert 16_5 16",
    "insert 17_1 17","insert 17_2 17","insert 17_3 17","insert 17_4 17","insert 17_5 17",
    "insert 18_1 18","insert 18_2 18","insert 18_3 18","insert 18_4 18","insert 18_5 18",
    "insert 19_1 19","insert 19_2 19","insert 19_3 19","insert 19_4 19","insert 19_5 19",
    "insert 20_1 20","insert 20_2 20","insert 20_3 20","insert 20_4 20","insert 20_5 20",

    // Высокие приоритеты
    "insert 50_1 50","insert 50_2 50","insert 50_3 50","insert 50_4 50","insert 50_5 50",
    "insert 100_1 100","insert 100_2 100","insert 100_3 100","insert 100_4 100","insert 100_5 100",
    "insert 150_1 150","insert 150_2 150","insert 150_3 150","insert 150_4 150","insert 150_5 150",
    "insert 200_1 200","insert 200_2 200","insert 200_3 200","insert 200_4 200","insert 200_5 200",
    "insert 250_1 250","insert 250_2 250","insert 250_3 250","insert 250_4 250","insert 250_5 250",

    // Максимальный приоритет
    "insert 255_1 255","insert 255_2 255","insert 255_3 255","insert 255_4 255","insert 255_5 255",

    // Финальная команда
    "enqueue 17"
};

const int test2_size = sizeof(test2) / sizeof(test2[0]);




// ----------------- Тест 3 -----------------
const char* test3[] = {
    // Приоритеты 1–10, по 5 элементов каждый
    "insert 1_1 1","insert 1_2 1","insert 1_3 1","insert 1_4 1","insert 1_5 1",
    "insert 2_1 2","insert 2_2 2","insert 2_3 2","insert 2_4 2","insert 2_5 2",
    "insert 3_1 3","insert 3_2 3","insert 3_3 3","insert 3_4 3","insert 3_5 3",
    "insert 4_1 4","insert 4_2 4","insert 4_3 4","insert 4_4 4","insert 4_5 4",
    "insert 5_1 5","insert 5_2 5","insert 5_3 5","insert 5_4 5","insert 5_5 5",
    "insert 6_1 6","insert 6_2 6","insert 6_3 6","insert 6_4 6","insert 6_5 6",
    "insert 7_1 7","insert 7_2 7","insert 7_3 7","insert 7_4 7","insert 7_5 7",
    "insert 8_1 8","insert 8_2 8","insert 8_3 8","insert 8_4 8","insert 8_5 8",
    "insert 9_1 9","insert 9_2 9","insert 9_3 9","insert 9_4 9","insert 9_5 9",
    "insert 10_1 10","insert 10_2 10","insert 10_3 10","insert 10_4 10","insert 10_5 10",

    // Средние приоритеты 11–20, по 5 элементов каждый
    "insert 11_1 11","insert 11_2 11","insert 11_3 11","insert 11_4 11","insert 11_5 11",
    "insert 12_1 12","insert 12_2 12","insert 12_3 12","insert 12_4 12","insert 12_5 12",
    "insert 13_1 13","insert 13_2 13","insert 13_3 13","insert 13_4 13","insert 13_5 13",
    "insert 14_1 14","insert 14_2 14","insert 14_3 14","insert 14_4 14","insert 14_5 14",
    "insert 15_1 15","insert 15_2 15","insert 15_3 15","insert 15_4 15","insert 15_5 15",
    "insert 16_1 16","insert 16_2 16","insert 16_3 16","insert 16_4 16","insert 16_5 16",
    "insert 17_1 17","insert 17_2 17","insert 17_3 17","insert 17_4 17","insert 17_5 17",
    "insert 18_1 18","insert 18_2 18","insert 18_3 18","insert 18_4 18","insert 18_5 18",
    "insert 19_1 19","insert 19_2 19","insert 19_3 19","insert 19_4 19","insert 19_5 19",
    "insert 20_1 20","insert 20_2 20","insert 20_3 20","insert 20_4 20","insert 20_5 20",

    // Высокие приоритеты
    "insert 50_1 50","insert 50_2 50","insert 50_3 50","insert 50_4 50","insert 50_5 50",
    "insert 100_1 100","insert 100_2 100","insert 100_3 100","insert 100_4 100","insert 100_5 100",
    "insert 150_1 150","insert 150_2 150","insert 150_3 150","insert 150_4 150","insert 150_5 150",
    "insert 200_1 200","insert 200_2 200","insert 200_3 200","insert 200_4 200","insert 200_5 200",
    "insert 250_1 250","insert 250_2 250","insert 250_3 250","insert 250_4 250","insert 250_5 250",

    // Максимальный приоритет
    "insert 255_1 255","insert 255_2 255","insert 255_3 255","insert 255_4 255","insert 255_5 255",

    // Финальная команда
    "enqueuenomin 20"
};

const int test3_size = sizeof(test3) / sizeof(test3[0]);


#undef INSERT_BLOCK
#undef INSERT_HIGH
