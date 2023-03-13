#!/bin/bash

if [ "$SSU_BACKUP_PATH" != "TRUE" ]; then
SSU_BACKUP_PATH=$(pwd)
echo "export SSU_BACKUP_PATH=TRUE" >> ~/.bashrc
echo "export PATH=$PATH:$SSU_BACKUP_PATH" >> ~/.bashrc
source ~/.bashrc
fi
