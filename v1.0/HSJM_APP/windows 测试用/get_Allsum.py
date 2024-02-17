def calculate_checksum(filename):
    with open(filename, 'rb') as file:
        content = file.read()
        return sum(byte for byte in content)

if __name__ == "__main__":
    filename = "outfile.bin"
    checksum = calculate_checksum(filename)
    hex_checksum = "0x" + hex(checksum)[2:]  # 转换为16进制并添加0x前缀
    with open("s19_checksum.txt", "w") as text_file:
        text_file.write(f"Checksum of {filename}: {hex_checksum}")