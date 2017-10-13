#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Uso: ./resultado.sh pathDirectorioEntrada"
else
	for dir in `ls -v $1`
	do	
		rm -f Resultados/$dir"_Resultados"
		echo "Generating results for $dir" 	
		for entry in `ls -v "$1/$dir"`
		do
			#echo $entry
			#echo "$1$dir/$entry"
			result=$(./mdp "$1$dir/$entry" -r -p)
			echo $result					
			y=(${result//$'\n'/ })
			echo ${y[2]}
			echo ${y[2]} >> Resultados/$dir"_Resultados"		 
		done
		#python resultados.py $dir"_Opt" $dir"_Result"
		echo " "
	done
fi