import os
import csv
import itertools

def generate_xyz_repeated_patterns(
    output_file: str,
    period: float = 0.1,
    repeat: int = 5
):
    """
    x, y, z のテストパターンを生成し、各パターンを repeat 回ずつ繰り返します。
    - period: 時間間隔 [s]
    - repeat: 各パターンを何周期分繰り返すか
    """
    # 出力ディレクトリ確保
    os.makedirs(os.path.dirname(output_file), exist_ok=True)

    # x, y, z の各値リスト（例として境界 0.1, 0.3 と代表 0.2）
    x_vals = [0.1, 0.3, 0.2]
    y_vals = [0.1, 0.3, 0.2]
    z_vals = [0.1, 0.3, 0.2]

    # 全組み合わせ生成
    combos = list(itertools.product(x_vals, y_vals, z_vals))
    # ヘッダー
    header = ['time', 'x', 'y', 'z']

    # CSV書き出し
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(header)
        row_idx = 0
        for combo in combos:
            x, y, z = combo
            # 各パターンを repeat 回ずつ繰り返し
            for _ in range(repeat):
                t = round(row_idx * period, 3)
                writer.writerow([t, x, y, z])
                row_idx += 1

    total_rows = len(combos) * repeat
    print(f"Generated {output_file}: {total_rows} rows "
          f"({len(combos)} patterns × {repeat} repeats), period={period}s")

if __name__ == "__main__":
    generate_xyz_repeated_patterns(
        output_file="test/test_pattern_repeat5.csv",
        period=0.1,
        repeat=5
    )
