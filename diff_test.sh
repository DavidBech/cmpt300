#!/bin/bash
if [[ -e UnixLs ]]
then
    ./UnixLs "$@"
    ls -U "$@"
    echo "Diff Output"
    diff <(./UnixLs "$@") <(ls -Ux "$@")
else 
    echo "Build Project\n"
fi




