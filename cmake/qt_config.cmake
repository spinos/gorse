MESSAGE (" find Qt ")
if(WIN32)
set (QT_QMAKE_EXECUTABLE "D:/usr/qt4.8.6/bin/qmake.exe")
endif()

find_package (Qt4 COMPONENTS QtOpenGL REQUIRED)
message ("qt opengl is " ${QT_QTOPENGL_LIBRARY})
set (QT_MT_REQUIRED ON)
set (QT_USE_QTOPENGL ON)
set (CMAKE_AUTOMOC ON)

