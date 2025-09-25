#!/bin/bash

# Write error message
function write_error() {
    printf "\033[0;31m%s\033[0m\n" "${1}"
}

function write_green() {
    printf "\033[0;32m%s\033[0m\n" "${1}"
}

TEST_DIR="./tests" 
PARSER_PATH="./parser"

# All valid parse must begin with valid_ and not valid must start with error_
VALID_PARSE="valid_"
ERROR_PARSE="error_"

VALID_FILES=( $(find $TEST_DIR -maxdepth 1 -name "$VALID_PARSE*") )

test_counter=0
error_counter=0

for valid_file in "${VALID_FILES[@]}"
do
    test_counter=$((test_counter+1))
    if ! $PARSER_PATH "$valid_file" >> "/dev/null"
    then
        error_counter=$((error_counter+1))
        write_error "Test file $valid_file should be parsable, but appears to be not..."
    fi
done

ERROR_FILES=( $(find $TEST_DIR -maxdepth 1 -name "$ERROR_PARSE*") )
for error_file in "${ERROR_FILES[@]}"
do
    test_counter=$((test_counter+1))
    if $PARSER_PATH "$error_file" >> "/dev/null";
    then
        error_counter=$((error_counter+1))
        write_error "Test file $error_file should not be parsable, but appears to be..."
    fi
done

if [[ $error_counter == 0 ]]
then
    write_green "All tests passed! ($test_counter/$test_counter)"
else
    write_error "Not all tests passed! ($error_counter/$test_counter)"
fi