
#au#704: add permissions for '/etc/ntp/step-tickers' file
chmod 0777 /etc/ntp
chmod 0777 /etc/ntp/step-tickers
# also, make the file empty.
echo > /etc/ntp/step-tickers

# uy#1375 : Utility Functions.
SearchAndReplaceRegex()
{
       cat $1 | sed "s/$2/$3/g" > temp
       cat temp > $1
}

SearchAndDeleteLineContainingRegex()
{
       cat $1 | sed "/$2/d" > temp
       cat temp > $1
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

# Some necessary tweaks in "httpd" service.
#
rm /etc/httpd/logs
mkdir /etc/httpd/logs

# Generate the ssl-key and certificate.
# Note that initially, all XOs will be having the same pair
# (since the same image will be installled on all XOs).
#
# However, a new unique pair will be generated per XO, once the user
# creates her sugar-profile (on first startup of the XO).
mkdir -p /home/olpc/.sugar/default
openssl req -new -newkey rsa:1024 -days 365 -nodes -x509 -subj "/C=US/ST=Denial/L=Springfield/O=Dis/CN=www.example.com" -keyout /home/olpc/.sugar/default/ssl.key -out /home/olpc/.sugar/defaul

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

