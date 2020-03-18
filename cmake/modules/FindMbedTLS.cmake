find_path(MBEDTLS_INCLUDE_DIRS
   NAMES
      mbedtls/ssl.h
   HINTS
      ${MBEDTLS_ROOT_DIR}
   PATH_SUFFIXES
      include
)

find_library(MBEDTLS_LIBRARY
   NAMES
      mbedtls
   HINTS
      ${MBEDTLS_ROOT_DIR}
   PATH_SUFFIXES
      lib
      library
      library/Debug
)

find_library(MBEDX509_LIBRARY
   NAMES
      mbedx509
   HINTS
      ${MBEDTLS_ROOT_DIR}
   PATH_SUFFIXES
      lib
      library
      library/Debug
)
find_library(MBEDCRYPTO_LIBRARY
   NAMES
      mbedcrypto
   HINTS
      ${MBEDTLS_ROOT_DIR}
   PATH_SUFFIXES
      lib
      library
      library/Debug
)

set(MBEDTLS_LIBRARIES "${MBEDTLS_LIBRARY}" "${MBEDX509_LIBRARY}" "${MBEDCRYPTO_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MBEDTLS DEFAULT_MSG
    MBEDTLS_INCLUDE_DIRS MBEDTLS_LIBRARY MBEDX509_LIBRARY MBEDCRYPTO_LIBRARY)

mark_as_advanced(MBEDTLS_INCLUDE_DIRS MBEDTLS_LIBRARY MBEDX509_LIBRARY MBEDCRYPTO_LIBRARY)
