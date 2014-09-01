
#!/bin/sh
(( ${#} > 0 )) || {
  echo ''
  echo '*** Qucs uninstaller script for Machintosh ***'
  echo ''
  echo 'DISCLAIMER: USE THIS SCRIPT AT YOUR OWN RISK!'
  echo 'THE AUTHOR TAKES NO RESPONSIBILITY FOR THE RESULTS OF THIS SCRIPT.'
  echo "Disclaimer aside, this worked for the author, for what that's worth."
  echo 'Press Control-C to quit now.'
  echo 'You may need to use as sudo.'
  read
  echo 'Re-running the script with sudo.'
  echo 'You may be prompted for a password.'
  sudo ${0} sudo
  exit $?
}


# bill of materials
packages=(
/var/db/receipts/org.qucs.pkg.bom
)

# clear files from packages
for bom in "${packages[@]}"; do
  if [ ! -e ${bom} ]; then
    echo "bom file not found: ${bom}"
  else
    #echo "found ${bom}"
    lsbom -f -l -s -pf ${bom} \
    | while read i; do
      # Remove each file listed in the bom.
      cd /
      file="${i}"
      if [ -e $file ]; then
        echo "removing: ${file}"
        rm $file
      else
        echo "file not found: ${file}"
      fi
    done
  fi
done


## remove symlinks if found
# final location
PREFIX='/usr/local/bin'

# list of programs to link into /Applications
PROGS="
  qucs.app
  qucsattenuator.app
  qucsedit.app
  qucsfilter.app
  qucshelp.app
  qucslib.app
  qucsrescodes.app
  qucstrans.app"

# link programs
for app in $PROGS
do

  link=/Applications/$app
  if [ -L $link ]; then
    rm $link
  fi
done


#echo "*** Removing receipts: /var/db/receipts/org.qucs.*"
#rm -f /var/db/receipts/org.qucs.*


