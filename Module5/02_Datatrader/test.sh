#!/bin/bash

echo "========================"
echo "Start testing datatrader"
echo "========================"



if ! lsmod | grep -q datatrader; then
    echo -e "\033[33mDatatrader not loaded. Loading...\033[0m"
    sudo insmod datatrader.ko
    sleep 1
fi

echo -e "\033[32mDirectory /proc/datatrader exists.\033[0m"

if [ ! -d "/proc/datatrader" ]; then
    echo -e "\033[31mERROR: /proc/datatrader directory not found!\033[0m"
    exit 1
fi

echo -e "\033[32mlsmod | grep datatrader\033[0m"
lsmod | grep datatrader

TEST_DATA="Hello"
echo -e "\033[32mWriting to buffer: $TEST_DATA\033[0m"
echo "$TEST_DATA" >> /proc/datatrader/data

echo -e "\n\033[32msudo dmesg | tail -5:\033[0m"
sudo dmesg | tail -5

echo -e "\033[33mRemove datatrader. Loading...\033[0m"
sudo rmmod datatrader

echo -e "\033[32mlsmod | grep datatrader\033[0m"
lsmod | grep datatrader

echo -e "\033[33mTest done\033[0m"