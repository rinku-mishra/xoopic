#!/bin/sh

echo "dispersion execution script"
echo "inputs: 2d_dispersion_split_E5.py, 2d_dispersion_split_E15.py, 2d_dispersion_split_E25.py"

echo "processing E5......"
python3 2d_dispersion_split_E5.py
echo "E5 process completed"

echo "processing E15......"
python3 2d_dispersion_split_E15.py
echo "E15 process completed"

# echo "processing E25......"
# python3 2d_dispersion_split_E25.py
# echo "E25 process completed"

echo "all completed"

