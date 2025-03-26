#!/bin/bash

# *******************************************************************************
#  Author  : Lucas Vanzelli
#  Date    : 02/10/2025
#  Description: CS392 - Homework 1
#  Pledge  : I pledge my honor that I have abided by the Stevens Honor System.
# *******************************************************************************

#path to .recycle directory is readonly
readonly path=~/.recycle

#if the .recycle directory does not exist, then make it
if [ ! -d $path ]; then 
    mkdir $path
fi

#counts number of valid flags
validFlags=0

#see which flag is triggered
hFlag=0
lFlag=0
pFlag=0

#getopts that tracks valid flags and which flag is used
while getopts ":hlp" options; do 
    case "${options}" in 
        h)
            ((validFlags++))
            ((hFlag++))
            ;;
        l)
            ((validFlags++))
            ((lFlag++))
            ;;
        p)  
            ((validFlags++))
            ((pFlag++))
            ;;
        *) 
            cat<<EOF
Usage: rbin.sh [-hlp] [list of files]
   -h: Display this help;
   -l: List files in the recycle bin;
   -p: Empty all files in the recycle bin;
   [list of files] with no other flags,
        these files will be moved to the
        recycle bin.
EOF
            echo "Error: Unknown option '-${OPTARG}'." >&2
            exit 1
        ;;
    esac
done

#if there is more than one valid flag then uh oh
if [ $validFlags -gt 1 ]; then
    cat<<EOF
Usage: rbin.sh [-hlp] [list of files]
   -h: Display this help;
   -l: List files in the recycle bin;
   -p: Empty all files in the recycle bin;
   [list of files] with no other flags,
        these files will be moved to the
        recycle bin.
EOF
    echo "Error: Too many options enabled." >&2
    exit 1
fi

#if the f flag was triggered
if [ $hFlag -eq 1 ]; then
    cat<<EOF
Usage: rbin.sh [-hlp] [list of files]
   -h: Display this help;
   -l: List files in the recycle bin;
   -p: Empty all files in the recycle bin;
   [list of files] with no other flags,
        these files will be moved to the
        recycle bin.
EOF
    exit 0
fi

#if the l flag was triggered
if [ $lFlag -eq 1 ]; then
    ls -lAF $path
    exit 0
fi

#if the p flag was triggered
if [ $pFlag -eq 1 ]; then
    rm -rf $path
    exit 0
fi


#checks for no arguments
if [ $# -eq 0 ]; then
    cat<<EOF
Usage: rbin.sh [-hlp] [list of files]
   -h: Display this help;
   -l: List files in the recycle bin;
   -p: Empty all files in the recycle bin;
   [list of files] with no other flags,
        these files will be moved to the
        recycle bin.
EOF
    exit 1
fi

#moving files into recycle
for file in $@; do
    if [ -e $file ]; then 
        mv $file $path
    else  
        echo "Warning: '$file' not found." >&2
        
    fi
done

