make clean
make
#./create_hdd.sh
# qemu-system-x86_64 -L . -m 64 -fda ./Disk.img -hda ./HDD.img --boot a -M pc -curses
#qemu-system-x86_64 -L . -m 64 -fda ./Disk.img -M pc -curses -s -S
#qemu-system-x86_64 -L . -m 64 -fda ./Disk.img -hda ./HDD.img --boot a -M pc -serial tcp::4444,server,nowait -curses
qemu-system-x86_64 -L . -m 64 -fda ./Disk.img -hda ./HDD.img --boot a -M pc -serial tcp::4444,server,nowait -smp 10 -curses

stty sane
