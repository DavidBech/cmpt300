#!/bin/bash
make > /dev/null
if [[ ! (-e UnixLs) ]]
then 
    echo "Error running Make"
    exit 1
fi

diff <(./UnixLs "$@") <(ls --time-style="+%b %e %Y %H:%m" -U1 "$@")
if [[ "$?" -eq 0 ]] 
then 
    echo "Success"
else
    echo "Failed"
fi




