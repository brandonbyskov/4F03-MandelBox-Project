#!/bin/sh

echo "File Name: $0"
echo "First Parameter : $1"
echo "First Parameter : $2"

data_params="params"
data_params_ext=".dat"
executable="./mandelbox "


for ((a=$1; a <= $2 ; a++))
do
   # echo $executable$data_params$a$data_params_ext
   # eval $executable$data_params 
   eval $executable$data_params$a$data_params_ext
done