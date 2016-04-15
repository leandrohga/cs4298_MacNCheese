#!/bin/bash
if [ "$1" = "help" ]
then
    echo "This script automatically builds all of the required executables, compiles the mnc files, assembles the asm files, and runs the obj files."
    echo "Usage: ./mncRun.sh <inputFiles>              Compile and run the input files."
    echo "       ./mncRun.sh <inputFiles> clobber      Compile and run the input files, then clean up."
    echo "       ./mncRun.sh clobber                   Clean up generated files."
    echo "       ./mncRun.sh help                      Show this help message."
    echo "Example: ./mncRun.sh shoutAddition shoutFloatAssign clobber"
    echo "The clobber option cleans up (removes) the compiled executables."
    echo "Notes: "
    echo "    Do not include the .mnc extension."
    echo "    The input files are assumed to be in the tests folder."
else
    echo ">>> make build"
    make build
    echo ">>> cd tests"
    cd tests
    for var in "$@"
    do
        if [ "$var" = "clobber" ]
        then
            echo ">>> rm -f *.lis *.lst *.asm *.obj macc sam micro"
            rm -f *.lis *.lst *.asm *.obj macc sam micro
            cd complex
            rm -f *.lis *.lst *.asm *.obj macc sam micro
            cd ..
        else
            echo ">>> micro $var.mnc"
            ./micro $var.mnc
            if [ $? == 0 ]
            then
                echo ">>> sam $var.asm"
                ./sam $var.asm
                if [ $? == 0 ]
                then
                    echo ">>> macc $var.obj"
                    ./macc $var.obj
                fi
            fi
        fi
    done
fi
echo ">>> done"
