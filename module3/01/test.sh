#!/bin/bash

echo "=== Testing file copy program ==="

gcc -Wall -Wextra -o file_copier main.c file_copier.c

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Compilation successful."

echo "Test file 1" > test1.txt
echo "Test file 2" > test2.txt
echo "Test file 3" > test3.txt
dd if=/dev/urandom of=large.bin bs=1M count=2 2>/dev/null

echo -e "\n=== Test 1: Normal operation ==="
./file_copier test1.txt test2.txt large.bin

echo -e "\n=== Test 2: Non-existent file ==="
./file_copier test1.txt nonexistent.txt test2.txt 2>error.log
echo "Error messages:"
cat error.log

echo -e "\n=== Test 3: Named pipe ==="
./file_copier -p my_test_pipe test1.txt test2.txt test3.txt

echo -e "\n=== Test 4: Multiple files ==="
for i in {1..5}; do
    echo "File $i" > f$i.txt
done
./file_copier f*.txt

echo -e "\n=== Verification ==="
for file in test1.txt test2.txt test3.txt large.bin f*.txt; do
    if [ -f "$file" ] && [ -f "$file.copy" ]; then
        if cmp -s "$file" "$file.copy"; then
            echo "✓ $file: OK"
        else
            echo "✗ $file: FAIL"
        fi
    fi
done

# Cleanup
rm -f test*.txt large.bin f*.txt *.copy my_test_pipe_* error.log file_copier
echo -e "\nTest completed"