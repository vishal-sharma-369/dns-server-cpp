#!/bin/sh

set -e  # Exit early if any commands fail

cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory

exec ./build/my_dns_client "$@"
