#!/bin/sh

set -e  # Exit early if any commands fail

exec ./build/my_dns_client "$@"
