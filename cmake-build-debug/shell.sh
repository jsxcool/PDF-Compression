read -p "Your command: " -a cmd
# echo ${cmd[0]} ${cmd[1]} ${cmd[2]} ${cmd[3]}

if [ ${cmd[1]} == "-c" ]
then 
	echo "Start Compressing..."
	./${cmd[0]} ${cmd[1]} ${cmd[2]} ${cmd[3]}
	bzip2 ${cmd[3]}
	echo "Done"
elif [ ${cmd[1]} == "-d" ]
then
	echo "Start Deompressing..."
	bzip2 -d ${cmd[2]}
	temp=${cmd[2]: 0 : (${#cmd[2]}-4)}   # get rid of the postfix .bz2
	./${cmd[0]} ${cmd[1]} $temp ${cmd[3]}  
	ps2pdf ${cmd[3]}
	echo "Done"
fi 
