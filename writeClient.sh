#!/bin/bash

# Client 1
./rfs WRITE data/localfoo.txt folder/testReceived.txt&

# Client 2
./rfs WRITE data/testingOne.txt folder/receivedTestingOne.txt&

# Client 3
./rfs WRITE data/testingTwo.txt folder/receivedTestingTwo.txt&

# non existant local path file
./rfs WRITE data/notReal.txt&

wait