#!/bin/sh

set -e  # Exit early if any commands fail

echo -n "hello" | nc -4u localhost 2053
