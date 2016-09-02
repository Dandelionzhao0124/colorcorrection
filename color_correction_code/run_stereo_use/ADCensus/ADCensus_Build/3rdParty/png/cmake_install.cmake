# Install script for directory: D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/3rdParty/png" TYPE FILE FILES
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/CMakeLists.txt"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/LICENSE"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/README"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/README.ipol"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/png.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pnglibconf.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngconf.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngdebug.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pnginfo.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngpriv.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngstruct.h"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/png.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngerror.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngget.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngmem.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngpread.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngread.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngrio.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngrtran.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngrutil.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngset.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngtrans.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngwio.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngwrite.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngwtran.c"
    "D:/RQRunning/ColorCorrection/ZWJ/Disparity/ADCensus-master/ADCensus_CMake/3rdParty/png/pngwutil.c"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

