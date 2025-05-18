#define _GNU_SOURCE
#include "test_utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- ヘッダー行のパース ---------- */
static char **parse_header(char *line, size_t *out_cols)
{
    /* 末尾の改行除去 */
    size_t trim = strcspn(line, "\r\n");
    line[trim] = '\0';

    /* カンマ数から列数を推定 */
    size_t cols = 1;
    for (char *p = line; *p; ++p)
        if (*p == ',') ++cols;

    /* 名前配列を確保して strdup */
    char **names = malloc(sizeof(char*) * cols);
    if (!names) return NULL;

    size_t idx = 0;
    char *tok = strtok(line, ",");
    while (tok && idx < cols) {
        names[idx++] = strdup(tok);
        tok = strtok(NULL, ",");
    }
    *out_cols = cols;
    return names;
}

/* ---------- CSV 読み込み ---------- */
CsvData *load_csv(const char *filename)
{
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
    char *hdr = strdup(line);
    size_t num_cols = 0;
    char **col_names = parse_header(hdr, &num_cols);
    free(hdr);
    if (!col_names) {
        free(line);
        fclose(fp);
        return NULL;
    }

    /* データ行数を先にカウント */
    size_t num_rows = 0;
    while ((n = getline(&line, &len, fp)) > 0)
        if (n > 1) ++num_rows;

    /* メモリ確保 */
    double **data = malloc(sizeof(double*) * num_cols);
    if (!data) { free(line); fclose(fp); return NULL; }
    for (size_t c = 0; c < num_cols; ++c) {
        data[c] = malloc(sizeof(double) * num_rows);
        if (!data[c]) {
            /* 途中で失敗したら巻き戻し */
            for (size_t k = 0; k < c; ++k) free(data[k]);
            free(data);
            free(line);
            fclose(fp);
            return NULL;
        }
    }

    /* 再度ファイル先頭へ */
    fseek(fp, 0, SEEK_SET);
    // getline(&line, &len, fp);  /* ヘッダー読み飛ばし */
    {
        ssize_t ret = getline(&line, &len, fp);
        if (ret <= 0) {
            /* 何も読めなかった or エラー */
            free(line);
            fclose(fp);
            return NULL;
        }
    }

    /* 各行をパース */
    size_t row = 0;
    while ((n = getline(&line, &len, fp)) > 0) {
        if (n <= 1) continue;
        /* 改行除去 */
        size_t t = strcspn(line, "\r\n");
        line[t] = '\0';

        char *tok = strtok(line, ",");
        for (size_t c = 0; c < num_cols; ++c) {
            data[c][row] = tok ? atof(tok) : 0.0;
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

/* ---------- CSV 解放 ---------- */
void free_csv(CsvData *csv)
{
    if (!csv) return;
    for (size_t i = 0; i < csv->num_cols; ++i)
        free(csv->col_names[i]);
    free(csv->col_names);
    for (size_t i = 0; i < csv->num_cols; ++i)
        free(csv->data[i]);
    free(csv->data);
    free(csv);
}

/* ---------- CSV 書き出し ---------- */
int write_csv(const char *filename,
              const char *header,
              size_t      num_cols,
              size_t      num_rows,
              double    **results)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) return -1;
    fprintf(fp, "%s\n", header);
    for (size_t r = 0; r < num_rows; ++r) {
        for (size_t c = 0; c < num_cols; ++c) {
            if (c) fputc(',', fp);
            fprintf(fp, "%g", results[c][r]);
        }
        fputc('\n', fp);
    }
    fclose(fp);
    return 0;
}

/* ---------- find_columns ---------- */
int find_columns(const CsvData *csv,
                 const char *names[], size_t n_names,
                 int         indices[])
{
    for (size_t i = 0; i < n_names; ++i) {
        indices[i] = -1;
        for (size_t c = 0; c < csv->num_cols; ++c) {
            if (strcmp(names[i], csv->col_names[c]) == 0) {
                indices[i] = (int)c;
                break;
            }
        }
        if (indices[i] < 0)
            return -1;  /* 見つからなかった */
    }
    return 0;
}

/* ---------- alloc_results / free_results ---------- */
double **alloc_results(size_t num_cols, size_t num_rows)
{
    double **res = malloc(sizeof(double*) * num_cols);
    if (!res) return NULL;
    for (size_t c = 0; c < num_cols; ++c) {
        res[c] = malloc(sizeof(double) * num_rows);
        if (!res[c]) {
            for (size_t k = 0; k < c; ++k) free(res[k]);
            free(res);
            return NULL;
        }
    }
    return res;
}

void free_results(double **results, size_t num_cols)
{
    if (!results) return;
    for (size_t c = 0; c < num_cols; ++c)
        free(results[c]);
    free(results);
}

/* ---------- process_csv パイプライン ---------- */
int process_csv(const char *infile,
                const char *outfile,
                const char *in_cols[],  size_t num_in,
                const char *out_cols[], size_t num_out,
                RowFunc      fn)
{
    CsvData *csv = load_csv(infile);
    if (!csv) return -1;

    /* 入力列インデックス取得 */
    int *in_idx = malloc(sizeof(int) * num_in);
    if (!in_idx) { free_csv(csv); return -1; }
    if (find_columns(csv, in_cols, num_in, in_idx) != 0) {
        free(in_idx);
        free_csv(csv);
        return -1;
    }

    /* 出力配列確保 */
    double **results = alloc_results(num_out, csv->num_rows);
    if (!results) {
        free(in_idx);
        free_csv(csv);
        return -1;
    }

    /* 各行ごとの演算呼び出し */
    double *in_vals  = malloc(sizeof(double) * num_in);
    double *out_vals = malloc(sizeof(double) * num_out);
    if (!in_vals || !out_vals) {
        free(in_idx);
        free_results(results, num_out);
        free_csv(csv);
        free(in_vals); free(out_vals);
        return -1;
    }

    for (size_t r = 0; r < csv->num_rows; ++r) {
        for (size_t i = 0; i < num_in; ++i)
            in_vals[i] = csv->data[in_idx[i]][r];
        fn(in_vals, out_vals, r);
        for (size_t j = 0; j < num_out; ++j)
            results[j][r] = out_vals[j];
    }

    /* ヘッダ文字列を合成 */
    size_t hdr_len = 1;
    for (size_t i = 0; i < num_out; ++i)
        hdr_len += strlen(out_cols[i]) + 1;
    char *header = malloc(hdr_len);
    if (!header) {
        free(in_idx); free(in_vals); free(out_vals);
        free_results(results, num_out);
        free_csv(csv);
        return -1;
    }
    header[0] = '\0';
    for (size_t i = 0; i < num_out; ++i) {
        strcat(header, out_cols[i]);
        if (i + 1 < num_out) strcat(header, ",");
    }

    /* 書き出し */
    int err = write_csv(outfile, header, num_out, csv->num_rows, results);

    /* 後片付け */
    free(header);
    free(in_idx);
    free(in_vals);
    free(out_vals);
    free_results(results, num_out);
    free_csv(csv);
    return err;
}