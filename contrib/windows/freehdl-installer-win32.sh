
# create a FreeHDL installer for Win32

WINDIR=${HOME}/.wine/drive_c/freehdl_cross


# copy libregex.a into
cp ${HOME}/.wine/drive_c/mingw32/lib/libregex.a $WINDIR/lib

# copy misc

mkdir $WINDIR/misc
cp ${HOME}/git/qucs/qucs/contrib/innosetup/misc/website.url $WINDIR/misc/

# copy gpl.rtf
cp ${HOME}/git/qucs/qucs/contrib/innosetup/gpl.rtf $WINDIR/

# package freehdl.iss

INNOSETUP="${HOME}/.wine/drive_c/Program Files/Inno Setup 5/Compil32.exe"

echo Create Inno Setup installer...
cp  ${HOME}/git/qucs/qucs/contrib/innosetup/freehdl.iss .
wine "$INNOSETUP" /cc freehdl.iss
rm freehdl.iss

# TODO version is hardcoded on the Inno Setup file
cp Output/freehdl-*-setup.exe ${HOME}/Downloads/
#rm -r release/Output


echo Done!



