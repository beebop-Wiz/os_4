target remote :1234
symbol-file fs_mount/boot/init.exe
set disassembly-flavor intel

source os_4.img-gdb.py

b _start
c
