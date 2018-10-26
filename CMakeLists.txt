## Fri Feb 16 23:27:27 CST 2018

cmake_minimum_required (VERSION 2.8)
PROJECT (gorse)
SET (CMAKE_BUILD_TYPE RelWithDebInfo)

INCLUDE (./cmake/base_config.cmake)
INCLUDE (./cmake/boost_config.cmake)
INCLUDE (./cmake/qt_config.cmake)
INCLUDE (./cmake/hdf5_config.cmake)
INCLUDE (./cmake/zip_config.cmake)
INCLUDE (./cmake/exr_config.cmake)
INCLUDE (./cmake/clapack_config.cmake)

INCLUDE (${GORS_DIR}/core/math/config.cmake)
INCLUDE (${GORS_DIR}/core/geom/config.cmake)
INCLUDE (${GORS_DIR}/core/mesh/config.cmake)
INCLUDE (${GORS_DIR}/core/rng/config.cmake)
INCLUDE (${GORS_DIR}/core/bvh/config.cmake)
INCLUDE (${GORS_DIR}/gui/qt_base/config.cmake)
INCLUDE (${GORS_DIR}/gui/qt_graph/config.cmake)
INCLUDE (${GORS_DIR}/gui/qt_tree/config.cmake)
INCLUDE (${GORS_DIR}/gui/qt_ogl/config.cmake)
INCLUDE (${GORS_DIR}/gui/ogl/config.cmake)

ADD_SUBDIRECTORY (app)

