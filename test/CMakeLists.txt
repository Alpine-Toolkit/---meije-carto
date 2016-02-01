# -*- CMake -*-

####################################################################################################
#
# Include directories
#

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

####################################################################################################
#
# Executable
#

add_executable(test_geo_coordinate test_geo_coordinate.cpp)
target_link_libraries(test_geo_coordinate Qt5::Test qtcarto)
add_test(NAME geo_coordinate COMMAND test_geo_coordinate)

####################################################################################################
#
# End
#
####################################################################################################
