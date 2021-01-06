#! /bin/zsh

get_idf

cp esconfig sdkconfig
idf.py build
cp build/TractorKit-Peripheral-FW.bin build/output/ES.bin

cp naconfig sdkconfig
idf.py build
cp build/TractorKit-Peripheral-FW.bin build/output/NA.bin