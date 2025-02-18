#!/bin/bash

LOG_DIR="./logs/"
FIND "$LOG_DIR" -type f -mtime +10 -exec rm -f {} \;
echo "Old logs have been cleaned."