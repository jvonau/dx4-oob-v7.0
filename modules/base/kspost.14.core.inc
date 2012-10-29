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
            yumtmp.write('proxy_username %s\n' % auth_user)
            auth_pass = client.get_string('%s/authentication_password' % sys_dir)
            yumtmp.write('proxy_password %s\n' % auth_pass)
    else:
        pass
    yumtmp.close()

main()

EOF
chmod 755 /usr/bin/yumfix

# run the above from ~/.xsession
echo "/usr/bin/yumfix" > /home/olpc/.xsession

echo "include=/tmp/yum.conf.mod" >> /etc/yum.conf
