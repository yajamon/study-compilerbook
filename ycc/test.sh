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
assert 4 '(3+5)/2'

assert 1 '+1'
assert 6 '2 * +3'
assert 3 '-2 + 5'
assert 20 '30 + (4 - 2) * -5'

assert 1 '2 == 2'
assert 0 '2 == 3'
assert 0 '2 != 2'
assert 1 '2 != 3'

assert 1 '2 < 3'
assert 0 '3 < 3'
assert 0 '4 < 3'
assert 1 '2 <= 3'
assert 1 '3 <= 3'
assert 0 '4 <= 3'

echo OK
