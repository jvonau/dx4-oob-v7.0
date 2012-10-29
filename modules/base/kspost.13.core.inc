# Set microphone recording level
# don't think this is the right place JV
# might need to patch olpc-configure
echo "/usr/bin/amixer set \"Master Surround\" 31 unmute" > /etc/rc.d/rc.local
echo "/usr/bin/alsactl store" >> /etc/rc.d/rc.local

