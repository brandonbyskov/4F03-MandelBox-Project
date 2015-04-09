#!/bin/sh
#
#echo "File Name: $0"
#echo "First Parameter : $1"
#echo "First Parameter : $2"
#
data_params="params/params"
data_params_ext=".dat"
executable="./mandelbox "
z="0"
#
for((a=$1; a <= $2 ; a++))
do
# echo $executable$data_params$a$data_params_ext
# eval $executable$data_params 
if [ $a -lt 10 ]
	then
	eval $executable$data_params$z$z$z$a$data_params_ext
elif [ $a -lt 100 ]
	then
	eval $executable$data_params$z$z$a$data_params_ext
elif [ $a -lt 1000 ]
	then
	eval $executable$data_params$z$a$data_params_ext
else
	eval $executable$data_params$a$data_params_ext
fi
#eval $executable$data_params$a$data_params_ext
done