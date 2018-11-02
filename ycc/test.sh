#!/bin/bash
try() {
    expected="$1"
    input="$2"

    test -d tmp || mkdir -v tmp

    ./bin/ycc "$input" > tmp/tmp.s
    gcc -o bin/tmp tmp/tmp.s
    ./bin/tmp
    actual="$?"

    if [ "$actual" != "$expected" ]; then
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 0 0
try 42 42

echo 'OK'
