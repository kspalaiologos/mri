#!/bin/bash
set -e

gcc regiond.c -o regiond -O3 -lz

cp -r "$1" "$1-tmp"

truncate -s0 "$1-tmp/mri-regions.txt"
truncate -s0 "$1-tmp/mri-data.txt"

for f in $(find "$1-tmp"); do
    echo "Processing $f"
    
    # Handle .mca files:
    if [[ "$f" == *.mca ]]; then
        # strip 1st path element in f
        echo "${f#*/}.mri" >> "$1-tmp/mri-regions.txt"
        ./regiond c "$f" "$f.mri"
        rm -f $f
    fi

    # Check if `file` reports the contents as gzip-compressed:
    if [[ $(file "$f") == *"gzip compressed"* ]]; then
        echo "${f#*/}.ung" >> "$1-tmp/mri-data.txt"
        gzip -d < "$f" > "$f.ung"
        rm -f $f
    fi
done

echo "OK. Compressing..."

cd $1-tmp && tar -c . | bzip3 -c -f -e -b 511 > "../$1.mri.bz3" && cd ..
rm -rf "$1-tmp"
