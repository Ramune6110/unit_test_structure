#ifndef TEST_UTILITY_H
#define TEST_UTILITY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CSVデータ構造 */
typedef struct {
    size_t num_cols;   /* 列数 */
    size_t num_rows;   /* データ行数 */
    char   **col_names;/* [num_cols] 変数名 */
    double **data;     /* [num_cols][num_rows] の数値データ */
} CsvData;

/* CSV読み込み／解放／書き出し */
CsvData *load_csv(const char *filename);
void     free_csv(CsvData *csv);
int      write_csv(const char *filename,
                   const char *header,
                   size_t      num_cols,
                   size_t      num_rows,
                   double    **results);

/* 列名→インデックス検索 */
int      find_columns(const CsvData *csv,
                      const char *names[], size_t n_names,
                      int         indices[]);

/* 結果配列の確保・解放 */
double **alloc_results(size_t num_cols, size_t num_rows);
void     free_results(double **results, size_t num_cols);

/* 行ごとの演算をコールバックで受け取る型 */
typedef void (*RowFunc)(const double in[], double out[], size_t row);

/**
 * 汎用 CSV→演算→CSV パイプライン
 *  ・in_cols: 入力列名配列（長さ num_in）
 *  ・out_cols: 出力列名配列（長さ num_out）
 *  ・fn: 各行ごとに in[]→out[] を計算するコールバック
 */
int process_csv(const char *infile,
                const char *outfile,
                const char *in_cols[],  size_t num_in,
                const char *out_cols[], size_t num_out,
                RowFunc      fn);

#ifdef __cplusplus
}
#endif

#endif // TEST_UTILITY_H
