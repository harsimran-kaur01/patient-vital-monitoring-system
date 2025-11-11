@echo off
gcc -Iheaders sources/avl_tree.c sources/queue.c sources/vital_monitor.c sources/web_server.c sources/main.c -o patient_monitor.exe -lws2_32 -lwsock32
if %errorlevel% == 0 (
    echo Compilation successful!
) else (
    echo Compilation failed!
)
pause
