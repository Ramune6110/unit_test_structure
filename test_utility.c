#define _GNU_SOURCE
#include "test_utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// /* ヘッダー行を読み込んでカラム名を分割 */
// static char **parse_header(char *line, size_t *out_cols) {
//     /* 改行削除 */
//     char *p = line;
//     while (*p && (*p=='\r' || *p=='\n')) ++p;
//     *p = '\0';

//     /* カンマ区切りで数える */
//     size_t cols = 1;
//     for (char *q = line; *q; ++q)
//         if (*q == ',') ++cols;

//     char **names = malloc(sizeof(char*) * cols);
//     size_t idx = 0;
//     char *tok = strtok(line, ",");
//     while (tok && idx < cols) {
//         names[idx++] = strdup(tok);
//         tok = strtok(NULL, ",");
//     }
//     *out_cols = cols;
//     return names;
// }

static char **parse_header(char *line, size_t *out_cols) {
    // --- ここだけ置き換え ---
    // 末尾の "\r" や "\n" をすべて '\0' に
    size_t trim = strcspn(line, "\r\n");
    line[trim] = '\0';
    // --------------------------

    // カンマ数を数える
    size_t cols = 1;
    for (char *q = line; *q; ++q)
        if (*q == ',') ++cols;

    // 残りは元の実装と同じで OK
    char **names = malloc(sizeof(char*) * cols);
    size_t idx = 0;
    char *tok = strtok(line, ",");
    while (tok && idx < cols) {
        // 必要に応じて両端の空白も trim するとさらに堅牢
        names[idx++] = strdup(tok);
        tok = strtok(NULL, ",");
    }
    *out_cols = cols;
    return names;
}


CsvData *load_csv(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t n;

    /* 1行目：ヘッダー */
    if ((n = getline(&line, &len, fp)) <= 0) {
        free(line);
        fclose(fp);
        return NULL;
    }
    size_t num_cols = 0;
    char *hdr = strdup(line);
    char **col_names = parse_header(hdr, &num_cols);
    free(hdr);

    /* データ行数をカウント */
    size_t num_rows = 0;
    while ((n = getline(&line, &len, fp)) > 0) {
        if (n > 1) ++num_rows;
    }
    /* データ格納用メモリ確保 */
    double **data = malloc(sizeof(double*) * num_cols);
    for (size_t c = 0; c < num_cols; ++c)
        data[c] = malloc(sizeof(double) * num_rows);

    /* ファイル位置をデータ先頭に戻す */
    fseek(fp, 0, SEEK_SET);
    /* ヘッダー読み飛ばし */
    getline(&line, &len, fp);

    /* 各行をパース */
    size_t row = 0;
    while ((n = getline(&line, &len, fp)) > 0) {
        if (n <= 1) continue;
        /* 改行削除 */
        char *end = line + n;
        while (end > line && (*(end-1)=='\r' || *(end-1)=='\n')) *(--end) = '\0';

        char *tok = strtok(line, ",");
        for (size_t c = 0; c < num_cols; ++c) {
            if (!tok) data[c][row] = 0.0;
            else        data[c][row] = atof(tok);
            tok = strtok(NULL, ",");
        }
        ++row;
    }

    free(line);
    fclose(fp);

    CsvData *csv = malloc(sizeof(CsvData));
    csv->num_cols  = num_cols;
    csv->num_rows  = num_rows;
    csv->col_names = col_names;
    csv->data      = data;
    return csv;
}

void free_csv(CsvData *csv) {
    if (!csv) return;
    for (size_t i = 0; i < csv->num_cols; ++i)
        free(csv->col_names[i]);
    free(csv->col_names);
    for (size_t i = 0; i < csv->num_cols; ++i)
        free(csv->data[i]);
    free(csv->data);
    free(csv);
}

int write_csv(const char *filename,
              const char *header,
              size_t num_cols,
              size_t num_rows,
              double **results) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return -1;
    /* ヘッダー */
    fprintf(fp, "%s\n", header);
    /* 各行 */
    for (size_t r = 0; r < num_rows; ++r) {
        for (size_t c = 0; c < num_cols; ++c) {
            if (c > 0) fputc(',', fp);
            fprintf(fp, "%g", results[c][r]);
        }
        fputc('\n', fp);
    }
    fclose(fp);
    return 0;
}
