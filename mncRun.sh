#!/bin/bash
if [ "$1" = "help" ]
then
    echo "This script automatically builds all of the required executables, compiles the mnc files, assembles the asm files, and runs the obj files."
    echo "Usage: ./mncRun.sh <inputFiles>"
    echo "       ./mncRun.sh clobber"
    echo "       ./mncRun.sh help"
    echo "Example: ./mncRun.sh simple shoutTest declarations"
    echo "The clobber option cleans up (removes) the compiled executables."
    echo "Notes: "
    echo "    Do not include the .mnc extension."
    echo "    The input files are assumed to be in the tests folder."
elif [ "$1" = "clobber" ]
then
    echo ">>> cd tests"
    cd tests
    echo ">>> rm -f *.lis *.lst *.asm *.obj macc sam micro"
    rm -f *.lis *.lst *.asm *.obj macc sam micro
else
    echo ">>> make build"
    make build
    echo ">>> cd tests"
    cd tests
    for var in "$@"
    do
        echo ">>> micro $var.mnc"
        ./micro $var.mnc
        echo ">>> sam $var.asm"
        ./sam $var.asm
        echo ">>> macc $var.obj"
        ./macc $var.obj
    done
fi
echo ">>> done"
