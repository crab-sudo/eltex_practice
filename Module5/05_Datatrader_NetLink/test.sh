#!/bin/bash

NETLINK_ID=31
MODULE_NAME="datatrader_netlink"
MODULE_FILE="${MODULE_NAME}.ko"

echo "========================"
echo "Start testing ${MODULE_NAME}"
echo "========================"



if ! lsmod | grep -q "${MODULE_NAME}"; then
    echo -e "\033[33m${MODULE_NAME} not loaded. Loading...\033[0m"
    sudo insmod "${MODULE_FILE}"
    sleep 1
fi

echo -e "\033[32mlsmod | grep ${MODULE_NAME}\033[0m"
lsmod | grep "${MODULE_NAME}"

TEST_DATA="Hello"
echo -e "\033[32mWriting to buffer:\033[0m $TEST_DATA"
sudo ./test_netlink "$TEST_DATA"


echo -e "\n\033[32msudo dmesg | tail -5:\033[0m"
sudo dmesg | tail -5

echo -e "\033[33mRemove ${MODULE_NAME}. Loading...\033[0m"
sudo rmmod ${MODULE_NAME}

echo -e "\033[32mlsmod | grep ${MODULE_NAME}\033[0m"
lsmod | grep ${MODULE_NAME}

echo -e "\033[33mTest done\033[0m"