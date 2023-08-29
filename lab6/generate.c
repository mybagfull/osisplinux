#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"

#define INDEX_RECORD_SIZE 16
#define HEADER_SIZE 8

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s filename size_in_GB\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    long long size = atoll(argv[2]); // изменение типа переменной на long long
    long long recordsNum = (size * 1024LL * 1024LL * 1024LL) / INDEX_RECORD_SIZE; // изменение типа переменной на long long

    // изменение размера файла
    long long header_size = HEADER_SIZE + recordsNum * INDEX_RECORD_SIZE; // изменение типа переменной на long long
    struct index_hdr_s *header = (struct index_hdr_s *) malloc(header_size);
    if (header == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    header->recordsCount = recordsNum;
    srand(time(NULL)); // устанавливает в качестве базы текущее время

    for (long long i = 0; i < recordsNum; i++) { // изменение типа переменной на long long
        struct index_s *record = header->idx + i;
        double days_since_1900 = (double) (rand() % (365 * 123)) + 15020.0;
        double fraction_of_day = (double) rand() / RAND_MAX / 2.0;
        record->time = days_since_1900 + fraction_of_day;
        record->recordNumber = i;
    }

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Failed to open file %s\n", filename);
        return 1;
    }

    fwrite(header, header_size, 1, fp);
    fclose(fp);

    printf("Generated %lld index recordsCount in file %s\n", recordsNum, filename); // изменение типа переменной на long long
    return 0;
}

