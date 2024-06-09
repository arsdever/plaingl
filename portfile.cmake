vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  arsdever/prof
  REF
  d7a637baec83d3064e51a096ff807b74de4f2e8e
  SHA512
  06a05faf6f97bcc14b131cb617db4c853d6820869f83ad5c810cbf1eb612d569a016e0d983b8ee9b751a6da0a6a90edd814c44cb431859adbabd890b41f92c21
  HEAD_REF
  cmake-sample-lib)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME "gamify")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
configure_file("${CMAKE_CURRENT_LIST_DIR}/usage"
               "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage" COPYONLY)
