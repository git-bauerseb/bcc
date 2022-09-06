#! /bin/bash

# Shell script for running a specific test
if [ ! -f ./bin/main ]; then
  echo "building compiler"
  make
fi

# Run specified test
TEST_INPUT="./programs/source/test_"
TEST_INPUT+="$1.c"

BIN_OUTPUT="./programs/bin/test_"
BIN_OUTPUT+="$1"

echo "compiling test $1"
./bin/bcc $TEST_INPUT -o $TEST_OUTPUT

echo "executing test $1"
$BIN_OUTPUT