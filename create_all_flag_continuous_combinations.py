import os
import csv
import itertools

def generate_mixed_test_patterns(
    output_file: str,
    period: float = 0.1,
    continuous_vars: dict = None,
    discrete_vars: dict = None
):
    """
    連続値（境界値＋代表値）とフラグ／列挙型の全組み合わせを生成し、
    時系列データ付きのCSVを出力します。
    
    Parameters:
    - output_file: 出力CSVファイルパス
    - period:       各パターン間の時間間隔 [s]
    - continuous_vars: {
          'x': [xmin, xmax, xrep],
          'y': [ymin, ymax, yrep],
          ...
      }
    - discrete_vars: {
          'flag': [0, 1],
          'mode': [0,1,2,3],
          ...
      }
    """
    # 出力ディレクトリを確保
    os.makedirs(os.path.dirname(output_file), exist_ok=True)

    # デフォルトの連続・離散変数パターン
    if continuous_vars is None:
        continuous_vars = {
            'x': [0.1, 0.3, 0.2],
            'y': [0.1, 0.3, 0.2],
            'z': [0.1, 0.3, 0.2]
        }
    if discrete_vars is None:
        discrete_vars = {
            'flag': [0, 1],
            'mode': [0, 1, 2, 3]
        }

    # ヘッダー行作成
    headers = ['time'] + list(continuous_vars.keys()) + list(discrete_vars.keys())

    # すべての組み合わせを生成
    value_lists = list(continuous_vars.values()) + list(discrete_vars.values())
    combos = itertools.product(*value_lists)

    # CSV書き込み
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        for idx, combo in enumerate(combos):
            t = round(idx * period, 3)
            writer.writerow([t] + list(combo))

    total = len(list(itertools.product(*(value_lists))))
    print(f"Generated {output_file}: {total} patterns, period = {period}s")


if __name__ == "__main__":
    generate_mixed_test_patterns(
        output_file="test/mixed_test_patterns.csv",
        period=0.1,
        continuous_vars={
            'x': [0.1, 0.3, 0.2],
            'y': [0.1, 0.3, 0.2],
            'z': [0.1, 0.3, 0.2]
        },
        discrete_vars={
            'flag': [0, 1],
            'mode': [0, 1, 2, 3]
        }
    )
