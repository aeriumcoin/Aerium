#!/bin/bash

set -e

date

#################################################################
# Update Ubuntu and install prerequisites for running Aerium   #
#################################################################
sudo apt-get update
#################################################################
# Build Aerium from source                                     #
#################################################################
NPROC=$(nproc)
echo "nproc: $NPROC"
#################################################################
# Install all necessary packages for building Aerium           #
#################################################################
sudo apt-get install -y qt4-qmake libqt4-dev libminiupnpc-dev libdb++-dev libdb-dev libcrypto++-dev libqrencode-dev libboost-all-dev build-essential libboost-system-dev libboost-filesystem-dev libboost-program-options-dev libboost-thread-dev libboost-filesystem-dev libboost-program-options-dev libboost-thread-dev libssl-dev libdb++-dev libssl-dev ufw git
sudo add-apt-repository -y ppa:bitcoin/bitcoin
sudo apt-get update
sudo apt-get install -y libdb4.8-dev libdb4.8++-dev

# By default, assume running within repo
repo=$(pwd)
file=$repo/src/aeriumd
if [ ! -e "$file" ]; then
	# Now assume running outside and repo has been downloaded and named aerium
	if [ ! -e "$repo/aerium/build.sh" ]; then
		# if not, download the repo and name it aerium
		git clone https://github.com/aeriumd/source aerium
	fi
	repo=$repo/aerium
	file=$repo/src/aeriumd
	cd $repo/src/
fi
make -j$NPROC -f makefile.unix

cp $repo/src/aeriumd /usr/bin/aeriumd

################################################################
# Configure to auto start at boot                                      #
################################################################
file=$HOME/.aerium
if [ ! -e "$file" ]
then
        mkdir $HOME/.aerium
fi
printf '%s\n%s\n%s\n%s\n' 'daemon=1' 'server=1' 'rpcuser=u' 'rpcpassword=p' | tee $HOME/.aerium/aerium.conf
file=/etc/init.d/aerium
if [ ! -e "$file" ]
then
        printf '%s\n%s\n' '#!/bin/sh' 'sudo aeriumd' | sudo tee /etc/init.d/aerium
        sudo chmod +x /etc/init.d/aerium
        sudo update-rc.d aerium defaults
fi

/usr/bin/aeriumd
echo "Aerium has been setup successfully and is running..."

