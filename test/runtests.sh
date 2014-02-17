echo "Running unit tests:"

for i in test/*_test
do
	if test -f $i
	then
		if $MEMTEST ./$i 2>> test/tests.log
		then
			echo $i PASS
		else
			echo "ERROR in test $i: here's test/tests.log"
			echo "------"
			tail test/tests.log
			exit 1
		fi
	fi
done

echo ""
