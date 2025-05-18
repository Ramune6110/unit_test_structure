#include <stdio.h>
#include <stdlib.h>
#include "test_utility.h"

/*――――――――――――――――――――――――――――――――――――
  グローバル変数定義（main.c 内）
――――――――――――――――――――――――――――――――――――*/
static double g_t, g_x, g_y, g_z;
static double g_sum, g_diff;

/*――――――――――――――――――――――――――――――――――――
  小さな演算を別関数で定義（引数は必要な値のみ）
――――――――――――――――――――――――――――――――――――*/
static void compute_sum(double t, double x, double y, double z) {
    (void)t;  // time は加工せずそのまま出力
    g_sum = x + y + z;
}

static void compute_diff(double x, double y) {
    g_diff = x - y;
}

/*――――――――――――――――――――――――――――――――――――
  上記をまとめて呼び出す「合成関数」
  RowFunc シグネチャに合わせる必要あり
――――――――――――――――――――――――――――――――――――*/
static void composite_fn(const double in[], double out[], size_t row) {
    (void)row;  // 行番号は使わない

    /* globals に格納 */
    g_t = in[0];
    g_x = in[1];
    g_y = in[2];
    g_z = in[3];

    /* 演算を実行 */
    compute_sum(g_t, g_x, g_y, g_z);
    compute_diff(g_x, g_y);

    /* 出力配列にセット */
    out[0] = g_t;
    out[1] = g_sum;
    out[2] = g_diff;
}

/**
 *******************************************************************************************
 * MAIN Method
 *******************************************************************************************
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.csv output.csv\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *infile  = argv[1];
    const char *outfile = argv[2];

    /*――――――――――――――――――――――――――――――――――――
      列名定義（変更時はここを調整）
    ――――――――――――――――――――――――――――――――――――*/
    static const char *in_cols[]  = { "time", "x", "y", "z" };
    static const char *out_cols[] = { "time", "sum", "diff" };
    const size_t num_in  = sizeof(in_cols)  / sizeof(in_cols[0]);
    const size_t num_out = sizeof(out_cols) / sizeof(out_cols[0]);

    /* 汎用パイプライン呼び出し */
    int err = process_csv(
        infile, outfile,
        in_cols,  num_in,
        out_cols, num_out,
        composite_fn
    );
    if (err) {
        fprintf(stderr,
            "Error: CSV processing failed (%s → %s)\n",
            infile, outfile
        );
        return EXIT_FAILURE;
    }

    printf("Success: '%s' → '%s' を処理しました\n",
           infile, outfile);
    return EXIT_SUCCESS;
}
