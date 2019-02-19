# Custom target for NSIS packaging.
set(CPACK_GENERATOR "NSIS")

# Configure file with custom definitions for NSIS.
configure_file(
  ${PROJECT_SOURCE_DIR}/resources/nsis/NSIS.definitions.nsh.in
  ${CMAKE_CURRENT_BINARY_DIR}/resources/nsis/NSIS.definitions.nsh
)

set(CPACK_PACKAGE_NAME ${APP_LOW_NAME})
set(CPACK_PACKAGE_VERSION "0.1.2")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VENDOR ${APP_COMPANY})
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${APP_NAME})
set(CPACK_IGNORE_FILES "\\\\.psd$;/CVS/;/\\\\.svn/;/\\\\.git/;\\\\.swp$;/CMakeLists.txt.user;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;\\\\.qm$;/build/;\\\\.diff$;.DS_Store'")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})

SET(CPACK_PACKAGE_VERSION "0.1.2")
set(CPACK_NSIS_INSTALLED_ICON_NAME "${APP_LOW_NAME}.ico")
set(CPACK_NSIS_HELP_LINK ${APP_URL})
set(CPACK_NSIS_URL_INFO_ABOUT ${APP_URL})
set(CPACK_NSIS_CONTACT ${APP_EMAIL})

 # Load packaging facilities.
include(CPack)
