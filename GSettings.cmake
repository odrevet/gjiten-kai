#set the pkg-config variable
set(PKG_CONFIG_EXECUTABLE pkg-config)

#get the gsettings compiler command name
execute_process (
COMMAND ${PKG_CONFIG_EXECUTABLE} gio-2.0 --variable glib_compile_schemas  
OUTPUT_VARIABLE GLIB_COMPLE_SCHEMAS OUTPUT_STRIP_TRAILING_WHITESPACE)

#get the glib prefix directory with pkg-config 
execute_process (
  COMMAND ${PKG_CONFIG_EXECUTABLE} gio-2.0 --variable prefix
  OUTPUT_VARIABLE GLIB_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE
  )

#compile schemas in a directory and put the generated compiled file in
#the specified targetdir
function (compile_schemas SCHEMA_DIR TARGET_DIR)
  execute_process (COMMAND ${GLIB_COMPLE_SCHEMAS} ${SCHEMA_DIR} --targetdir=${TARGET_DIR})
endfunction()

#compile schemas in a directory at install
function (compile_schemas_install SCHEMA_DIR)
  install(
    CODE
    "execute_process (COMMAND ${GLIB_COMPLE_SCHEMAS} ${SCHEMA_DIR})"
    )
endfunction()