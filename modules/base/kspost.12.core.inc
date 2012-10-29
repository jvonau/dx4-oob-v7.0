# sl#3267, enable ctrl+alt+erase to restart sugar for all xo-[1,1.5,1.75] laptops
sed -i -e '/DontZap/d' /usr/share/olpc-utils/xorg.conf.d/xo1.conf
sed -i -e '/DontZap/d' /usr/share/olpc-utils/xorg.conf.d/xo1.5.conf
sed -i -e '/DontZap/d' /usr/share/olpc-utils/xorg.conf.d/xo1.75.conf
