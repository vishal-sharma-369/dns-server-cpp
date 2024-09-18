#!/bin/sh

set -e  # Exit early if any commands fail

  cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory

# Pass the content as a parameters to the executable
./build/my_dns_client < tests/client_query_params.txt