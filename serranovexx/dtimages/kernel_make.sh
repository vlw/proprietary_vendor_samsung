#!/sbin/sh
cd /tmp/dtimage/

kernel=./out/boot.img-zImage
rd=./out/boot.img-ramdisk.gz
cmdline=`cat ./out/*-cmdline`;
board=`cat ./out/*-board`;
base=`cat ./out/*-base`;
pagesize=`cat ./out/*-pagesize`;

kerneloff=`cat ./out/*-kerneloff`;
ramdiskoff=`cat ./out/*-ramdiskoff`;
tagsoff=`cat ./out/*-tagsoff`;

secondoff=`cat ./out/*-secondoff`;
secondoff="--second_offset $secondoff";

osver=`cat ./out/*-osversion`;
oslvl=`cat ./out/*-oslevel`;

hash=`cat ./out/*-hash`;
test "$hash" == "unknown" && hash=sha1;
hash="--hash $hash";

dt=./out/boot.img-dt;

./mkbootimg --kernel $kernel --ramdisk $rd --cmdline "$cmdline" --base $base --pagesize $pagesize --kernel_offset $kerneloff --ramdisk_offset $ramdiskoff $secondoff --tags_offset "$tagsoff" --os_version "$osver" --os_patch_level "$oslvl" $hash --dt $dt --output newboot.img

echo -n 'SEANDROIDENFORCE' >> /tmp/dtimage/newboot.img
dd if=/tmp/dtimage/newboot.img of=/dev/block/bootdevice/by-name/boot
