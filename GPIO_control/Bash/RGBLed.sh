#!/bin/bash
# Duty cycle
cycle_duration=20
period=500
MOUSEFILE=/dev/input/event3
LOGFILE=MOUSEMOUSEMOUSEMOUSEMOUSE

function nano_detect {
	(                                   
                grep E $MOUSEFILE > $LOGFILE      
        ) &                                          
        SUBSHELL_PID=$!                              
                                                     
        ./nanosleep                                  
        kill $SUBSHELL_PID                           
        if [ -s $LOGFILE ]                  
        then                 
                echo "Mouse detected, Terminating!!!"
                exit                                       
        fi 
}

echo Removing mouse_event for mouse detect...
rm -rf $LOGFILE
echo Please enter duty percentage of level-high
read cycle
if [ $cycle -gt 99 ] || [ $cycle -lt 1 ];
then
	echo The duty cyle is invalid, quiting...
	exit 1
else
	echo Duty cycle set.
fi

# R Led
echo Please shield pin number of R pin
read R
if [ $R -gt 13 ] || [ $R -lt 0 ];
then
	echo The R pin is invalid, quiting...
	exit 1
else
	echo R pin set.
fi

case $R in
	0)  R_pin=11;;
	1)  R_pin=12;;
	2)  R_pin=13;;
	3)  R_pin=14;;
	4)  R_pin=6;;
	5)  R_pin=0;;
	6)  R_pin=1;;
	7)  R_pin=38;;
	8)  R_pin=40;;
	9)  R_pin=4;;
	10) R_pin=10;;
	11) R_pin=5;;
	12) R_pin=15;;
	13) R_pin=7;;
	*) echo R_pin error;;
esac

# G Led
echo Please shield pin number of G pin
read G
if [ $G -gt 13 ] || [ $G -lt 0 ] || [ $G -eq $R ];
then
	echo The G pin is invalid, quiting...
	exit 1
else
	echo G pin set.
fi

case $G in
	0)  G_pin=11;;
	1)  G_pin=12;;
	2)  G_pin=13;;
	3)  G_pin=14;;
	4)  G_pin=6;;
	5)  G_pin=0;;
	6)  G_pin=1;;
	7)  G_pin=38;;
	8)  G_pin=40;;
	9)  G_pin=4;;
	10) G_pin=10;;
	11) G_pin=5;;
	12) G_pin=15;;
	13) G_pin=7;;
	*) echo G_pin error;;
esac

# B Led
echo Please shield pin number of B pin
read B
if [ $B -gt 13 ] || [ $B -lt 0 ] || [ $B -eq $R ] || [ $B -eq $G ];
then
	echo The B pin is invalid, quiting...
	exit 1
else
	echo B pin set.
fi

case $B in
	0)  B_pin=11;;
	1)  B_pin=12;;
	2)  B_pin=13;;
	3)  B_pin=14;;
	4)  B_pin=6;;
	5)  B_pin=0;;
	6)  B_pin=1;;
	7)  B_pin=38;;
	8)  B_pin=40;;
	9)  B_pin=4;;
	10) B_pin=10;;
	11) B_pin=5;;
	12) B_pin=15;;
	13) B_pin=7;;
	*) echo B_pin error;;
esac

# Initial of GPIOs
echo -n "11" > /sys/class/gpio/export 
echo -n "32" > /sys/class/gpio/export 
echo -n "12" > /sys/class/gpio/export
echo -n "28" > /sys/class/gpio/export
echo -n "45" > /sys/class/gpio/export
echo -n "13" > /sys/class/gpio/export
echo -n "34" > /sys/class/gpio/export
echo -n "77" > /sys/class/gpio/export
echo -n "14" > /sys/class/gpio/export
echo -n "16" > /sys/class/gpio/export
echo -n "76" > /sys/class/gpio/export
echo -n "6"  > /sys/class/gpio/export
echo -n "36" > /sys/class/gpio/export
echo -n "0"  > /sys/class/gpio/export
echo -n "18" > /sys/class/gpio/export
echo -n "66" > /sys/class/gpio/export
echo -n "1"  > /sys/class/gpio/export
echo -n "20" > /sys/class/gpio/export
echo -n "68" > /sys/class/gpio/export
echo -n "38" > /sys/class/gpio/export
echo -n "40" > /sys/class/gpio/export
echo -n "4"  > /sys/class/gpio/export
echo -n "22" > /sys/class/gpio/export
echo -n "70" > /sys/class/gpio/export
echo -n "10" > /sys/class/gpio/export
echo -n "26" > /sys/class/gpio/export
echo -n "74" > /sys/class/gpio/export
echo -n "5"  > /sys/class/gpio/export
echo -n "24" > /sys/class/gpio/export
echo -n "44" > /sys/class/gpio/export
echo -n "15" > /sys/class/gpio/export
echo -n "42" > /sys/class/gpio/export
echo -n "7"  > /sys/class/gpio/export
echo -n "30" > /sys/class/gpio/export
echo -n "46" > /sys/class/gpio/export

echo -n "out" > /sys/class/gpio/gpio11/direction 
echo -n "out" > /sys/class/gpio/gpio32/direction 
echo -n "out" > /sys/class/gpio/gpio12/direction
echo -n "out" > /sys/class/gpio/gpio28/direction
echo -n "out" > /sys/class/gpio/gpio45/direction
echo -n "out" > /sys/class/gpio/gpio13/direction
echo -n "out" > /sys/class/gpio/gpio34/direction
#echo -n "out" > /sys/class/gpio/gpio77/direction
echo -n "out" > /sys/class/gpio/gpio14/direction
echo -n "out" > /sys/class/gpio/gpio16/direction
#echo -n "out" > /sys/class/gpio/gpio76/direction
echo -n "out" > /sys/class/gpio/gpio6/direction
echo -n "out" > /sys/class/gpio/gpio36/direction
echo -n "out" > /sys/class/gpio/gpio0/direction
echo -n "out" > /sys/class/gpio/gpio18/direction
#echo -n "out" > /sys/class/gpio/gpio66/direction
echo -n "out" > /sys/class/gpio/gpio1/direction
echo -n "out" > /sys/class/gpio/gpio20/direction
#echo -n "out" > /sys/class/gpio/gpio68/direction
echo -n "out" > /sys/class/gpio/gpio38/direction
echo -n "out" > /sys/class/gpio/gpio40/direction
echo -n "out" > /sys/class/gpio/gpio4/direction
echo -n "out" > /sys/class/gpio/gpio22/direction
#echo -n "out" > /sys/class/gpio/gpio70/direction
echo -n "out" > /sys/class/gpio/gpio10/direction
echo -n "out" > /sys/class/gpio/gpio26/direction
#echo -n "out" > /sys/class/gpio/gpio74/direction
echo -n "out" > /sys/class/gpio/gpio5/direction
echo -n "out" > /sys/class/gpio/gpio24/direction
echo -n "out" > /sys/class/gpio/gpio44/direction
echo -n "out" > /sys/class/gpio/gpio15/direction
echo -n "out" > /sys/class/gpio/gpio42/direction
echo -n "out" > /sys/class/gpio/gpio7/direction
echo -n "out" > /sys/class/gpio/gpio30/direction
echo -n "out" > /sys/class/gpio/gpio46/direction

echo -n "0" > /sys/class/gpio/gpio32/value
echo -n "0" > /sys/class/gpio/gpio28/value
echo -n "0" > /sys/class/gpio/gpio45/value
echo -n "0" > /sys/class/gpio/gpio34/value
echo -n "0" > /sys/class/gpio/gpio77/value
echo -n "0" > /sys/class/gpio/gpio16/value
echo -n "0" > /sys/class/gpio/gpio76/value
echo -n "0" > /sys/class/gpio/gpio36/value
echo -n "0" > /sys/class/gpio/gpio18/value
echo -n "0" > /sys/class/gpio/gpio66/value
echo -n "0" > /sys/class/gpio/gpio20/value
echo -n "0" > /sys/class/gpio/gpio68/value
echo -n "0" > /sys/class/gpio/gpio22/value
echo -n "0" > /sys/class/gpio/gpio70/value
echo -n "0" > /sys/class/gpio/gpio26/value
echo -n "0" > /sys/class/gpio/gpio74/value
echo -n "0" > /sys/class/gpio/gpio24/value
echo -n "0" > /sys/class/gpio/gpio44/value
echo -n "0" > /sys/class/gpio/gpio42/value
echo -n "0" > /sys/class/gpio/gpio30/value
echo -n "0" > /sys/class/gpio/gpio46/value

COUNTER=0
high=$(echo $cycle_duration $cycle | awk '{printf("%d",$1*$2*10)}')
low=$(echo $cycle_duration $high | awk '{printf("%d",$1*1000-$2)}')
number=$((period/cycle_duration))

while [ $COUNTER -lt 10 ];
do
	#echo "$COUNTER"
	for((i=1; i<=number; i++ ))
	do
		#echo "i = $i"
		echo -n "1" > /sys/class/gpio/gpio$R_pin/value
		usleep $high
		echo -n "0" > /sys/class/gpio/gpio$R_pin/value
		usleep $low
	done
	
	nano_detect
	
	for((i=1; i<=number; i++ ))
	do
		echo -n "1" > /sys/class/gpio/gpio$G_pin/value
		usleep $high
		echo -n "0" > /sys/class/gpio/gpio$G_pin/value
		usleep $low
	done
	
	nano_detect
	
	for((i=1; i<=number; i++ ))
	do
		echo -n "1" > /sys/class/gpio/gpio$B_pin/value
		usleep $high
		echo -n "0" > /sys/class/gpio/gpio$B_pin/value
		usleep $low
	done

	nano_detect
	
	for((i=1; i<=number; i++ ))
	do
		echo -n "1" > /sys/class/gpio/gpio$R_pin/value
		echo -n "1" > /sys/class/gpio/gpio$G_pin/value
		usleep $high
		echo -n "0" > /sys/class/gpio/gpio$R_pin/value
		echo -n "0" > /sys/class/gpio/gpio$G_pin/value
		usleep $low
	done

	nano_detect
	
	for((i=1; i<=number; i++ ))
	do
		echo -n "1" > /sys/class/gpio/gpio$R_pin/value
		echo -n "1" > /sys/class/gpio/gpio$B_pin/value
		usleep $high
		echo -n "0" > /sys/class/gpio/gpio$R_pin/value
		echo -n "0" > /sys/class/gpio/gpio$B_pin/value
		usleep $low
	done

	nano_detect
	
	for((i=1; i<=number; i++ ))
	do
		echo -n "1" > /sys/class/gpio/gpio$G_pin/value
		echo -n "1" > /sys/class/gpio/gpio$B_pin/value
		usleep $high
		echo -n "0" > /sys/class/gpio/gpio$G_pin/value
		echo -n "0" > /sys/class/gpio/gpio$B_pin/value
		usleep $low
	done

	nano_detect
	
	for((i=1; i<=number; i++ ))
	do
		echo -n "1" > /sys/class/gpio/gpio$R_pin/value
		echo -n "1" > /sys/class/gpio/gpio$G_pin/value
		echo -n "1" > /sys/class/gpio/gpio$B_pin/value
		usleep $high
		echo -n "0" > /sys/class/gpio/gpio$R_pin/value
		echo -n "0" > /sys/class/gpio/gpio$G_pin/value
		echo -n "0" > /sys/class/gpio/gpio$B_pin/value
		usleep $low
	done

	nano_detect
	
	#COUNTER=$((COUNTER+1))

done
