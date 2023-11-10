#!/bin/bash

make -C ../

run_test() {
    local dir="$1"
    echo "=================================================="
    echo "Testing: $dir"
    echo "=================================================="
    cd "$dir" && bash test.sh
    cd - > /dev/null
    echo -e "\n\n"
}

run_test default_error_page
run_test listen

run_test location
run_test location/allow_methods
run_test location/autoindex
run_test location/chunked_transfer_encoding
run_test location/client_max_body_size
run_test location/error_page
run_test location/index

run_test server_name
run_test simple
