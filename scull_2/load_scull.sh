#!/bin/bash
module="scull_2"
device="scull_2"

/sbin/insmod ./$module.ko $* || exit 1

rm -f /dev/$device*

major=$(avk "\$2==\$module\" {print \$1}" /proc/device)

mknod /dev/${device}0 c $major 0

