#!/bin/bash

DEV_PATH="/dev/datrachar"
MODULE_NAME="datatrader_chardev"
MODULE_FILE="${MODULE_NAME}.ko"


echo "========================"
echo "Start testing ${MODULE_NAME}"
echo "========================"



if ! lsmod | grep -q "${MODULE_NAME}"; then
    echo -e "\033[33m${MODULE_NAME} not loaded. Loading...\033[0m"
    sudo insmod "${MODULE_FILE}"
    sleep 1
fi

if [ ! -c "${DEV_PATH}" ]; then
    echo -e "\033[31mERROR: ${DEV_PATH} directory not found!\033[0m"
    exit 1
else 
    echo -e "\033[32mDirectory ${DEV_PATH} exists.\033[0m"
fi

echo -e "\033[32mlsmod | grep ${MODULE_NAME}\033[0m"
lsmod | grep "${MODULE_NAME}"

TEST_DATA="Hello"
echo -e "\033[32mWriting to buffer:\033[0m $TEST_DATA"
sudo echo "$TEST_DATA" > "${DEV_PATH}" > /dev/null


echo -e "\033[32msudo cat ${DEV_PATH}\033[0m"
sudo cat "${DEV_PATH}"


echo -e "\n\033[32msudo dmesg | tail -5:\033[0m"
sudo dmesg | tail -5

echo -e "\033[33mRemove ${MODULE_NAME}. Loading...\033[0m"
sudo rmmod ${MODULE_NAME}

echo -e "\033[32mlsmod | grep ${MODULE_NAME}\033[0m"
lsmod | grep ${MODULE_NAME}

echo -e "\033[33mTest done\033[0m"