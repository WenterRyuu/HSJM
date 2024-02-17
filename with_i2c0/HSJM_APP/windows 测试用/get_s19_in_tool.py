import os
import subprocess

def process_s19_line(line):
    if line.startswith("S3"):
        prefix = "53 03"
        rest_of_line = line[2:]
    elif line.startswith("S7"):
        prefix = "53 07"
        rest_of_line = line[2:]
    else:
        return None

    split_line = ' '.join(rest_of_line[i:i+2] for i in range(0, len(rest_of_line), 2))
    return prefix + " " + split_line

def process_s19_file(input_file_path, output_file_path):
    with open(input_file_path, 'r') as file, open(output_file_path, 'w') as output_file:
        for line in file:
            processed_line = process_s19_line(line.strip())
            if processed_line:
                output_file.write(processed_line + "\n")

# 输入和输出文件路径
input_file_path = "outfile.s19"
output_file_path = "outfile_from_s19.txt"

# 处理文件并保存结果
process_s19_file(input_file_path, output_file_path)

# 打开输出文件
if os.name == 'nt':  # for Windows
    os.startfile(output_file_path)
elif os.name == 'posix':  # for macOS and Linux
    subprocess.call(['open', output_file_path])

print(f"Output saved to {output_file_path}")
