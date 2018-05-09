sudo fusermount -u fs_mount
sudo rm -rf fs_mount
sudo rm disk.img

/sbin/mkfs.ext2 -b 1024 -t ext2 disk.img 20M
sudo chown beebop disk.img

mkdir fs_mount

sudo fuse2fs -o allow_other disk.img fs_mount

sudo mkdir -p fs_mount/bin
sudo mkdir -p fs_mount/boot
sudo mkdir -p fs_mount/lib
sudo mkdir -p fs_mount/usr
sudo mkdir -p fs_mount/home
sudo mkdir -p fs_mount/root

sudo chown -R beebop fs_mount

sync

