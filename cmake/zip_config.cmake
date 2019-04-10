MESSAGE (" find zip and szip ")
IF(WIN32)
SET (ZLIB_INCLUDE_DIR "D:/usr/CMake-hdf5-1.8.20/ZLib")
SET (ZLIB_LIBRARY "D:/usr/lib/libzlib.lib")
ELSEIF (APPLE)
ENDIF ()

MESSAGE (" zlib include is " ${ZLIB_INCLUDE_DIR})
MESSAGE (" zlib library is " ${ZLIB_LIBRARY})

IF (WIN32)
set (SZIP_INCLUDE_DIR "D:/usr/CMake-hdf5-1.8.20/SZip")
set (SZIP_LIBRARY "D:/usr/lib/libszip.lib")

ELSEIF (APPLE)
set (SZIP_DIR "/usr/local/share/cmake/SZIP")
set (SZIP_INCLUDE_DIR "/usr/local/include")
set (SZIP_LIBRARY "/usr/local/lib/libszip.a")

ENDIF ()

MESSAGE (" szip include is " ${SZIP_INCLUDE_DIR})
MESSAGE (" szip library is " ${SZIP_LIBRARY})

