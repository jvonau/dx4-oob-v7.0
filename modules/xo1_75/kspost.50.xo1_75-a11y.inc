#Install a11y packages
# There is a problem during yum phase due to dependencies

/bin/rpm -ivh http://build.activitycentral.com/dextrose4-extra/RPMS/armv7hl/ax-1.0-1.uy.fc17.armv7hl.rpm
/bin/rpm -ivh http://build.activitycentral.com/dextrose4-extra/RPMS/armv7hl/magnifier-Ceibal-3.3.2-1.1.uy.fc17.armv7hl.rpm
/bin/rpm -ivh http://build.activitycentral.com/dextrose4-extra/RPMS/noarch/CapitalFont-1.0-1.uy.noarch.rpm
/bin/rpm -ivh http://build.activitycentral.com/dextrose4-extra/RPMS/noarch/FlatbedCursors-0.2-1.fc14.1.uy.noarch.rpm

# We install the same version than sugar-cp-frame
A11YRPM=`rpm -qa | grep sugar-cp-frame | sed -e 's/frame/accessibility/g'`
/bin/rpm -ivh http://build.activitycentral.com/devel-daily/noarch/"$A11YRPM".rpm
