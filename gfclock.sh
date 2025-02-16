#!/bin/bash

# Paths to sound files (modify these as needed)
HOUR_START="./grandfclock.wav"
HOUR_SOUND="./grandfclock-chime.wav"
HALF_HOUR_SOUND="./grandfclock-chime.wav"
vol=3

# Function to play the chime sound based on the time
play_chime() {
    #local hour=$(date +"%I")  # Get the current hour (12-hour format)
	local hour=$((10#$(date +"%I")))  # Fix octal issue
#echo $hour
    local minute=$(date +"%M")
    
    if [[ "$minute" == "00" ]]; then
 	   afplay "$HOUR_START" -v $vol
        # Play hour chime for the number of hours
        for ((i=1; i<=hour; i++)); do
	   echo $hour
            afplay "$HOUR_SOUND" -v $vol
            #sleep 1  # Small delay between chimes
        done
	sleep 200
    elif [[ "$minute" == "30" ]]; then
        # Play half-hour chime
        afplay "$HALF_HOUR_SOUND" -v $vol
	sleep 200
    fi

}

# Run the chime function every minute using a loop
while true; do
    play_chime
    sleep 10  # Wait for the next minute to check again
done