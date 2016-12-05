openmediavault-fail2ban
========

To Build :

$ apt-get install build-essential fakeroot debhelper libfile-fcntllock-perl

$ git clone https://github.com/OpenMediaVault-Plugin-Developers/openmediavault-fail2ban.git

$ cd openmediavault-fail2ban

$ fakeroot debian/rules clean binary

$ dpkg -i openmediavault-fail2ban_X.X.X_all.deb

How to install depends :
apt-get install -f
