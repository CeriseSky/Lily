#!/bin/bash

set -e

mkdir -p bin

gcc vecspeed.c -o bin/vecspeed -I ../include -Wall -Wextra -O3

