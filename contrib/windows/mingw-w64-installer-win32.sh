

# simply repackage the original upstream package

WINDIR=${HOME}/.wine/drive_c/mingw_original


# Download/extract MinGW 4.8.2
mingw=i686-4.8.2-release-posix-dwarf-rt_v3-rev3.7z
if [ -e ${HOME}/Downloads/$mingw ]; then
  echo Extract MinGW...
  7z x ${HOME}/Downloads/$mingw -o${WINDIR}
else
  echo Downloading Mingw...
  wget http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.2/threads-posix/dwarf/$mingw -P ${HOME}/Downloads/
fi


# copy misc
mkdir $WINDIR/mingw32/misc
cp ${HOME}/git/qucs/qucs/contrib/innosetup/misc/mingw.url $WINDIR/mingw32/misc/

# copy gpl.rtf
cp ${HOME}/git/qucs/qucs/contrib/innosetup/gpl.rtf $WINDIR/mingw32/

# package mingw-w64 

INNOSETUP="${HOME}/.wine/drive_c/Program Files/Inno Setup 5/Compil32.exe"

echo Create Inno Setup installer...
cp  ${HOME}/git/qucs/qucs/contrib/innosetup/mingw-w64.iss .
cp  ${HOME}/git/qucs/qucs/contrib/innosetup/modpath.iss .
wine "$INNOSETUP" /cc mingw-w64.iss
rm mingw-w64.iss
rm modpath.iss

# TODO version is hardcoded on the Inno Setup file
echo Copy mingw installer to ~/Downloads/
cp Output/mingw-w64-*-setup.exe ${HOME}/Downloads/
#rm -r release/Output

echo Done!


