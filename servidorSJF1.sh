#!/bin/bash
make clean
make
rm output-folder/output
clear
./bin/orchestrator output-folder 1 SJF