#!/bin/bash

# Client 1
./rfs GET folder/receivedTestingOne.txt getFolder/getTestOne.txt&

# Client 2
./rfs GET folder/receivedTestingTwo.txt getFolder/getTestTwo.txt&

# Client 3
./rfs GET folder/testReceived.txt getFolder/receivedTestReceived.txt&

# non-existant file
./rfs GET getFolder/notReal.txt

wait