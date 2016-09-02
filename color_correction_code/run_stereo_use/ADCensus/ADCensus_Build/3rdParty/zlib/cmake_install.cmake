# Install script for directory: D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/3rdParty/zlib" TYPE FILE FILES
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/CMakeLists.txt"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/ChangeLog"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/INDEX"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/README"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/README.ipol"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zconf.h.cmakein"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zconf.h.in"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zlib2ansi"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zlib.3"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zlib.map"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zlib.pc.in"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zconf.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zlib.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/crc32.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/deflate.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/gzguts.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/inffast.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/inffixed.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/inflate.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/inftrees.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/trees.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zutil.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/adler32.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/compress.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/crc32.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/deflate.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/gzclose.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/gzlib.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/gzread.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/gzwrite.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/inflate.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/infback.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/inftrees.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/inffast.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/trees.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/uncompr.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/zutil.c"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/3rdParty/zlib/win32" TYPE FILE FILES
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/win32/zlib1.rc"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/zlib/win32/zlib.def"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

