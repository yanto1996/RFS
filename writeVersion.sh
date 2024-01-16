#!/bin/bash

./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&
./rfs WRITE data/localfoo.txt versions/testReceived.txt&

wait