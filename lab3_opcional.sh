#!/bin/bash

ejecutable="./ejecutable.exe"
fichero_salida="salida.txt"
n=1

gcc practica3.c -o ejecutable.exe
#Para ejecutar el Opcional 1 con el Opcional 2 
#gcc practica3_sigsuspend.c -o ejecutable_sigsuspend.exe 

echo "Límite Alto | Límite Medio | Límite Bajo | Recursos Totales"
echo "----------------------------------------------------------"

for ((lim_bajo=0; $lim_bajo<=200; lim_bajo+=20)); do
	for ((lim_medio=0; lim_medio<=200; lim_medio+=20)); do
		for ((lim_alto=0; lim_alto<=200; lim_alto+=20)); do
			
			#ejecucion del codigo con los limites actuales y redireccion al fichero de salida
			$ejecutable $lim_alto $lim_medio $lim_bajo $n > $fichero_salida
			
			#guardar recursos totales en una variable, el valor se encontrará a continuación de la frase: "Total recursos extraidos:"
			extraccion_total=$(grep "Total recursos extraídos: " $fichero_salida | awk '{print $4}')
			
			#impresion por pantalla para crear una tabla con los datos actuales
			echo "$lim_alto $lim_medio $lim_bajo $extraccion_total"
		done
	done
done
