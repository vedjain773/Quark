#!/bin/bash

for file in tests/*.c
do
    expected=$(grep "EXPECTED:" "$file" | cut -d':' -f2 | xargs)

    for mode in normal optimized
    do
        if [ "$mode" = "optimized" ]; then
            ./quark "$file" -z -o test.o
            label="[OPT]"
        else
            ./quark "$file" -o test.o
            label="[STD]"
        fi

        if [ $? -ne 0 ]; then
            echo -e "\033[31m [FAIL] \033[0m $label $file compiler crashed"
            continue
        fi

        gcc test.o -o out/test

        if [ $? -ne 0 ]; then
            echo -e "\033[31m [FAIL] \033[0m $label $file linker error"
            continue
        fi

        ./out/test
        actual=$?

        if [ "$actual" = "$expected" ]; then
            echo -e "\033[34m [PASS] \033[0m $label $file"
        else
            echo -e "\033[31m [FAIL] \033[0m $label $file expected=$expected got=$actual"
        fi
    done
done

rm -f test.o out/test
