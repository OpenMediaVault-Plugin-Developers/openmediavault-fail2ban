openmediavault-fail2ban
========

To Build :

$ apt-get install build-essential fakeroot debhelper libfile-fcntllock-perl

$ git clone https://github.com/prbond/openmediavault-fail2ban

$ cd openmediavault-fail2ban

$ fakeroot debian/rules clean binary

$ dpkg -i openmediavault-fail2ban_0.1.0_all.deb

Depends :
fail2ban
python-central

How to install depends :
apt-get install fail2ban python-central
