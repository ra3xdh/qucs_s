#!/bin/bash

## file: tarball_sign.sh
#
## This script signs the tarball.

# TODO
## Sign the package in: ~/git/qucs/release/qucs-x.x.x.tar.gz



# Create Key:
# http://www.cyberciti.biz/tips/linux-how-to-create-our-own-gnupg-privatepublic-key.html


# Verify integrity:
# http://www.cyberciti.biz/tips/howto-verify-integrity-of-the-tar-balls-with-gpg-command.html




#!/bin/bash
source gpg_id
echo "using gpg id: $GPG_ID"
echo "Press any key..."
read
cd ../..




DISTS="saucy trusty"
cd qucs-$RELEASE
./configure
cd ..
tar -zcvhf qucs_$RELEASE.orig.tar.gz qucs-$RELEASE
#cp qucs-$RELEASE.tar.gz qucs_$RELEASE.orig.tar.gz

cd qucs-$RELEASE
COUNT=-0 #last version number in repository
for DIST in ${DISTS} ; do
	COUNT=$(($COUNT-1))
	dch -D $DIST -m -v $RELEASE$COUNT -b
	debuild -S -k$GPG_ID
	./configure
done

