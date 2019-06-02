#!/sbin/sh
cd /tmp/dtimage/
rm -rf out
mkdir out
dd if=/dev/block/bootdevice/by-name/boot of=/tmp/dtimage/boot.img
./unpackbootimg -i boot.img -o out
