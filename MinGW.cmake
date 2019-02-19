option(NSIS "Use NSIS generator to produce installer" OFF)
option(STANDALONE "Install as standalone" OFF)
option(ICONS "Install GTK icons" ON)

#NSIS packages are standalone
if(NSIS)
  set (STANDALONE ON)
endif()

# Add MINGW flag to use in source code
add_definitions(-DMINGW)
#prevent prompt to open
SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")

#set the Windows executable icon with windres and a rc file Windows
set(RES_FILES "resources/icon.rc")
set(CMAKE_RC_COMPILER_INIT windres)
ENABLE_LANGUAGE(RC)
set(CMAKE_RC_COMPILE_OBJECT
  "<CMAKE_RC_COMPILER> -O coff -i <SOURCE> -o <OBJECT>")

set(OUTPUT_BIN ${OUTPUT_BIN}.exe)

if(STANDALONE)
  if(NSIS)
    set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME}.exe)
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)
    include (NSIS.cmake)
  endif()

  #install the Gtk DLLs from the Gtk's bin directory
  set (GTK_BIN ${GLIB_PREFIX}/bin)

  #these are all the required dll for GTK
  set (GTK_LIBS
    ${GTK_BIN}/libatk-1.0-0.dll
    ${GTK_BIN}/libfontconfig-1.dll
    ${GTK_BIN}/libgdk-3-0.dll
    ${GTK_BIN}/liblzma-5.dll
    ${GTK_BIN}/libcairo-2.dll
    ${GTK_BIN}/libcairo-gobject-2.dll
    ${GTK_BIN}/libffi-6.dll
    ${GTK_BIN}/libfreetype-6.dll
    ${GTK_BIN}/libfribidi-0.dll
    ${GTK_BIN}/libgdk_pixbuf-2.0-0.dll
    ${GTK_BIN}/libgio-2.0-0.dll
    ${GTK_BIN}/libglib-2.0-0.dll
    ${GTK_BIN}/libgmodule-2.0-0.dll
    ${GTK_BIN}/libgobject-2.0-0.dll
    ${GTK_BIN}/libgtk-3-0.dll
    ${GTK_BIN}/libiconv-2.dll
    ${GTK_BIN}/libintl-8.dll
    ${GTK_BIN}/libpango-1.0-0.dll
    ${GTK_BIN}/libpango-1.0-0.dll
    ${GTK_BIN}/libpangocairo-1.0-0.dll
    ${GTK_BIN}/libpangoft2-1.0-0.dll
    ${GTK_BIN}/libpangowin32-1.0-0.dll
    ${GTK_BIN}/libpixman-1-0.dll
    ${GTK_BIN}/libpng16-16.dll
    ${GTK_BIN}/libxml2-2.dll
    ${GTK_BIN}/zlib1.dll
    ${GTK_BIN}/libcroco-0.6-3.dll
    ${GTK_BIN}/librsvg-2-2.dll
    ${GTK_BIN}/libsoup-2.4-1.dll
    ${GTK_BIN}/libpcre-1.dll
    ${GTK_BIN}/libgraphite2.dll
    ${GTK_BIN}/libsqlite3-0.dll
	)

  install(
    PROGRAMS ${GTK_LIBS}
    DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT System
    )

  #install WINGW runtime libs
  get_filename_component( MINGW_BIN ${CMAKE_C_COMPILER} DIRECTORY )

 if(${MINGW_BIN} MATCHES mingw64)
    set(LIB_GCC libgcc_s_seh-1.dll)
  else()
    set(LIB_GCC libgcc_s_dw2-1.dll)
  endif()

  set(RUNTIME_LIBS
    ${MINGW_BIN}/${LIB_GCC}
    ${MINGW_BIN}/libwinpthread-1.dll
    ${MINGW_BIN}/libbz2-1.dll
    ${MINGW_BIN}/libepoxy-0.dll
    ${MINGW_BIN}/libexpat-1.dll
    ${MINGW_BIN}/libharfbuzz-0.dll
    ${MINGW_BIN}/libstdc++-6.dll)

  install( PROGRAMS ${RUNTIME_LIBS} DESTINATION bin COMPONENT System )

  #compile gschema with required gtk settings in a temporary directory
  #set the tmp directory name
  set(TMP_SCHEMADIR $ENV{TMP}/${PROJECT_NAME})
  #create tmp directory
  file(MAKE_DIRECTORY ${TMP_SCHEMADIR})
  #copy application schema and gtk schemas to the tmp directory
  file(COPY ${SOURCE_SHARE}/${GLIB_SCHEMADIR}/${PROJECT_NAME}.gschema.xml
    ${GLIB_PREFIX}/${SCHEMADIR}/org.gtk.Settings.ColorChooser.gschema.xml
    ${GLIB_PREFIX}/${SCHEMADIR}/org.gtk.Settings.FileChooser.gschema.xml
    DESTINATION ${TMP_SCHEMADIR})
  #compile schemas in tmp directory, output in local schemadir
  compile_schemas(${TMP_SCHEMADIR} ${SCHEMADIR})

  #install compiled gschema in a directory relative to the binary
  install(
    FILES ${SCHEMADIR}/gschemas.compiled
    DESTINATION ${SCHEMADIR}
    )

  if(ICONS)
    #install icons
    install(
      DIRECTORY ${GLIB_PREFIX}/share/icons/Adwaita/scalable/
      DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/Adwaita/scalable
      )

    install(
      FILES
      ${GLIB_PREFIX}/share/icons/Adwaita/index.theme
      ${GLIB_PREFIX}/share/icons/Adwaita/icon-theme.cache
      DESTINATION
      ${CMAKE_INSTALL_DATADIR}/icons/Adwaita/
      )

    #pixbuf cache file and dll
    install(
      DIRECTORY ${GLIB_PREFIX}/lib/gdk-pixbuf-2.0
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
      )
  endif(ICONS)
endif(STANDALONE)
