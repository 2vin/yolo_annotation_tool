#!/bin/bash -e

#############
### Usage ###
#############

## Compile the program using the following command
g++ annotation.cpp -o annotation `pkg-config --cflags --libs opencv` -std=c++11 -lpthread

## Run the executable with arguments
# ./annotation -v video_path -l label_file -o output_folder
