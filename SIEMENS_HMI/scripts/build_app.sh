#!/bin/bash
set -e
BSP=/root/EmbeddedLinuxNewBSP
TC=$BSP/buildroot/output/host/bin
PRJ=$BSP/SourceCode/09、Qt综合例程源码/HeatingSCADA

cd "$PRJ"
rm -f Makefile

${TC}qmake HeatingSCADA.pro
make -j4
echo "=== build done ==="
ls -la HeatingSCADA
