# Disable "requiretty" for "wheel" group, so that "sudo" commands are be able to run in a terminal-less environment.
echo "Defaults:%wheel !requiretty" >> /etc/sudoers

