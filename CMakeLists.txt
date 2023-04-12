cmake_minimum_required(VERSION 3.11)
project(bmn_particle_finder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE debug)

macro(add_target_property _target _prop _value)
    get_target_property(_oldprop "${_target}" ${_prop})
    if(NOT _oldprop)
        set_target_properties("${_target}" PROPERTIES ${_prop} "${_value}")
    else(NOT _oldprop)
        set_target_properties("${_target}" PROPERTIES ${_prop} "${_oldprop} ${_value}")
    endif(NOT _oldprop)
endmacro(add_target_property)

find_package(ROOT REQUIRED COMPONENTS RIO Tree)
find_package(KFParticle REQUIRED)
find_package(Vc REQUIRED)
find_package(KFParticleSimple REQUIRED)

message(STATUS ${ROOT_INCLUDE_DIRS})
message(STATUS ${KFParticleSimple_LIB_DIR})

set(SOURCES
        src/find_particles.cc
        src/tree_manager.cc
        src/utils.cc
        )
set(HEADERS
        src/find_particles.h
        src/tree_manager.h
        src/utils.h
        )

link_directories(${Vc_LIB_DIR})
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${Vc_INCLUDE_DIR}
        ${KFParticle_INCLUDE_DIR}
        ${KFParticleSimple_INCLUDE_DIR}
        ${KFParticleInterface_INCLUDE_DIR})
#
add_library(Finder SHARED ${SOURCES} G__Finder.cxx )
ROOT_GENERATE_DICTIONARY( G__Finder ${HEADERS} LINKDEF src/FinderLinkDef.h OPTIONS "-DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS")
add_target_property(Finder COMPILE_FLAGS "-DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS")
target_link_libraries(Finder PUBLIC KFParticle Vc KFParticleSimple KFParticleInterface ROOT::RIO ROOT::Tree ROOT::EG ROOT::ROOTNTuple)

add_executable(find_candidates src/main.cc)
add_target_property(find_candidates COMPILE_FLAGS "-DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS")
target_link_libraries(find_candidates Finder KFParticleSimple KFParticleInterface ROOT::RIO ROOT::Tree )
