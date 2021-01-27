#! /bin/zsh

# Get IDF, create output folder
get_idf
mkdir output

# Backup sdkconfig
cp sdkconfig sdkconfig.bak

# Generate EngineSense firmware
cp esconfig sdkconfig
idf.py build
cp build/TractorKit-Peripheral-FW.bin output/ES.bin

# Generate Navis firmware
cp naconfig sdkconfig
idf.py build
cp build/TractorKit-Peripheral-FW.bin output/NA.bin

# Restore sdkconfig
mv sdkconfig.bak sdkconfig