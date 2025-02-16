#!/bin/bash

# Paths to sound files (modify these as needed)
HOURSTART="./grandfclock.wav"
HOURSOUND="./grandfclock-chime.wav"
fifteenSOUND="./grandfclock-chime.wav"
fourtyfiveSOUND="./grandfclock-chime.wav"
HALFHOURSOUND="./grandfclock-chime.wav"
vol=3
mac=true	# MACOS option
fourminchimes=false # 15 minute chimes
thirtychime=true # half hour chimes

play_chime() {
	local hour=$((10#$(date +"%I")))  # Fix octal issue
    local minute=$(date +"%M")
    
    if [[ "$minute" == "00" ]]; then
	if [ $mac ]; then
 	   	afplay "$HOURSTART" -v $vol
		say "the time is now" $hour
	else
		aplay "$HOURSTART"
	fi
        # Play hour chime for the number of hours
        for ((i=1; i<=hour; i++)); do
	   #echo $hour
		if [ $mac ]; then
            	afplay "$HOURSOUND" -v $vol/2
		else
			aplay "$HOURSOUND"
		fi
            #sleep 1  # Small delay between chimes
        done
	sleep 200
fi
if $thirtychime; then
    if [[ "$minute" == "30" ]]; then
        # Play half-hour chime
	if [ $mac ]; then
		say $hour "thirty"#$minute
        	afplay "$HALFHOURSOUND" -v $vol/2
	else
		aplay "$HALFHOURSOUND"
	fi
	sleep 200
    fi
fi
if $fourminchimes ; then
    if [[ "$minute" == "15" ]]; then
        # Play half-hour chime
	if [ $mac ]; then
		say $hour "fifteen" #$minute
        	afplay "$fifteenSOUND" -v $vol/2
	else
		aplay "$fifteenSOUND"
	fi
	sleep 200
    elif [[ "$minute" == "45" ]]; then
        # Play half-hour chime
	if [ $mac ]; then
		say $hour "fourty five" #$minute
        	afplay "$fourtyfiveSOUND" -v $vol/2
	else
		aplay "$fourtyfiveSOUND"
	fi
	sleep 200
	fi
fi

}

# Run the chime function every minute using a loop
while true; do
    play_chime
    sleep 10 
done