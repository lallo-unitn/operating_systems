#!/usr/bin/env bash
count=0;
nargs=$#;
if [[ "$1" -lt 11 && "$1" -gt 0 ]]
then
    while [ $count -lt $1 ]
    do
        (echo "Child PID = $BASHPID");
        let "count = count + 1";
    done
elif [ $nargs -ne 1 ]
then
    echo "Argument needed";
    exit 2;
elif [[ "$1" -gt 11 || "$1" -lt 1 ]]
then
    echo "Argument must be less then 11 and greater then 0";
    exit 2;
fi