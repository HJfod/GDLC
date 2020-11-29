#!/bin/bash

INP_NAME="main.cpp"
OUT_NAME="main"
ONL_COMP=0
COM_PARM="-lglut -lGL"
COM_ARGS=""

while getopts ":a:c:" opt; do
  case $opt in
    c)
      ONL_COMP=$OPTARG >&2
      ;;
    a)
      COM_ARGS=$OPTARG >&2
      ;;
    \?)
      echo "Invalid option: -$OP
      exit 1TARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done


printf "Compiling...\n"
# BAR="––––––––––"
BAR=""  # no bars rn
if clang++ $INP_NAME -o $OUT_NAME $COM_PARM; then
    printf "Succesfully compiled!\n"
    if [ $ONL_COMP = 0 ]; then
        printf "Running...\n${BAR}\n"
        ./$OUT_NAME $COM_ARGS
    fi
else
    printf "\nCompile error, see output above\n"
fi