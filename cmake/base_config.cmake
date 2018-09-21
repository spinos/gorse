MESSAGE (" find gorse ")

IF (WIN32)
SET (GORS_DIR "D:/gorse")

elseif (APPLE)
IF (EXISTS /Users/jollyroger)
	SET (APPLE_HOME /Users/jollyroger)
	
ELSEIF (EXISTS /Users/jianzhang)
	SET (APPLE_HOME /Users/jianzhang)
	
ENDIF ()

SET (ALOE_DIR ${APPLE_HOME}/gorse)

else()
SET (ALEO_DIR "~/gorse")

endif ()

if(WIN32)
add_definitions (-DWIN32)
ELSEIF (APPLE)

ELSEIF (UNIX)
add_definitions (-DLINUX)
endif()

MESSAGE (" gorse is " ${GORS_DIR})

INCLUDE_DIRECTORIES (${GORS_DIR}/core
	${GORS_DIR}/gui
	${GORS_DIR}/io)
SET (EXECUTABLE_OUTPUT_PATH ${GORS_DIR}/bin)

