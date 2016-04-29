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

add_executable(test_triangulation test_triangulation.cpp polygon_seidler_triangulation.cpp)
target_link_libraries(test_triangulation Qt5::Test qtcarto)
add_test(NAME triangulation COMMAND test_triangulation)

####################################################################################################
#
# End
#
####################################################################################################
