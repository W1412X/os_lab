#!/bin/bash
process_name="main"
pids=$(pgrep $process_name)
if [ -z "$pids" ]; then
    echo "没有找到要终止的进程：$process_name"
    exit 1
fi
for pid in $pids; do
    echo "终止进程：$pid"
    kill $pid
done

echo "进程终止完成"

