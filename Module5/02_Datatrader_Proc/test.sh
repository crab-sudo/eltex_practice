#!/bin/bash

PROC_DIR="datraproc"
PROC_FILE="data"
MODULE_NAME="datatrader_proc"
MODULE_FILE="${MODULE_NAME}.ko"


echo "========================"
echo "Start testing datatrader"
echo "========================"



if ! lsmod | grep -q "${MODULE_NAME}"; then
    echo -e "\033[33m${MODULE_NAME} not loaded. Loading...\033[0m"
    sudo insmod "${MODULE_FILE}"
    sleep 1
fi

echo -e "\033[32mDirectory /proc/${PROC_DIR} exists.\033[0m"

if [ ! -d "/proc/${PROC_DIR}" ]; then
    echo -e "\033[31mERROR: /proc/${PROC_DIR} directory not found!\033[0m"
    exit 1
fi

echo -e "\033[32mlsmod | grep ${MODULE_NAME}\033[0m"
lsmod | grep "${MODULE_NAME}"

TEST_DATA="Hello"
echo -e "\033[32mWriting to buffer:\033[0m $TEST_DATA"
echo "$TEST_DATA" >> /proc/"${PROC_DIR}"/"${PROC_FILE}"

echo -e "\n\033[32msudo dmesg | tail -5:\033[0m"
sudo dmesg | tail -5

echo -e "\033[33mRemove ${MODULE_NAME}. Loading...\033[0m"
sudo rmmod ${MODULE_NAME}

echo -e "\033[32mlsmod | grep ${MODULE_NAME}\033[0m"
lsmod | grep ${MODULE_NAME}

echo -e "\033[33mTest done\033[0m"