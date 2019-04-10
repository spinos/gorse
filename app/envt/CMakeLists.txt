set (ENVT_SOURCE_FILES IblRenderer.cpp
EnvironScene.cpp
RenderWidget.cpp
SampleWidget.cpp
SampleDlg.cpp
ImgWidget.cpp
ImgDlg.cpp
window.cpp
main.cpp)

add_executable (envt ${ENVT_SOURCE_FILES}
${GORS_MATH_SRC}
${GORS_RNG_SRC}
${ALOE_INTERFACE_SRC}
${ALOE_QT_INTERFACE_SRC}
${ALOE_QT_SRC}
${ALOE_QT_LAB_SRC}
${ALOE_BRDF_SRC}
${ALOE_IMG_SRC})

target_link_libraries (envt Qt5::Core Qt5::Widgets)
target_link_libraries (envt ${Boost_SYSTEM_LIBRARY})
target_link_libraries (envt ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (envt ${Boost_REGEX_LIBRARY})
target_link_libraries (envt ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (envt ${Boost_THREAD_LIBRARY})
target_link_libraries (envt ${Boost_CHRONO_LIBRARY})
target_link_libraries (envt ${OpenEXR_Library})

