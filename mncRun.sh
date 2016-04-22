#!/bin/bash
function process {
    echo ">>> micro $1.mnc"
    ./micro $1.mnc
    r=$?
    if [ $r -eq 0 ]
    then
        echo ">>> sam $1.asm"
        ./sam $1.asm
        r=$?
        if [ $r -eq 0 ]
        then
            echo ">>> macc $1.obj"
            ./macc $1.obj
            r=$?
        fi
    fi
    return $r
}
function clobber {
    rm -fv *.lis *.lst *.asm *.obj trace.txt macc sam micro
    cd complex
    rm -fv *.lis *.lst *.asm *.obj trace.txt macc sam micro
    cd ../betaTests
    rm -fv *.lis *.lst *.asm *.obj trace.txt macc sam micro
    cd ..
}
function checkTerminate {
    if [ $1 -ne 0 ]
    then
        echo ">>> Processing failure ($1), terminating script"
        exit $1
    fi
}
if [ "$1" = "help" ]
then
    echo "Usage: ./mncRun.sh <testsList> [clobber]"
    echo "Example: ./mncRun.sh myProgram1 myProgram2 clobber"
else
    echo ">>> Making all required executables"
    make build
    cd tests
    for var in "$@"
    do
        if [ "$var" = "clobber" ]
        then
            echo ">>> Removing generated files"
            clobber
        elif [ -d "$var" ]
        then
            for file in $var/*.mnc
            do
                process "${file%.*}"
                checkTerminate $?
            done
        elif [ -f "$var.mnc" ]
        then
            process "$var"
            checkTerminate $?
        else
            echo ">>> Could not find file/directory '$var' in the tests directory"
        fi
    done
fi
echo ">>> Done"
