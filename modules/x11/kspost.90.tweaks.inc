# Disable "requiretty" for "wheel" group, so that "sudo" commands are be able to run in a terminal-less environment.
echo "Defaults:%wheel !requiretty" >> /etc/sudoers

# sl#3267, enable ctrl+alt+erase to restart sugar for all xo-[1,1.5,1.75] laptops
sed -i -e '/DontZap/d' /usr/share/olpc-utils/xorg.conf.d/xo1.conf
sed -i -e '/DontZap/d' /usr/share/olpc-utils/xorg.conf.d/xo1.5.conf
sed -i -e '/DontZap/d' /usr/share/olpc-utils/xorg.conf.d/xo1.75.conf

# Set microphone recording level
# don't think this is the right place JV
# might need to patch olpc-configure
echo "/usr/bin/amixer set \"Master Surround\" 31 unmute" > /etc/rc.d/rc.local
echo "/usr/bin/alsactl store" >> /etc/rc.d/rc.local

cat << EOF > /usr/bin/yumfix
#!/usr/bin/env python
# Copyright (C) 2011, OLPC-AU.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

import os
import sys
import string
import gettext
import gconf

def main():
    

    ## JV export http_proxy from .gconf
    yumtmp = open("/tmp/yum.conf.mod", "w")
    if not os.path.exists('/home/olpc/.gconf/system/proxy'):
        yumtmp.close()
        sys.exit(1)
    sys_dir = '/system/http_proxy'
    client = gconf.client_get_default()
    use_proxy = client.get_string('/system/proxy/mode')
    if use_proxy != 'none':
        http_host = client.get_string('%s/host' % sys_dir)
        http_port = client.get_int('%s/port' % sys_dir)
        proxy_info = '%s:%d' % (http_host, http_port)
        yumtmp.write('proxy=http://%s\n' % proxy_info)
        use_auth = client.get_bool('%s/use_authentication' % sys_dir)
        if use_auth is True:
            auth_user = client.get_string('%s/authentication_user' % sys_dir)
            yumtmp.write('proxy_username=%s\n' % auth_user)
            auth_pass = client.get_string('%s/authentication_password' % sys_dir)
            yumtmp.write('proxy_password=%s\n' % auth_pass)
    else:
        pass
    yumtmp.close()

main()

EOF
chmod 755 /usr/bin/yumfix

# run the above from ~/.xsession
echo "/usr/bin/yumfix" > /home/olpc/.xsession

echo "include=/tmp/yum.conf.mod" >> /etc/yum.conf

#au#704: add permissions for '/etc/ntp/step-tickers' file
mkdir -p /etc/ntp
echo > /etc/ntp/step-tickers
chmod 0777 /etc/ntp/step-tickers /etc/ntp

# uy#1375 : Utility Functions.
SearchAndReplaceRegex()
{
       sed "s/$2/$3/g" "$1" -i
}

SearchAndDeleteLineContainingRegex()
{
       sed "/$2/d" "$1" -i
}

# uy#1375 (a): Append to "/home/olpc/.xsession"
#              Done in "config/sugar-client-conf.sh"

# uy#1375 (b): Add "/home/olpc/xmodmap.conf"
filename="/home/olpc/xmodmap.conf"
echo "keycode 224 = F13" >> $filename

# uy#1375 (c): Modify "/usr/share/X11/xkb/keycodes/evdev"
filename="/usr/share/X11/xkb/keycodes/evdev"
SearchAndDeleteLineContainingRegex $filename " *<LWIN> = 133; *"
SearchAndReplaceRegex $filename "<CAPS> = 66;" "<CAPS> = 133;"

# uy#1769 : Set up "httpd", to host "WebDAV" shares.
#
#
# Make the directories (if not already), and set the permissions.
#
mkdir -p /var/www/web1/web/.Sugar-Metadata
chmod -R 0777 /var/www/web1/

# From Fedora-18 onwards, we also need to specify a "lock"-file, to be used
# by "DavLockDB" directive in httpd-virtualhost(s).
mkdir -p /var/www/lock
touch /var/www/lock/lock_file
chmod -R 0777 /var/www/lock

# Some necessary tweaks in "httpd" service.
#
rm /etc/httpd/logs -f
mkdir /etc/httpd/logs -p

# Generate the ssl-key and certificate.
# Note that initially, all XOs will be having the same pair
# (since the same image will be installled on all XOs).
#
# However, a new unique pair will be generated per XO, once the user
# creates her sugar-profile (on first startup of the XO).
mkdir -p /home/olpc/.sugar/default
openssl req -new -newkey rsa:1024 -days 365 -nodes -x509 -subj "/C=US/ST=Denial/L=Springfield/O=Dis/CN=www.example.com" -keyout /home/olpc/.sugar/default/ssl.key -out /home/olpc/.sugar/default/ssl.crt

# Replace the key- and crt-path in conf-file, so that secure-transfer may be enabled.
#
filename="/etc/httpd/conf.d/ssl.conf"
SearchAndReplaceRegex $filename "SSLCertificateKeyFile \/etc\/pki\/tls\/private\/localhost.key" "SSLCertificateKeyFile \/home\/olpc\/.sugar\/default\/ssl.key"
SearchAndReplaceRegex $filename "SSLCertificateFile \/etc\/pki\/tls\/certs\/localhost.crt" "SSLCertificateFile \/home\/olpc\/.sugar\/default\/ssl.crt"

# Create the password file for WebDAV.
#
htpasswd -bc /var/www/web1/passwd.dav test olpc

# Finally, configure "VirtualHost".
filename="/etc/httpd/conf.d/ssl.conf"
SearchAndDeleteLineContainingRegex $filename "<\/VirtualHost>"
cat << EOF >> /etc/httpd/conf.d/ssl.conf

        DocumentRoot /var/www/web1/web
        <Directory /var/www/web1/web/>
                Options Indexes MultiViews
                AllowOverride None
                Order allow,deny
                allow from all
        </Directory>

        Alias /webdav /var/www/web1/web
        DavLockDB /var/www/lock/lock_file

        <Location /webdav>
            DAV On
            AuthType Basic
            AuthName "webdav"
            AuthUserFile /var/www/web1/passwd.dav
            Require valid-user
        </Location>
</VirtualHost>
EOF

# Change the startup preferences of services.
#
/sbin/chkconfig httpd --levels 5 on

# uy#1769 customizations end.


# uy#1792: Have a documents folder (if not already,
#         as is the case in non-gnome image).
#
#          Note that, this folder needs to be registered,
#          to be recognized as a special folder.
if [ ! -d "/home/olpc/Documents" ]; then
    mkdir "/home/olpc/Documents"
    xdg-user-dirs-update
    chmod -R 0777 /home/olpc/Documents
fi


# au#1588: Need to make soft-links, to join library-versions.
ln -s /usr/lib/libssl.so.10    /usr/lib/libssl.so.8
ln -s /usr/lib/libcrypto.so    /usr/lib/libcrypto.so.8


# disable login sound
rm /usr/share/gnome/autostart/libcanberra-login-sound.desktop


# disable hot-corners (some deployments begged us to do it, let's see how many will beg us to revert this)
gconftool-2 --direct --config-source=xml:readwrite:/etc/gconf/gconf.xml.defaults -s -t int /desktop/sugar/frame/corner_delay 1000


# enable ad-hoc icon 
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type bool --set /desktop/sugar/network/adhoc true

# From http://sugardextrose.org/projects/dextrose/wiki/Gconf-keys
# gconf keys for dx4-international build

#2662 Lease-Duration information in "My Settings" -> "About My Computer" 
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/extensions/aboutcomputer/display_lease --type bool True

#2669 System-Resources feature (gconf-controllable)
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/frame/show_network_resources --type bool True

#2671 Add "nm-connection-editor" in "My Settings" -> "Network" (gconf-controllable)
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/extensions/network/show_nm_connection_editor --type bool True

#2663 XO-Model-Number in "My Settings" -> "About My Computer" 
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --set /desktop/sugar/extensions/aboutcomputer/display_model --type bool True





# Create an alternative tmp dir that is not in ramdisk, empty on every boot
echo "rm -rf /usr/local/tmp" >> $INSTALL_ROOT/etc/rc.local
echo "mkdir /usr/local/tmp" >> $INSTALL_ROOT/etc/rc.local
echo "chmod 1777 /usr/local/tmp" >> $INSTALL_ROOT/etc/rc.local

# The main fedora yum repo metadata should never expire
sed 's/metadata_expire.*/metadata_expire=never/' /etc/yum.repos.d/fedora.repo -i

# sugar-client config file customizations
cat << EOF > /etc/sugar-client.conf
[main]
# url to connect to school server's RESTfull API
api-url = http://schoolserver:8000
# process auto-registeration on a school server on startup; multiple argument,
# if mentioned more than once and previously registered server's IP is different
# from "schoolserver"'s one, process re-registrtion
auto-register = 0
# if, after registration, there are backups to restore on the server, process
# restore automatically
auto-restore = False
# if more than 0, do regular backups with specified delay in seconds
backup-timeout = 0
# rsync url to backup/restore Journal
backup-url =
# debug logging level; multiple argument
debug = 0
# jabber server for Sugar collaboraiton
jabber-url =
# specify machine's serial number instead of detecting it
machine-sn = anonymous
# specify machine's UUID instead of detecting it
machine-uuid = anonymous
# specify user's nickname instead of detecting it
nickname = anonymous
# school server's FQDN, will be auto-set after registration
schoolserver =
# CA certificates file to get access to --stats-url via HTTPS
stats-ca-certs =
# if more than 0 and --stats-url specified, ping stats server with specified
# delay in seconds to coordinate stats gathering work
stats-timeout = 300
# stats server's http(s) url to interact with; if omited, any stats related
# functionality is disabled
stats-url = http://stats.sugardextrose.org:8000
# current user's unique identity; needs to be set only from profile
# configuration file; will be auto-created on initial registration
uid =
# optional command-line arguments to pass to a packager while updating the
# system
update-args = --disablerepo=* --enablerepo=dextrose
# if more than 0, do regular unattended updates with specified delay in seconds
update-timeout = 86400

EOF
echo "sugar-client session" >> /home/olpc/.xsession

sed '1s/env\ //' /usr/bin/sugar-client* -i

# Just making sure we didn't mess up ownerships
chown olpc.olpc /home/olpc -R
chown root.root /
chmod 1755 /
chmod 755 /etc/init.d/*
# ADD THINGS BEFORE THIS BLOCK
