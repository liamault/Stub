#!/bin/bash

# Check if at least two arguments are provided
if [ "$#" -lt 2 ]; then
    echo "Usage:"
    echo "  $0 input1.csv [input2.csv ... inputN.csv] output.csv"
    echo "  At least one input file and one output file must be specified."
    exit 1
fi

# Assign output file (last argument)
output=${!#}  # The last argument

# Collect all input files (all but the last argument)
input_files=("${@:1:$(($#-1))}")

# Check if all input files exist
for file in "${input_files[@]}"; do
    if [ ! -f "$file" ]; then
        echo "Error: Input file '$file' does not exist."
        exit 1
    fi
done

# Process and output directly
{
    # Print the header line only once
    echo "dayTimestamp,hourTimestamp,eventID,Op1,Op2,Op3,Op4,Op5,Op6"

    # Combine, clean, sort, and process in a single step
    awk -F, 'FNR == 1 && NR != 1 { next } NR > 1 { gsub(/^[ \t]+|[ \t]+$/, ""); print }' "${input_files[@]}" | sort -t, -k1,1n -k2,2n | awk -F, '
    BEGIN {
        OFS = FS;
        current_day = -1;
    }
    {
        # Skip empty or invalid rows
        if (NF < 3) next;

        # Extract dayTimestamp
        day = $1;

        # For the very first row
        if (current_day == -1) {
            print day ",0,SOD";
        }

        # Handle day transitions
        if (day != current_day && current_day != -1) {
            print current_day ",23,EOD";
            print day ",0,SOD";
        }

        # Print the current row
        print $0;

        # Update current_day
        current_day = day;
    }
    END {
        # Add the final EOD
        if (current_day != -1) {
            print current_day ",23,EOD";
        }
    }'
} > "$output"

echo "Processed file created: $output"
