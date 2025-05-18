import os
import csv
import matplotlib.pyplot as plt

def main():
    # ファイルパス
    input_path  = 'test/test_input.csv'
    output_path = 'test/test_output.csv'

    # ファイル存在チェック
    if not os.path.exists(input_path):
        print(f"Error: Input file '{input_path}' not found.")
        return
    if not os.path.exists(output_path):
        print(f"Error: Output file '{output_path}' not found.")
        return

    # --- 入力CSV読み込み ---
    with open(input_path, newline='') as f:
        reader = csv.reader(f)
        header_in = next(reader)
        time_in = []
        data_in = {h: [] for h in header_in[1:]}
        for row in reader:
            time_in.append(float(row[0]))
            for i, col in enumerate(header_in[1:], start=1):
                data_in[col].append(float(row[i]))

    # --- 出力CSV読み込み ---
    with open(output_path, newline='') as f:
        reader = csv.reader(f)
        header_out = next(reader)
        time_out = []
        data_out = {h: [] for h in header_out[1:]}
        for row in reader:
            time_out.append(float(row[0]))
            for i, col in enumerate(header_out[1:], start=1):
                data_out[col].append(float(row[i]))

    # プロット準備
    total_vars = list(data_in.keys()) + list(data_out.keys())
    total = len(total_vars)

    fig, axes = plt.subplots(total, 1, figsize=(10, 3 * total), sharex=False)
    if total == 1:
        axes = [axes]

    # 入力変数のステッププロット（青）
    for idx, var in enumerate(data_in):
        ax = axes[idx]
        ax.step(time_in, data_in[var], where='post', label=var, color='blue')
        ax.scatter(time_in, data_in[var], marker='o', color='blue')
        ax.set_ylabel(var)
        ax.grid(True)

    # 出力変数のステッププロット（赤）
    offset = len(data_in)
    for jdx, var in enumerate(data_out):
        ax = axes[offset + jdx]
        ax.step(time_out, data_out[var], where='post', label=var, color='red')
        ax.scatter(time_out, data_out[var], marker='o', color='red')
        ax.set_ylabel(var)
        ax.grid(True)

    # X軸ラベル
    axes[-1].set_xlabel('Time')

    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    main()
