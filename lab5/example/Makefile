# 定义变量
hdrs = ipc.h
c_src = control.c ipc.c
c_obj = control.o ipc.o
r_src = reader.c ipc.c
r_obj = reader.o ipc.o
w_src = writer.c ipc.c
w_obj = writer.o ipc.o
opts = -g -c

# 默认目标
all: control reader writer

# 构建control目标
control: $(c_obj)
    gcc $(c_obj) -o control

# 构建control.o目标
control.o: $(c_src) $(hdrs)
    gcc $(opts) $(c_src)

# 构建reader目标
reader: $(r_obj)
    gcc $(r_obj) -o reader

# 构建reader.o目标
reader.o: $(r_src) $(hdrs)
    gcc $(opts) $(r_src)

# 构建writer目标
writer: $(w_obj)
    gcc $(w_obj) -o writer

# 构建writer.o目标
writer.o: $(w_src) $(hdrs)
    gcc $(opts) $(w_src)

# 清理操作
clean:
    rm control reader writer *.o
