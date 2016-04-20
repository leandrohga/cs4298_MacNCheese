#!/bin/bash
function process {
    echo ">>> micro $1.mnc"
    ./micro $1.mnc
    if [ $? == 0 ]
    then
        echo ">>> sam $1.asm"
        ./sam $1.asm
        if [ $? == 0 ]
        then
            echo ">>> macc $1.obj"
            ./macc $1.obj
        fi
    fi
    return $?
}
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
            echo ">>> rm -f *.lis *.lst *.asm *.obj trace.txt macc sam micro"
            rm -f *.lis *.lst *.asm *.obj trace.txt macc sam micro
            cd complex
            rm -f *.lis *.lst *.asm *.obj trace.txt macc sam micro
            cd ../betaTests
            rm -f *.lis *.lst *.asm *.obj trace.txt macc sam micro
            cd ..
        elif [ -d "$var" ]
        then
            for file in $var/*.mnc
            do
                process "${file%.*}"
                if [ $? != 0 ]
                then
                    echo ">>> Processing failure ($?), terminating script"
                    exit $?
                fi
            done
        else
            process "$var"
            if [ $? != 0 ]
            then
                echo ">>> Processing failure ($?), terminating script"
                exit $?
            fi
        fi
    done
fi
echo ">>> done"
