# Install script for directory: D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files/ADCensus")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/3rdParty/tiff" TYPE FILE FILES
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/CMakeLists.txt"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_aux.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_close.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_codec.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_color.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_compress.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_dir.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_dirinfo.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_dirread.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_dirwrite.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_dumpmode.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_error.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_extension.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_fax3.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_fax3sm.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_flush.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_getimage.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_jbig.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_luv.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_lzw.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_next.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_open.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_packbits.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_pixarlog.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_predict.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_print.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_read.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_strip.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_swab.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_thunder.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_tile.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_unix.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_version.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_warning.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_write.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_zip.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/t4.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_config.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_config.vc.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_dir.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_fax3.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tiff.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tiffconf.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tiffconf.vc.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tiffio.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tiffiop.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tiffvers.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_predict.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/uvcode.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tiffconf.gcc.h.cmakein"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/tiff/tif_config.gcc.h.cmakein"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

