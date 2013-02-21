# disable hot-corners (some deployments begged us to do it, let's see how many will beg us to revert this)
gconftool-2 --direct --config-source=xml:readwrite:/etc/gconf/gconf.xml.defaults -s -t int /desktop/sugar/frame/corner_delay 1000


# enable ad-hoc icon
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type bool --set /desktop/sugar/network/adhoc true

# From http://sugardextrose.org/projects/dextrose/wiki/Gconf-keys
# gconf keys for dx4-international build

#sdxo#2662 Lease-Duration information in "My Settings" -> "About My Computer"
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/extensions/aboutcomputer/display_lease --type bool True

#sdxo#2669 System-Resources feature (gconf-controllable)
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/frame/show_network_resources --type bool True

#sdxo#2671 Add "nm-connection-editor" in "My Settings" -> "Network" (gconf-controllable)
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/extensions/network/show_nm_connection_editor --type bool True

#sdxo#2663 XO-Model-Number in "My Settings" -> "About My Computer"
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/extensions/aboutcomputer/display_model --type bool True

#sdxo#3047 Tam tam mini activity does not occupy full screen area
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/activities_not_requiring_osk_accumulation --type list --list-type string ['org.laptop.TamTamMini','org.laptop.TamTamEdit','org.laptop.TamTamJam','org.laptop.TamTamSynthLab']
