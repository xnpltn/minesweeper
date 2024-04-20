#!/bin/bash
gcc -o game  main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game