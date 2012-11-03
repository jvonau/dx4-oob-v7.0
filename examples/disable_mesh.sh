#!/bin/bash

# uy#1671: Pass "libertas_disablemesh" parameter to "libertas" KLM.
#          This would disable mesh at the hardware level.
#
#          One thing to note, that the kernel package installed must be
#          kernel-2.6.35.13_xo1-20120502.1603.olpc.bde819f.i586
filename="${INSTALL_ROOT}/etc/modprobe.d/libertas.conf"
echo "options libertas libertas_disablemesh=1" > ${filename}
