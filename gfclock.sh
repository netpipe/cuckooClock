#!/bin/bash

# Paths to sound files (modify these as needed)
HOUR_START="./grandfclock.wav"
HOUR_SOUND="./grandflock.wav"
HALF_HOUR_SOUND="./grandfclock-chime.wav"

# Function to play the chime sound based on the time
play_chime() {
    local hour=$(date +"%I")  # Get the current hour (12-hour format)
    local minute=$(date +"%M")
    
    if [[ "$minute" == "00" ]]; then
        # Play hour chime for the number of hours
        for ((i=1; i<=hour; i++)); do
	    afplay "$HOUR_START"
            afplay "$HOUR_SOUND"
            #sleep 1  # Small delay between chimes
        done
    elif [[ "$minute" == "30" ]]; then
        # Play half-hour chime
        afplay "$HALF_HOUR_SOUND"
    fi
}

# Run the chime function every minute using a loop
while true; do
    play_chime
    sleep 60  # Wait for the next minute to check again
done