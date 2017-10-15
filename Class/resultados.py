from sys import argv
from subprocess import Popen, PIPE
import os

rootdir = argv[1]
for root, dirs, files in os.walk(rootdir):
	if (files != []):
		dirActual = str(root).split("/")
		dirActual = dirActual[1]
		if (argv[4] != "-p"):
			fileName = "Resultados/" + dirActual + "_" + argv[3] + "_" + argv[4]
		else:
			fileName = "Resultados/" + dirActual + "_" + argv[3] + "_" + argv[4] + "_" + argv[5]
		fOutput = open(fileName,'w')
		fileName = "Resultados/" + dirActual + "_Optimo"
		optFile = open(fileName,'r')
		files = sorted(files, key=int)
		fOutput.write("Optimo Valor Desviacion Tiempo \n")
		print(files)
		print(argv[4])
		print(argv[5])
		numFiles = 0
		desvTotal = 0
		totalTime = 0
		for file in files:
			fileName = str(root)+"/"+str(file)
			value = 0.0
			time = 0.0
			for i in range(0,10):
				print("Iteracion" + str(i))
				if (argv[4] != "-p"):
					p = Popen([argv[2], fileName, argv[3], argv[4]], stdout=PIPE, stderr=PIPE)
				else:
					p = Popen([argv[2], fileName, argv[3], argv[4], argv[5]], stdout=PIPE, stderr=PIPE)
				output, error = p.communicate()
				print(output)
				output = output.split("\n")
				#print(output[1])
				#print(output[2])
				value += float(output[1])
				time += float(output[2])
				#print(output)
			print(str(root)+"/"+str(file))
			value /= 10.0
			time /= 10.0
			optimo = float(optFile.readline())
			desv = ((optimo - value) / optimo) * 100
			desvTotal += desv
			totalTime += time 
			numFiles += 1
			fOutput.write(str(optimo) + " " + str(value) + " " + str(desv) + " " + str(time) + "\n")
			print(optimo)
			print(value)
			print(desv)
			print(time)
		desvTotal /= numFiles
		totalTime /= numFiles
		fOutput.write("-----------------------------\n")
		fOutput.write("Desviacion_Total Tiempo_Total\n")
		fOutput.write(str(desvTotal) + " " + str(totalTime) + "\n")
		fOutput.close()
		optFile.close()
