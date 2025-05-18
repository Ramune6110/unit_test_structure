import os
import csv
import itertools

def generate_struct_patterns(
    output_file: str,
    period: float = 0.1,
    repeat: int = 5
):
    """
    フラグ、モード、レベルの組み合わせを生成し、
    各パターンを repeat 回ずつ繰り返しで出力。
    time は period [s] ごとに増加します。
    """
    # 出力ディレクトリの作成
    os.makedirs(os.path.dirname(output_file), exist_ok=True)

    # フィールドごとに取りうる値を列挙
    flags  = [0, 1]                   # ブール／フラグ
    modes  = [0, 1, 2, 3]             # 列挙型
    levels = ['LOW', 'MID', 'HIGH']   # 文字列ENUM

    # ヘッダー行
    header = ['time', 'flag', 'mode', 'level']
    # 全組み合わせを生成
    combinations = list(itertools.product(flags, modes, levels))

    # CSV書き出し
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(header)

        row_idx = 0
        for combo in combinations:
            for _ in range(repeat):
                t = round(row_idx * period, 3)  # 時刻
                writer.writerow([t, *combo])
                row_idx += 1

    total_rows = len(combinations) * repeat
    print(f'Generated {output_file}: {total_rows} rows '
          f'({len(combinations)} patterns × {repeat} repeats), '
          f'period = {period}s')


if __name__ == "__main__":
    generate_struct_patterns(
        output_file="test/struct_patterns.csv",
        period=0.1,
        repeat=5
    )
