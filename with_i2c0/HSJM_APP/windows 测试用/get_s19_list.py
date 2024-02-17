# 打开原始文件和目标文件
with open('outfile_from_s19.txt', 'r') as infile, open('outfile_to_list.txt', 'w') as outfile:
    # 读取原始文件的每一行
    for line in infile:
        # 移除行尾的换行符
        line = line.strip()

        # 写入目标文件，首先写入'写数据'行，然后写入'读数据'行
        outfile.write(f"1,写数据,0x1A,{line},11,100\n")
        outfile.write("1,读数据,0x1A,,11,10\n")