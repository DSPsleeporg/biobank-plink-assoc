cmake_minimum_required(VERSION 3.14)

project(plink VERSION 1.9 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#Using NOLAPACK macro in plink_common.h
#find_package(LAPACK BLAS REQUIRED)
#find_package(ZLIB REQUIRED)
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
set(REPO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
if (NOT DEFINED ${ZLIB_SOURCE_DIR})
    set(ZLIB_SOURCE_DIR ${REPO_ROOT_DIR}/zlib-1.2.11)
endif()
message(STATUS "ZLIB_SOURCE_DIR: ${ZLIB_SOURCE_DIR}")
set(ZLIB_BIN_DIR ${ZLIB_SOURCE_DIR}/bin)
add_subdirectory(${ZLIB_SOURCE_DIR} ${ZLIB_BIN_DIR})
set(PLINK_SOURCE_DIR ${REPO_ROOT_DIR}/plink_1.9)
set(PLINK_SOURCES 
bgzf.c
dcdflib.c
hfile.c
pigz.c
plink.c
plink_assoc.c
plink_calc.c
plink_cluster.c
plink_cnv.c
plink_common.c
plink_data.c
plink_dosage.c
plink_family.c
plink_filter.c
plink_glm.c
plink_help.c
plink_homozyg.c
plink_lasso.c
plink_ld.c
plink_matrix.c
plink_misc.c
plink_perm.c
plink_rserve.c
plink_set.c
plink_stats.c
SFMT.c
yarn.c
)
set(PLINK_HEADERS
    bgzf.h
    config.h
    dcdflib.h
    hfile.h
    hfile_internal.h
    hts.h
    hts_defs.h
    ipmpar.h
    khash.h
    pigz.h
    plink_assoc.h
    plink_calc.h
    plink_cluster.h
    plink_cnv.h
    plink_common.h
    plink_data.h
    plink_dosage.h
    plink_family.h
    plink_filter.h
    plink_glm.h
    plink_help.h
    plink_homozyg.h
    plink_lasso.h
    plink_ld.h
    plink_matrix.h
    plink_misc.h
    plink_perm.h
    plink_rserve.h
    plink_set.h
    plink_stats.h
    Rconnection.h
    Rsrv.h
    SFMT.h
    sisocks.h
    yarn.h
)
list(TRANSFORM PLINK_SOURCES PREPEND ${PLINK_SOURCE_DIR}/)
list(TRANSFORM PLINK_HEADERS PREPEND ${PLINK_SOURCE_DIR}/)
message(STATUS "PLINK_SOURCES: ${PLINK_SOURCES}")
add_executable(plink ${PLINK_SOURCES} ${PLINK_HEADERS})
target_include_directories(plink PUBLIC ${ZLIB_SOURCE_DIR})
target_link_libraries(plink zlibstatic)

