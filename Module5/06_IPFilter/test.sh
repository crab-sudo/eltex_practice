#!/bin/bash

PROC_FILE="ip_blacklist"
MODULE_NAME="ipfilter"
MODULE_FILE="${MODULE_NAME}.ko"

clear

echo "========================"
echo "Start testing ${MODULE_NAME}"
echo "========================\n"



if ! lsmod | grep -q "${MODULE_NAME}"; then
    echo -e "\033[33m${MODULE_NAME} not loaded. Loading...\033[0m"
    sudo insmod "${MODULE_FILE}"
    sleep 1
fi

if [ ! -f "/proc/${PROC_FILE}" ]; then
    echo -e "\033[31mERROR: /proc/${PROC_FILE} directory not found!\033[0m"
    exit 1
fi

echo -e "\033[32mlsmod | grep ${MODULE_NAME}\033[0m"
lsmod | grep "${MODULE_NAME}"



TEST_DATA="add 94.131.14.113"
echo -e "\033[32m\nWriting to ${PROC_FILE}:\033[0m $TEST_DATA"
echo "$TEST_DATA" > "/proc/${PROC_FILE}"

TEST_DATA="add 192.168.10.1"
echo -e "\033[32mWriting to ${PROC_FILE}:\033[0m $TEST_DATA"
echo "$TEST_DATA" > "/proc/${PROC_FILE}"

TEST_DATA="add 127.10.0.1"
echo -e "\033[32mWriting to ${PROC_FILE}:\033[0m $TEST_DATA"
echo "$TEST_DATA" > "/proc/${PROC_FILE}"

TEST_DATA="add 192.168.10.2"
echo -e "\033[32mWriting to ${PROC_FILE}:\033[0m $TEST_DATA"
echo "$TEST_DATA" > "/proc/${PROC_FILE}"



echo -e "\033[32m\ncat /proc/${PROC_FILE}\033[0m"
cat /proc/${PROC_FILE}



TEST_DATA="del 192.168.10.1"
echo -e "\033[32m\nWriting to ${PROC_FILE}:\033[0m $TEST_DATA"
echo "$TEST_DATA" > "/proc/${PROC_FILE}"



echo -e "\033[32m\ncat /proc/${PROC_FILE}/n\033[0m"
cat /proc/${PROC_FILE}

read -n 1 -s -p "Preess any key to continue..."

echo -e "\033[33m\nRemove ${MODULE_NAME}. Loading...\033[0m"
sudo rmmod ${MODULE_NAME}

echo -e "\033[32mlsmod | grep ${MODULE_NAME}\033[0m"
lsmod | grep ${MODULE_NAME}

echo -e "\033[33mTest done\033[0m"