# -*- CMake -*-

####################################################################################################

# PREPEND(FILES_TO_TRANSLATE ${CMAKE_CURRENT_SOURCE_DIR} ${SRC_FILES})
FUNCTION(PREPEND var prefix)
  SET(list_var "")
  FOREACH(f ${ARGN})
    LIST(APPEND list_var "${prefix}/${f}")
  ENDFOREACH(f)
  SET(${var} "${list_var}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

####################################################################################################
#
# End
#
####################################################################################################
