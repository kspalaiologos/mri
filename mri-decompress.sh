#!/bin/bash
set -e

gcc regiond.c -o regiond -O3 -lz

mkdir "$1" && cd "$1" && bzip3 -d -c < "../$1.mri.bz3" | tar -vx && cd ..

# Process the .mri region data.
cat "$1/mri-regions.txt" | while read mri; do
    # Remove the .mri suffix
    region="${mri%.mri}"
    echo "Processing $region"
    ./regiond d "$1/$mri" "$1/$region"
    rm -f "$1/$mri"
done
rm -f "$1/mri-regions.txt"

# Process the gzipped data.
cat "$1/mri-data.txt" | while read mri; do
    # Remove the .ung suffix
    region="${mri%.ung}"
    echo "Processing $region"
    gzip -6 < "$1/$mri" > "$1/$region"
    rm -f "$1/$mri"
done
rm -f "$1/mri-data.txt"

echo "OK."
