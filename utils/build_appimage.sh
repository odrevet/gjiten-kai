# from https://docs.appimage.org/packaging-guide/from-source/native-binaries.html#using-the-build-system-to-build-the-basic-appdir
# linuxdeploy and appimagetool appimage need to be in PATH

cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCOMPILE_GSETTINGS_AT=share/glib-2.0/schemas
make
make install DESTDIR=AppDir
linuxdeploy-x86_64.AppImage --appdir AppDir -e bin/gjitenkai -d gjitenkai.desktop -i gjitenkai/kai.png
mv AppDir/usr/bin/gjitenkai AppDir/usr/bin/gjitenkai_bin
cp gjitenkai_wrapper.sh AppDir/usr/bin/gjitenkai
#sudo chmod -R 777 AppDir
#sudo chmod 777 AppDir/AppRun AppDir/usr/bin/gjitenkai
ARCH=x86_64 appimagetool-x86_64.AppImage AppDir
