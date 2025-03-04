#!/bin/bash

# constant start time (30 seconds from now)
START_TIME=$(( $(date +%s) + 30 ))
CONTAINERS=("trader1" "trader2") # containers
EXECUTABLE_PATHS=("/home/ubuntu/component1" "home/ubuntu/component2") # executables

# Ensure the arrays have the same length
if [[ ${#CONTAINERS[@]} -ne ${#EXECUTABLE_PATHS[@]} ]]; then
    echo "Error: Arrays must have the same length."
    exit 1
fi

# iterate through arrays
for i in "${!CONTAINERS[@]}"; do
    CONTAINER="${CONTAINERS[$i]}"
    EXECUTABLE="${EXECUTABLE_PATHS[$i]}"

    echo "Executing executable $EXECUTABLE in container $CONTAINER"

    # incus exec commands
    incus exec "$CONTAINER" --user 1000 --group 1000 -- "$EXECUTABLE" "$START_TIME" >& "$CONTAINER".out &
done
