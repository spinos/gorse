file (GLOB ACAC_WIDGETS_SRC
widgets/*.h
widgets/*.cpp)

set (ACAC_SOURCE_FILES main.cpp 
mainwindow.cpp
AssetPalette.cpp
PiecesList.cpp
${ACAC_WIDGETS_SRC})

QT5_ADD_RESOURCES(ACAC_RES_SOURCES acacia.qrc)

add_executable (acacia ${ACAC_SOURCE_FILES}
${ACAC_RES_SOURCES})

target_link_libraries (acacia Qt5::OpenGL)
