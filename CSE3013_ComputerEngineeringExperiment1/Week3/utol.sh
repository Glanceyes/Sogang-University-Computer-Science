echo "working directory"
read directory
if [ -z $directory ]
then
	echo "Error! You didn't tell me a directory name."
	exit 1
else
	if [ ! -d $directory -o ! -x $directory ]
	then
		echo "Error! The directory doesn't exist or it can't be accessible."
		exit 2
	else
		cd $directory
		changename=""
		for i in *
		do
			if [ -f $i ]
			then
				changename=$(echo $i | tr '[:lower:][:upper:]' '[:upper:][:lower:]')
				if [ -f $changename ]
				then
					echo "$changename file already exist."
				else
					mv $i $changename
				fi
			fi
		done		
	fi
fi
exit 0
