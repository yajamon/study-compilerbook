#!/bin/bash

assert() {
    expected="$1"
    input="$2"

    ./ycc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but $actual"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 21 '5+20-4'
assert 41 ' 12 + 34 - 5 '
assert 47 '5+6*7'
assert 26 '2*3+4*5'
assert 15 '5*(9-6)'

echo OK
