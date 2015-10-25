sudo -v
sudo umount fs_mount
sudo rm -rf fs_mount
mkdir -p fs_mount
sudo losetup -d /dev/loop0
sudo losetup /dev/loop0 disk.img
sudo mount -o umask=000 /dev/loop0 fs_mount/
