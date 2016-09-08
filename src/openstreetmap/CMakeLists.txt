# -*- CMake -*-

####################################################################################################
#
# Include directories
#

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

####################################################################################################

# set(CMAKE_CXX_FLAGS "-O3 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64")

# Generate in source !
# https://cmake.org/cmake/help/v3.6/module/FindProtobuf.html
# PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS openstreetmap/fileformat.proto openstreetmap/osmformat.proto)

# add_executable(osmpbf-outline osmpbf-outline.cpp ${PROTO_SRCS} ${PROTO_HDRS})

####################################################################################################
#
# End
#
####################################################################################################
