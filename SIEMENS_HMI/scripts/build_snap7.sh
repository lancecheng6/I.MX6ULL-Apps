#!/bin/bash
# Build snap7 as ARMv7 static library using buildroot toolchain
set -e

BSP=/root/EmbeddedLinuxNewBSP
TC=$BSP/buildroot/output/host/bin/arm-buildroot-linux-gnueabihf-
PRJ=$BSP/SourceCode/09、Qt综合例程源码/HeatingSCADA
SRC=$PRJ/snap7/vendor/snap7-full-1.4.2/src
OUT=$PRJ/snap7

SRCS="$SRC/sys/snap_msgsock.cpp $SRC/sys/snap_sysutils.cpp \
$SRC/sys/snap_tcpsrvr.cpp $SRC/sys/snap_threads.cpp \
$SRC/core/s7_client.cpp $SRC/core/s7_isotcp.cpp $SRC/core/s7_partner.cpp \
$SRC/core/s7_peer.cpp $SRC/core/s7_server.cpp $SRC/core/s7_text.cpp \
$SRC/core/s7_micro_client.cpp $SRC/lib/snap7_libmain.cpp"

INC="-I$SRC/sys -I$SRC/core -I$SRC/lib"

mkdir -p "$OUT"
OBJDIR=$(mktemp -d)

for f in $SRCS; do
    echo "CC  $(basename $f)"
    ${TC}g++ -O2 -fPIC $INC -c "$f" -o "$OBJDIR/$(basename ${f%.cpp}).o"
done

echo "AR  libsnap7.a"
${TC}ar rcs "$OUT/libsnap7.a" "$OBJDIR"/*.o
cp -f "$ROOT/release/Wrappers/c-cpp/snap7.h" "$OUT/snap7.h" 2>/dev/null || cp -f /tmp/snap7_src/snap7-full-1.4.2/release/Wrappers/c-cpp/snap7.h "$OUT/snap7.h"

rm -rf "$OBJDIR"
echo "=== result ==="
ls -la "$OUT/"
