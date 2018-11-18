set (FDLG_SOURCE_FILES main.cpp
mainwindow.cpp)

QT5_ADD_RESOURCES(FDLG_RES_SOURCES fdlgt.qrc)

add_executable (fdlgt ${FDLG_SOURCE_FILES}
${GORS_QT_BASE_SRC}
${FDLG_RES_SOURCES}
)

target_link_libraries (fdlgt Qt5::Widgets)
