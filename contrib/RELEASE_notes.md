


Recommended sequence:

* bump version

* review commiters, acknowledgements, translators

* git tag

* push tags to remote


* create source release: `qucs_release.sh`
  * upload tarball to SF

* cross-compile Win32: `qucs_build_win32.sh`

  * [optional] repackage compiler: `mingw-w64-installer.sh`
    * upload installer to SF

  * [optional] rebuild/repackage FreeHDL: `freehdl-build-win32.sh`
    * [manual step] build GNU regex, deploy regex.h libregex.a into mingw tree
    * `freehdl-installer.sh`
    * upload instaler to SF

* package Win32 build: `qucs_installer_win32.sh`
  * upload installer to SF

