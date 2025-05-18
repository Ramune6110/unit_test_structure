#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_utility.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.csv output.csv\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *infile  = argv[1];
    const char *outfile = argv[2];

    CsvData *csv = load_csv(infile);
    if (!csv) {
        fprintf(stderr, "Error: Failed to load '%s'\n", infile);
        return EXIT_FAILURE;
    }

    /* 列名から index を探す */
    int idx_time=-1, idx_x=-1, idx_y=-1, idx_z=-1;
    for (size_t i = 0; i < csv->num_cols; ++i) {
        if (strcmp(csv->col_names[i], "time")==0) idx_time = i;
        if (strcmp(csv->col_names[i], "x")==0)    idx_x    = i;
        if (strcmp(csv->col_names[i], "y")==0)    idx_y    = i;
        if (strcmp(csv->col_names[i], "z")==0)    idx_z    = i;
    }
    if (idx_time<0 || idx_x<0 || idx_y<0 || idx_z<0) {
        fprintf(stderr, "Error: required columns (time,x,y,z) not found\n");
        free_csv(csv);
        return EXIT_FAILURE;
    }

    /* 出力データ：time と (x+y+z) の２列 */
    size_t out_cols = 2;
    double **results = malloc(sizeof(double*) * out_cols);
    for (size_t c = 0; c < out_cols; ++c)
        results[c] = malloc(sizeof(double) * csv->num_rows);

    for (size_t r = 0; r < csv->num_rows; ++r) {
        double t = csv->data[idx_time][r];
        double x = csv->data[idx_x   ][r];
        double y = csv->data[idx_y   ][r];
        double z = csv->data[idx_z   ][r];
        results[0][r] = t;
        results[1][r] = x + y + z;
    }

    /* ファイル書き出し */
    if (write_csv(outfile, "time,sum", out_cols, csv->num_rows, results) != 0) {
        fprintf(stderr, "Error: Failed to write '%s'\n", outfile);
    }

    /* 解放 */
    for (size_t c = 0; c < out_cols; ++c)
        free(results[c]);
    free(results);
    free_csv(csv);
    return EXIT_SUCCESS;
}
