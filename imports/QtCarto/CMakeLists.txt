# -*- CMake -*-

####################################################################################################
#
# Include directories
#

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

####################################################################################################
#
# Library
#

set(qtcarto_files
  declarative_map_item.cpp
  qtcarto.cpp
)
add_library(qmlqtcartoplugin SHARED ${qtcarto_files})
# include QImage requires Qui
target_link_libraries(qmlqtcartoplugin qtcarto Qt5::Qml Qt5::Quick Qt5::Positioning)

####################################################################################################
#
# End
#
####################################################################################################
