#!/bin/bash

for file in tests/*.c
do
    expected=$(grep "EXPECTED:" "$file" | cut -d':' -f2 | xargs)

    ./minic "$file" -o test.o

    if [ $? -ne 0 ]; then
        echo -e "\033[31m [FAIL] \033[0m compiler crashed"
        continue
    fi

    gcc test.o -o out/test

    if [ $? -ne 0 ]; then
        echo -e "\033[31m [FAIL] \033[0m linker error"
        continue
    fi

    ./out/test
    actual=$?

    if [ "$actual" = "$expected" ]; then
        echo -e "\033[34m [PASS] \033[0m $file"
    else
        echo -e "\033[31m [FAIL] \033[0m $file expected=$expected got=$actual"
    fi
done
