#!/bin/bash

# Copying sugar-client basic configuration to /etc/sugar-client.conf
cat >> "${INSTALL_ROOT}/etc/sugar-client.conf" << __EOF__
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
machine-uuid = 
# specify user's nickname instead of detecting it
nickname = anonymous
# school server's FQDN, will be auto-set after registration
schoolserver = 
# CA certificates file to get access to --stats-url via HTTPS
stats-ca-certs = 
# if more than 0 and --stats-url specified, ping stats server with specified
# delay in seconds to coordinate stats gathering work
stats-timeout = 600
# stats server's http(s) url to interact with; if omited, any stats related
# functionality is disabled
stats-url = http://localhost:8000
# current user's unique identity; needs to be set only from profile
# configuration file; will be auto-created on initial registration
uid = 
# optional command-line arguments to pass to a packager while updating the
# system
update-args = --disablerepo=* --enablerepo=dextrose --enablerepo=dextrose-next
# if more than 0, do regular unattended updates with specified delay in seconds
update-timeout = 86400
__EOF__


# Enabling sugar-client session at startup
cat >> "${INSTALL_ROOT}/home/olpc/.xsession" << __EOF__

# Enabling sugar-client session at sugar startup
sugar-client session

# Performing xmodmap customizations
xmodmap xmodmap.conf


__EOF__

