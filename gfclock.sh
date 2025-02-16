#!/bin/bash

# Paths to sound files (modify these as needed)
HOUR_START="./grandfclock.wav"
HOUR_SOUND="./grandfclock-chime.wav"
fiftenChime="./grandfclock-chime.wav"
fourtyfive_SOUND="./grandfclock-chime.wav"
HALF_HOUR_SOUND="./grandfclock-chime.wav"
vol=3
mac=true
fourminchimes=false
# Function to play the chime sound based on the time
play_chime() {
    #local hour=$(date +"%I")  # Get the current hour (12-hour format)
	local hour=$((10#$(date +"%I")))  # Fix octal issue
#echo $hour
    local minute=$(date +"%M")
    
    if [[ "$minute" == "00" ]]; then
	if [ $mac ]; then
 	   	afplay "$HOUR_START" -v $vol
		say "the time is now" $hour
	else
		aplay "$HOUR_START"
	fi
        # Play hour chime for the number of hours
        for ((i=1; i<=hour; i++)); do
	   #echo $hour
		if [ $mac ]; then
            	afplay "$HOUR_SOUND" -v $vol
		else
			aplay "$HOUR_SOUND"
		fi
            #sleep 1  # Small delay between chimes
        done
	sleep 200
    elif [[ "$minute" == "30" ]]; then
        # Play half-hour chime
	if [ $mac ]; then
		say $hour "thirty"//$minute
        	afplay "$HALF_HOUR_SOUND" -v $vol/2
	else
		aplay "$HALF_HOUR_SOUND"
	fi
	sleep 200
fi
if [ $fourminchimes ]; then
    if [[ "$minute" == "15" ]]; then
        # Play half-hour chime
	if [ $mac ]; then
		say $hour "fifteen"//$minute
        	afplay "$fifteen_SOUND" -v $vol/2
	else
		aplay "$fifteen_SOUND"
	fi
	sleep 200
    elif [[ "$minute" == "45" ]]; then
        # Play half-hour chime
	if [ $mac ]; then
		say $hour "fourty five"//$minute
        	afplay "$fourtyfive_SOUND" -v $vol/2
	else
		aplay "$fourtyfive_SOUND"
	fi
	sleep 200
	fi
fi

}

# Run the chime function every minute using a loop
while true; do
    play_chime
    sleep 10  # Wait for the next minute to check again
done