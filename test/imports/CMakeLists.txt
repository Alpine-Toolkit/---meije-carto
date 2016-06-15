# -*- CMake -*-

####################################################################################################
#
# Include directories
#

include_directories(${CMAKE_SOURCE_DIR})

####################################################################################################
#
# Executable
#

foreach(name
    geo_coordinate_animation
    )
  add_executable(test_${name} test_${name}.cpp)
  target_link_libraries(test_${name} Qt5::Test qtcarto)
  add_test(NAME ${name} COMMAND test_${name})
endforeach(name)

####################################################################################################
#
# End
#
####################################################################################################
