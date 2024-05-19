#!/bin/bash
PROGRAM_CMD="AIS-catcher -C /home/NeptunePink/BoatyConfig.json -v 5 -o 5"
TRANSMIT_CMD="python /home/NeptunePink/CarryTheBoats.py"

$PROGRAM_CMD 2>&1 | $TRANSMIT_CMD &
pid=$!

rm /home/NeptunePink/BoatyOutput.log

{
	echo "Boats Launched Under Process: $pid"
	echo "-------------------" 
}> BoatyOutput.log

tail /home/NeptunePink/BoatyOutput.log -f
