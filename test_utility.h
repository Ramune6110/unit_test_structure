#ifndef TEST_UTILITY_H
#define TEST_UTILITY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t num_cols;    /* 列数 */
    size_t num_rows;    /* データ行数 */
    char **col_names;   /* [num_cols] 変数名 */
    double **data;      /* [num_cols][num_rows] データ */
} CsvData;

/**
 * CSVファイルを読み込む。
 * ・1行目をカラム名として分割
 * ・2行目以降を数値として読み込み、data[col][row]に格納
 * @param filename パス
 * @return CsvData* (malloc済)、失敗時 NULL
 */
CsvData *load_csv(const char *filename);

/**
 * CsvDataのメモリをすべて解放する
 */
void free_csv(CsvData *csv);

/**
 * CSVファイルを書き出す。
 * @param filename   出力ファイルパス
 * @param header     "col1,col2,..." のヘッダー文字列
 * @param num_cols   列数
 * @param num_rows   行数
 * @param results    [num_cols][num_rows] の出力データ
 * @return 0:成功, 非0:エラー
 */
int write_csv(const char *filename,
              const char *header,
              size_t num_cols,
              size_t num_rows,
              double **results);

#ifdef __cplusplus
}
#endif

#endif // TEST_UTILITY_H
