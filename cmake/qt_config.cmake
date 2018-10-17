MESSAGE (" find Qt ")

SET (HAVE_QT5 OFF)
if(WIN32)
IF(EXISTS d:/usr/local/Qt-5.10.0)
    set (QT_HOME d:/usr/local/Qt-5.10.0)
    SET (HAVE_QT5 ON)
ELSE()
    set (QT_HOME D:/usr/qt4.8.6)
ENDIF()

message ("Qt home is " ${QT_HOME})

SET(CMAKE_PREFIX_PATH ${QT_HOME})
## set (QT_QMAKE_EXECUTABLE ${QT_HOME}/bin/qmake.exe)

endif()

IF (HAVE_QT5)
find_package (Qt5 COMPONENTS Core Widgets OpenGL)
get_target_property(QtGui_location Qt5::Widgets LOCATION)

MESSAGE ("Qt5 Core Version" ${Qt5Core_VERSION})
MESSAGE ("Qt5 Gui Location" ${QtGui_location})

include_directories(${QT_HOME}/include)
include_directories(${QT_HOME}/include/QtCore)
include_directories(${QT_HOME}/include/QtGui)
include_directories(${QT_HOME}/include/QtWidgets)
include_directories(${QT_HOME}/include/QtOpenGL)
include_directories(${QT_HOME}/include/QtAngle)
ELSE ()
find_package (Qt4 COMPONENTS QtOpenGL REQUIRED)

message ("qt opengl is " ${QT_QTOPENGL_LIBRARY})

ENDIF()


set (QT_MT_REQUIRED ON)
set (QT_USE_QTOPENGL ON)
set (CMAKE_AUTOMOC ON)

