#!/bin/bash

# Client 1
./rfs WRITE data/localfoo.txt folder/testReceived.txt

# Client 2
./rfs GET folder/testReceived.txt data/testing.txt

# Client 3
./rfs RM data/testing.txt