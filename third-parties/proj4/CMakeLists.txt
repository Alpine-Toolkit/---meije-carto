# -*- CMake -*-

####################################################################################################

# /usr/lib64/ccache/cc
# -DMUTEX_pthread
# -DWITH_PROJ4
# -Dqtcarto_proj4_EXPORTS
# -I/home/fabrice/home/developpement/qtcarto-application/alpine-toolkit/qtcarto/cmake-build/third-parties/proj4
# -I/home/fabrice/home/developpement/qtcarto-application/alpine-toolkit/qtcarto/third-parties/proj4
# -I/home/fabrice/home/developpement/qtcarto-application/alpine-toolkit/qtcarto/PROJ4_INCLUDE_DIR
# -I/home/fabrice/home/developpement/qtcarto-application/alpine-toolkit/qtcarto/src
# -g
# -o
# CMakeFiles/qtcarto_proj4.dir/proj.4.git/src/pj_fwd3d.c.o
# -c /home/fabrice/home/developpement/qtcarto-application/alpine-toolkit/qtcarto/third-parties/proj4/proj.4.git/src/pj_fwd3d.c

# /usr/lib64/ccache/cc
# -DMUTEX_pthread
# -DPROJ_LIB=\"/usr/local/share/proj\"
# -Dproj_EXPORTS
# -I/home/fabrice/home/developpement/qtcarto-application/alpine-toolkit/qtcarto/third-parties/proj4/proj.4.git/src
# -I/home/fabrice/home/developpement/qtcarto-application/alpine-toolkit/qtcarto/third-parties/proj4/proj.4.git/build/src
# -DNDEBUG
# -fPIC
# -o PJ_sch.c.o
# -c /home/fabrice/home/developpement/qtcarto-application/alpine-toolkit/qtcarto/third-parties/proj4/proj.4.git/src/PJ_sch.c

####################################################################################################

set(PROJ4_SOURCE_PATH "proj.4.git")
set(LIBPROJ_SOURCE_PATH "${PROJ4_SOURCE_PATH}/src")

####################################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${PROJ4_SOURCE_PATH}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${PROJ4_SOURCE_PATH}/cmake/modules" ${CMAKE_MODULE_PATH})

include(Proj4Utilities)

####################################################################################################

set(PROJECT_INTERN_NAME PROJ)

include(Proj4Version)
proj_version(MAJOR 4 MINOR 9 PATCH 3)
set(PROJ_API_VERSION "11")
set(PROJ_BUILD_VERSION "11.0.0")

####################################################################################################

include("proj4_config.cmake")

####################################################################################################
#
# threading configuration
#

find_package(Threads)
include(CheckIncludeFiles)
include(CheckSymbolExists)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

CHECK_SYMBOL_EXISTS(PTHREAD_MUTEX_RECURSIVE pthread.h HAVE_PTHREAD_MUTEX_RECURSIVE_DEFN)
if (HAVE_PTHREAD_MUTEX_RECURSIVE_DEFN)
  add_definitions(-DHAVE_PTHREAD_MUTEX_RECURSIVE=1)
endif()

option(USE_THREAD "Build libproj with thread/mutex support " ON)

if(NOT USE_THREAD)
   add_definitions(-DMUTEX_stub)
endif(NOT USE_THREAD)

# if(USE_THREAD AND Threads_FOUND AND CMAKE_USE_WIN32_THREADS_INIT )
#    add_definitions(-DMUTEX_win32)
# endif(USE_THREAD AND Threads_FOUND AND CMAKE_USE_WIN32_THREADS_INIT )

if(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
   add_definitions(-DMUTEX_pthread)
endif(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)

if(USE_THREAD AND NOT Threads_FOUND)
  message(FATAL_ERROR "No thread library found and thread/mutex support is required by USE_THREAD option")
endif(USE_THREAD AND NOT Threads_FOUND)

####################################################################################################

add_definitions(-Dproj_EXPORTS)
# add_definitions(-DNDEBUG)

# Embed PROJ_LIB data files location
# add_definitions(-DPROJ_LIB="${CMAKE_INSTALL_PREFIX}/${DATADIR}")
add_definitions(-DPROJ_LIB="${CMAKE_CURRENT_SOURCE_DIR}/${PROJ4_SOURCE_PATH}/nad")

####################################################################################################

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${PROJ4_SOURCE_PATH}/src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

####################################################################################################

SET(SRC_LIBPROJ_PJ
  nad_init.c
  PJ_aea.c
  PJ_aeqd.c
  PJ_airy.c
  PJ_aitoff.c
  PJ_august.c
  PJ_bacon.c
  PJ_bipc.c
  PJ_boggs.c
  PJ_bonne.c
  PJ_calcofi.c
  PJ_cass.c
  PJ_cc.c
  PJ_cea.c
  PJ_chamb.c
  PJ_collg.c
  PJ_comill.c
  PJ_crast.c
  PJ_denoy.c
  PJ_eck1.c
  PJ_eck2.c
  PJ_eck3.c
  PJ_eck4.c
  PJ_eck5.c
  PJ_eqc.c
  PJ_eqdc.c
  PJ_fahey.c
  PJ_fouc_s.c
  PJ_gall.c
  PJ_geos.c
  PJ_gins8.c
  PJ_gnom.c
  PJ_gn_sinu.c
  PJ_goode.c
  PJ_gstmerc.c
  PJ_hammer.c
  PJ_hatano.c
  PJ_igh.c
  PJ_isea.c
  PJ_imw_p.c
  PJ_krovak.c
  PJ_labrd.c
  PJ_laea.c
  PJ_lagrng.c
  PJ_larr.c
  PJ_lask.c
  PJ_lcca.c
  PJ_lcc.c
  PJ_loxim.c
  PJ_lsat.c
  PJ_misrsom.c
  PJ_mbt_fps.c
  PJ_mbtfpp.c
  PJ_mbtfpq.c
  PJ_merc.c
  PJ_mill.c
  PJ_mod_ster.c
  PJ_moll.c
  PJ_natearth.c
  PJ_natearth2.c
  PJ_nell.c
  PJ_nell_h.c
  PJ_nocol.c
  PJ_nsper.c
  PJ_nzmg.c
  PJ_ob_tran.c
  PJ_ocea.c
  PJ_oea.c
  PJ_omerc.c
  PJ_ortho.c
  PJ_patterson.c
  PJ_poly.c
  PJ_putp2.c
  PJ_putp3.c
  PJ_putp4p.c
  PJ_putp5.c
  PJ_putp6.c
  PJ_qsc.c
  PJ_robin.c
  PJ_rpoly.c
  PJ_sch.c
  PJ_sconics.c
  PJ_somerc.c
  PJ_sterea.c
  PJ_stere.c
  PJ_sts.c
  PJ_tcc.c
  PJ_tcea.c
  PJ_tmerc.c
  PJ_tpeqd.c
  PJ_urm5.c
  PJ_urmfps.c
  PJ_vandg.c
  PJ_vandg2.c
  PJ_vandg4.c
  PJ_wag2.c
  PJ_wag3.c
  PJ_wag7.c
  PJ_wink1.c
  PJ_wink2.c
  proj_etmerc.c
  )

SET(SRC_LIBPROJ_CORE
  aasincos.c
  adjlon.c
  bch2bps.c
  bchgen.c
  biveval.c
  dmstor.c
  emess.c
  emess.h
  geocent.c
  geocent.h
  geodesic.c
  mk_cheby.c
  nad_cvt.c
  nad_init.c
  nad_intr.c
  pj_apply_gridshift.c
  pj_apply_vgridshift.c
  pj_auth.c
  pj_ctx.c
  pj_fileapi.c
  pj_datum_set.c
  pj_datums.c
  pj_deriv.c
  pj_ell_set.c
  pj_ellps.c
  pj_errno.c
  pj_factors.c
  pj_fwd.c
  pj_fwd3d.c
  pj_gauss.c
  pj_gc_reader.c
  pj_geocent.c
  pj_gridcatalog.c
  pj_gridinfo.c
  pj_gridlist.c
  PJ_healpix.c
  pj_init.c
  pj_initcache.c
  pj_inv.c
  pj_inv3d.c
  pj_latlong.c
  pj_list.c
  pj_list.h
  pj_log.c
  pj_malloc.c
  pj_mlfn.c
  pj_msfn.c
  pj_mutex.c
  pj_open_lib.c
  pj_param.c
  pj_phi2.c
  pj_pr_list.c
  pj_qsfn.c
  pj_release.c
  pj_strerrno.c
  pj_transform.c
  pj_tsfn.c
  pj_units.c
  pj_utils.c
  pj_zpoly1.c
  proj_mdist.c
  proj_rouss.c
  rtodms.c
  vector1.c
  pj_strtod.c
  )

set(HEADERS_LIBPROJ
  projects.h
  proj_api.h
  geodesic.h
  )

set(ALL_LIBPROJ_SOURCES ${SRC_LIBPROJ_PJ} ${SRC_LIBPROJ_CORE})
set(ALL_LIBPROJ_HEADERS ${HEADERS_LIBPROJ} )

PREPEND(ALL_LIBPROJ_SOURCES ${LIBPROJ_SOURCE_PATH} ${ALL_LIBPROJ_SOURCES})
PREPEND(ALL_LIBPROJ_HEADERS ${LIBPROJ_SOURCE_PATH} ${ALL_LIBPROJ_HEADERS})

set(PROJ_CORE_TARGET qtcarto_proj4)
set(PROJ_LIBRARY_TYPE SHARED)
# set(PROJ_LIBRARY_TYPE STATIC)

add_library(${PROJ_CORE_TARGET}
  ${PROJ_LIBRARY_TYPE}
  ${ALL_LIBPROJ_SOURCES}
  ${ALL_LIBPROJ_HEADERS}
  ${CMAKE_CURRENT_BINARY_DIR}/proj_config.h
  # ${PROJ_RESOURCES}
  )

set(PROJ_LIBRARIES ${PROJ_CORE_TARGET} )
if(UNIX)
  find_library(M_LIB m)
  if(M_LIB)
    TARGET_LINK_LIBRARIES(${PROJ_CORE_TARGET} -lm)
  endif()
endif(UNIX)
if(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
  TARGET_LINK_LIBRARIES(${PROJ_CORE_TARGET} ${CMAKE_THREAD_LIBS_INIT})
endif(USE_THREAD AND Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)

####################################################################################################

add_executable(test_proj4 test.c)
target_link_libraries(test_proj4 ${PROJ_CORE_TARGET})

####################################################################################################
#
# End
#
####################################################################################################
