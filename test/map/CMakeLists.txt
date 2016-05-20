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

# tile_loader

foreach(name
    file_tile_cache
    geo_coordinate
    geoportail_license
    # geoportail_wmts_tile_fetcher
    cache3q
    tile_matrix_set
    # viewport
    # wmts_manager
    # wmts_request_manager
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
