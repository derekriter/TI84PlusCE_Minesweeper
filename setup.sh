#!/bin/bash
if [[ $0 != "/bin/bash" ]]; then
    echo "Must be sourced! ex. 'source setup.sh'"
    exit
fi

export PATH=$PATH:$PWD/CEdev/bin
