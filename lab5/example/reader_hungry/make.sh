#!/bin/bash

# 编译控制器
gcc -g -c control.c ipc.c
gcc control.o ipc.o -o control

# 编译读取器
gcc -g -c reader.c ipc.c
gcc reader.o ipc.o -o reader

# 编译写入器
gcc -g -c writer.c ipc.c
gcc writer.o ipc.o -o writer

# 清理中间文件
rm *.o
