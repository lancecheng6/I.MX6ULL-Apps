#!/bin/bash
# Cross-build s7_test for ATK-IMX6U
set -e
BSP=/root/EmbeddedLinuxNewBSP
TC=$BSP/buildroot/output/host/bin/arm-buildroot-linux-gnueabihf-
PRJ=$BSP/SourceCode/09、Qt综合例程源码/HeatingSCADA

${TC}g++ -O2 -I"$PRJ/snap7" \
    "$PRJ/test/s7_test.cpp" \
    "$PRJ/snap7/libsnap7.a" \
    -lpthread -lrt \
    -o "$PRJ/test/s7_test"

echo "=== result ==="
${TC}size "$PRJ/test/s7_test"
${TC}readelf -h "$PRJ/test/s7_test" | grep -E "Class|Machine"
ls -la "$PRJ/test/"
