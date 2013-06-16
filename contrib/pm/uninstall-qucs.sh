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

# receipts for /usr/local/bin
bin=(
/var/db/receipts/org.qucs.qucs.asco.pkg.bom
/var/db/receipts/org.qucs.qucs.ps2sp.pkg.bom
/var/db/receipts/org.qucs.qucs.qucs-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucs.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsator.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsattenuator-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsattenuator.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsconv.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsdigi.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsedit-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsedit.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsfilter.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsfilter-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucshelp-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucshelp.pkg.bom
/var/db/receipts/org.qucs.qucs.qucslib-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucslib.pkg.bom
/var/db/receipts/org.qucs.qucs.qucstrans-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucstrans.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsveri.pkg.bom
)

# receipts for /usr/local/share/qucs/
share=(
/var/db/receipts/org.qucs.qucs.qucs-2.pkg.bom
)

# receipts for /usr/local/share/man/man1/
man=(
/var/db/receipts/org.qucs.qucs.qucsattenuator-2.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsator-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsedit-2.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsdigi-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsconv-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsfilter-2.pkg.bom
/var/db/receipts/org.qucs.qucs.qucshelp-2.pkg.bom
/var/db/receipts/org.qucs.qucs.qucslib-2.pkg.bom
/var/db/receipts/org.qucs.qucs.qucstrans-2.pkg.bom
/var/db/receipts/org.qucs.qucs.qucsveri-1.pkg.bom
/var/db/receipts/org.qucs.qucs.qucs-3.pkg.bom
)

# Verify the bom exists, otherwise don't do anything
[ -e  /var/db/receipts/org.qucs.qucs.qucs.pkg.bom ] || {
  echo 'Qucs bill of materials not found!'
  exit 0
}

# clear /usr/local/bin/
for bom in "${bin[@]}"; do
  #echo "${bom}"
  lsbom -f -l -s -pf ${bom} \
  | while read i; do
    # Remove each file listed in the bom.
    file="/usr/local/bin/${i}"
    if [ -e $file ]; then
      echo "removing: ${file}"
      rm $file
    else
      echo "file not found: ${file}"
    fi
  done
done

# Remove app directories
apps=(
qucs.app
qucsattenuator.app
qucsedit.app
qucsfilter.app
qucshelp.app
qucslib.app
qucstrans.app
)

for dir in "${apps[@]}"; do
  echo "*** Removing directory: /usr/local/bin/${dir}"
  rm -rf /usr/local/bin/${dir}
done

# clear /usr/local/share/qucs/
for bom in "${share[@]}"; do
  #echo "${bom}"
  lsbom -f -l -s -pf ${bom} \
  | while read i; do
    # Remove each file listed in the bom.
    file="/usr/local/share/qucs/${i}"
    if [ -e $file ]; then
      echo "removing: ${file}"
      rm $file
    else
      echo "file not found: ${file}"
    fi
  done
done

# clear /usr/local/share/man/man1/
for bom in "${man[@]}"; do
  #echo "${bom}"
  lsbom -f -l -s -pf ${bom} \
  | while read i; do
    # Remove each file listed in the bom.
    file="/usr/local/share/man/man1/${i}"
    if [ -e $file ]; then
      echo "removing: ${file}"
      rm $file
    else
      echo "file not found: ${file}"
    fi
  done
done

# Remove directories and files related to qucs
echo "*** Removing directory: /usr/local/share/qucs/"
rm -rf /usr/local/share/qucs

echo "*** Removing receipts: /var/db/receipts/org.qucs.*"
rm -f /var/db/receipts/org.qucs.*

echo "*** Removing alias: /Application/qucs"
rm -f /Application/qucs


# Remove other installed packages
echo "*** Removing optional packages..."

packages=(
/var/db/receipts/org.icarus-verilog.pkg.bom
)

# clear files on flat package
for bom in "${packages[@]}"; do
  if [ ! -e ${bom} ]; then
    echo "bom file not found: ${bom}"
  else
    #echo "found ${bom}"
    lsbom -f -l -s -pf ${bom} \
    | while read i; do
      # Remove each file listed in the bom.
      file="/usr/local/${i}"
      if [ -e $file ]; then
        echo "removing: ${file}"
        rm $file
      else
        echo "file not found: ${file}"
      fi
    done
  fi
done

echo "*** Removing receipts: /var/db/receipts/org.icarus-verilog.*"
rm -f /var/db/receipts/org.icarus-verilog.*
rm -rf /usr/local/include/iverilog
rm -rf /usr/local/lib/ivl

exit 0

