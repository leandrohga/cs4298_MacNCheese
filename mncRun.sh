#!/bin/bash
if [ "$1" = "help" ]
then
    echo "This script automatically builds all of the required executables, compiles the mnc files, assembles the asm files, and runs the obj files."
    echo "Usage: ./mncRun.sh <inputFiles>"
    echo "Example: ./mncRun.sh simple shoutTest declarations"
    echo "Note: do not include the .mnc extension, also the files are assumed to be in the tests folder"
else
    make build
    cd tests
    for var in "$@"
    do
        echo -e "\n\n****** Starting '$var' ******"
        echo -e "\n*** Compiling '$var.mnc' ***"
        ./micro $var.mnc
        echo -e "\n*** Assembling '$var.mnc' ***"
        ./sam $var.asm
        echo -e "\n*** Running '$var.obj' ***"
        ./macc $var.obj
        echo -e "\n****** Done with '$var' ******"
    done
fi
