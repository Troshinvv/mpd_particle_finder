cmake_minimum_required(VERSION 3.11)
project(bmn_particle_finder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE debug)


find_package(ROOT REQUIRED COMPONENTS RIO Tree)
find_package(KFParticle REQUIRED)
find_package(Vc REQUIRED)
find_package(KFParticleSimple REQUIRED)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${ROOT_INCLUDE_DIRS}
        ${KFParticle_INCLUDE_DIR}
        ${Vc_INCLUDE_DIR}
        ${KFParticleSimple_INCLUDE_DIR}
        ${KFParticleInterface_INCLUDE_DIR}
)

message(STATUS ${ROOT_INCLUDE_DIRS})
message(STATUS ${KFParticleSimple_LIB_DIR})
#
#set(SOURCES
#        src/find_particles.cc
#        src/utils.cc
#        )
#set(HEADERS
#        src/find_particles.h
#        src/utils.h
#        )
#
link_directories(${Vc_LIB_DIR} ${KFParticle_LIB_DIR} ${KFParticleSimple_LIB_DIR} ${KFParticleInterface_LIB_DIR})
include_directories(${Vc_INCLUDE_DIR} ${KFParticle_INCLUDE_DIR} ${KFParticleSimple_INCLUDE_DIR} ${KFParticleInterface_INCLUDE_DIR})
#
#ROOT_GENERATE_DICTIONARY( G__Finder ${HEADERS} LINKDEF src/FinderLinkDef.h)
#add_library(Finder SHARED ${SOURCES} G__Finder.cxx )
#target_link_libraries(Finder KFParticle Vc KFParticleSimple KFParticleInterface ROOT::RIO ROOT::Tree )

add_executable(find_candidates ${SOURCES} src/main.cc)
target_link_libraries(find_candidates KFParticleSimple KFParticleInterface KFParticle Vc  ROOT::RIO ROOT::Tree ROOT::RIO )
