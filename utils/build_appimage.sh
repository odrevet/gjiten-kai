# from https://docs.appimage.org/packaging-guide/from-source/native-binaries.html#using-the-build-system-to-build-the-basic-appdir
# linuxdeploy and appimagetool appimage need to be in PATH

cmake . -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install DESTDIR=AppDir
linuxdeploy-x86_64.AppImage --appdir AppDir -e bin/gjitenkai -d gjitenkai.desktop -i gjitenkai/kai.png
appimagetool-x86_64.AppImage AppDir
