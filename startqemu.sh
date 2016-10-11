qemu-system-i386 -drive 'file=os_4.img,index=0,media=disk' -drive 'file=disk.img,index=1,cache=writethrough,media=disk' -serial stdio $@
