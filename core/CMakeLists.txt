#  cmake_minimum_required(VERSION 3.1.0)
#  # Neural net core 
#  set(P2PN_filename "P2PN")
#  set(P2PN_Include "inc")
#  
#  set(CMAKE_CXX_STANDARD 11)
#  set(CMAKE_CXX_STANDARD_REQUIRED ON)
#  
#  if(CMAKE_VERSION VERSION_LESS "3.7.0")
#      set(CMAKE_INCLUDE_CURRENT_DIR ON)
#  endif()
#  
#  
#  
#  MACRO(FILE_DIRECTORIES return_list ending)
#      FILE(GLOB_RECURSE new_list ${ending})
#      SET(dir_list "")
#      FOREACH(file_path ${new_list})
#          GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
#          SET(dir_list ${dir_list} ${file_path})
#      ENDFOREACH()
#      LIST(REMOVE_DUPLICATES dir_list)
#      SET(${return_list} ${dir_list})
#  ENDMACRO()
#  
#  
#  # QT settings
#  set(CMAKE_AUTOMOC ON)
#  set(CMAKE_AUTORCC ON)
#  #set(CMAKE_AUTOUIC ON)
#  
#  include(${QT_LOCATOR_CMAKE})
#  message("Search for QT5 finished")
#  # find_package(Qt5Widgets REQUIRED)
#  find_package(Qt5 COMPONENTS Network REQUIRED)
#  message("find_package for Qt5 finished")
#  
#  
#  
#  # end QT settings
#  
#  
#  # External dependencies
#  include(FetchContent)
#  set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/dependencies")
#  
#  #  EasyProfiler
#  set(EASY_GIT_REPO  https://github.com/yse/easy_profiler.git)
#  set(EASY_GIT_TAG   v2.1.0)
#  set(BUILD_SHARED_LIBS OFF)
#  set(EASY_PROFILER_NO_SAMPLES True)
#  FetchContent_Declare(
#   EASY_PROFILER
#   GIT_REPOSITORY ${EASY_GIT_REPO}
#   GIT_TAG ${EASY_GIT_TAG}
#  )
#  message("Download dependency: ${EASY_GIT_REPO}")
#  
#  FetchContent_MakeAvailable(EASY_PROFILER)
#  #set(EASY_PROFILER_NO_GUI False)
#  set_target_properties(easy_profiler PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RELATIVE_BUILD_FOLDER}")
#  set_target_properties(easy_profiler PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RELATIVE_BUILD_FOLDER}")
#  set_target_properties(easy_profiler PROPERTIES CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RELATIVE_BUILD_FOLDER}")
#  set_target_properties(easy_profiler PROPERTIES DEBUG_POSTFIX "-d")
#  
#  # End external dependencies
#  
#  include_directories(inc)
#  include_directories(${CMAKE_CURRENT_BINARY_DIR})
#  
#  FILE_DIRECTORIES(H_FILES *.h)
#  FILE_DIRECTORIES(CPP_FILES *.cpp)
#  FILE_DIRECTORIES(UI_FILES *.ui)
#  
#  qt5_wrap_cpp(CPP_MOC_FILES ${H_FILES})
#  qt5_wrap_ui(UIS_HDRS ${UI_FILES})
#  
#  
#  set(SOURCES
#  	${CPP_MOC_FILES}
#      ${CPP_FILES}
#      ${UIS_HDRS}
#      ${H_FILES}
#  )
#  
#  message("CMAKE_CURRENT_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR})
#  
#  message("Create library with files:")
#   FOREACH(file ${SOURCES})
#      message("  File: " ${file})
#   ENDFOREACH()
#  
#  
#  
#   set(P2PN_dependencies 
#      Qt5::Network)
#  
#  add_library(P2PN_shared SHARED ${SOURCES})
#  add_library(P2PN_static STATIC ${SOURCES})
#  add_library(P2PN_static_profile STATIC ${SOURCES})
#   
#  target_compile_definitions(P2PN_shared PRIVATE  P2PN_LIB)
#  target_compile_definitions(P2PN_static PRIVATE  BUILD_STATIC P2PN_LIB)
#  target_compile_definitions(P2PN_static_profile PRIVATE  BUILD_STATIC P2PN_PROFILING P2PN_LIB)
#  
#  set_target_properties(easy_profiler PROPERTIES DEBUG_POSTFIX "-d")
#  set_target_properties(P2PN_shared PROPERTIES LINKER_LANGUAGE CXX  OUTPUT_NAME "${P2PN_filename}" DEBUG_POSTFIX "-d")
#  set_target_properties(P2PN_static PROPERTIES LINKER_LANGUAGE CXX  OUTPUT_NAME "${P2PN_filename}-s" DEBUG_POSTFIX "-d")
#  set_target_properties(P2PN_static_profile PROPERTIES LINKER_LANGUAGE CXX  OUTPUT_NAME "${P2PN_filename}-s-p" DEBUG_POSTFIX "-d")
#  
#  
#  target_link_libraries(P2PN_shared ${P2PN_dependencies})
#  target_link_libraries(P2PN_static ${P2PN_dependencies})
#  target_link_libraries(P2PN_static_profile ${P2PN_dependencies} easy_profiler)
#  
#  
#  target_include_directories(P2PN_shared PUBLIC
#      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${P2PN_Include}>
#      ${CMAKE_CURRENT_BINARY_DIR}
#  	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
#  )
#  target_include_directories(P2PN_static PUBLIC
#      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${P2PN_Include}>
#      ${CMAKE_CURRENT_BINARY_DIR}
#  	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
#  )
#  target_include_directories(P2PN_static_profile PUBLIC
#      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${P2PN_Include}>
#      ${CMAKE_CURRENT_BINARY_DIR}
#  	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
#  )
#  # Define the install path
#  message("Installation to path: ${CMAKE_INSTALL_PREFIX}/bin")
#  message("Installation to path: ${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
#  install(TARGETS P2PN_shared DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
#  install(TARGETS P2PN_shared DESTINATION "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
#  install(TARGETS P2PN_static DESTINATION "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
#  install(TARGETS P2PN_static_profile DESTINATION "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
#  install(TARGETS easy_profiler DESTINATION "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")


cmake_minimum_required(VERSION 3.1.0)

## --------------------------------------------------------------------------------
## -------------------- Project settings ------------------------------------------
## --------------------------------------------------------------------------------


set(LIBRARY_NAME P2PN)
set(LIB_DEFINE P2PN_LIB)
set(LIB_PROFILE_DEFINE P2PN_PROFILING)

set(INCLUDE_PATH "inc")

# Enable/disable QT modules for this example
set(QT_ENABLE ON)
set(QT_DEPLOY ON)
# Find the necessary QT modules
set(QT_MODULES
    Core
    Widgets
    # Gui
    Network
    # Add any other required modules here
)

set(DEBUG_POSTFIX_STR  "-d")
set(STATIC_POSTFIX_STR "-s")
set(PROFILING_POSTFIX_STR "-p")

## --------------------------------------------------------------------------------
## --------------------------------------------------------------------------------
## --------------------------------------------------------------------------------
## --------------------------------------------------------------------------------

set(LIBRARY_NAME_STATIC ${LIBRARY_NAME}_static)
set(LIBRARY_NAME_SHARED ${LIBRARY_NAME}_shared)
set(LIBRARY_NAME_STATIC_PROFILE ${LIBRARY_NAME_STATIC}_profile)



if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()



# QT settings
if(QT_ENABLE)
    include(${QT_LOCATOR_CMAKE})

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    #set(CMAKE_AUTOUIC ON)

    find_package(Qt5 REQUIRED COMPONENTS ${QT_MODULES})
endif()
# end QT settings

# External dependencies from the dependencies.cmake
use_easy_profiler(EASY_PROFILER_DEPS)
set(LIBRARY_DEPENDENCIES)


include_directories(inc)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

FILE_DIRECTORIES(H_FILES *.h)
FILE_DIRECTORIES(CPP_FILES *.cpp)


set(SOURCES
    ${CPP_FILES}
    ${H_FILES}
)

if(QT_ENABLE)
    # Search for QT specific files
    FILE_DIRECTORIES(UI_FILES *.ui)    
    FILE_DIRECTORIES(RES_FILES *.qrc)    

    qt5_wrap_cpp(CPP_MOC_FILES ${H_FILES})
    qt5_wrap_ui(UIS_HDRS ${UI_FILES})
    qt5_add_resources(RESOURCE_FILES ${RES_FILES})

    set(SOURCES ${SOURCES}
	    ${CPP_MOC_FILES}
	    ${UIS_HDRS}
        ${RESOURCE_FILES})

    # Link the QT modules to your executable
    foreach(MODULE ${QT_MODULES})
        set(QT_LIBS ${QT_LIBS} Qt5::${MODULE})
    endforeach()
    set(LIBRARY_DEPENDENCIES ${LIBRARY_DEPENDENCIES} ${QT_LIBS})
endif()

message("Create library with files:")
FOREACH(file ${SOURCES})
   message("  File: " ${file})
ENDFOREACH()
message("Using external dependencies:")
FOREACH(dep ${extDependencies})
   message("  Library: " ${dep})
ENDFOREACH()





add_library(${LIBRARY_NAME_SHARED}                          SHARED      ${SOURCES})
add_library(${LIBRARY_NAME_STATIC}                          STATIC      ${SOURCES})
add_library(${LIBRARY_NAME_STATIC_PROFILE}                  STATIC      ${SOURCES})
 
target_compile_definitions(${LIBRARY_NAME_SHARED}           PRIVATE                  ${LIB_DEFINE})
target_compile_definitions(${LIBRARY_NAME_STATIC}           PRIVATE     BUILD_STATIC ${LIB_DEFINE})
target_compile_definitions(${LIBRARY_NAME_STATIC_PROFILE}   PRIVATE     BUILD_STATIC ${LIB_DEFINE}  ${LIB_PROFILE_DEFINE})

set_target_properties(${LIBRARY_NAME_SHARED}                PROPERTIES  LINKER_LANGUAGE CXX  OUTPUT_NAME "${LIBRARY_NAME}"                                              DEBUG_POSTFIX ${DEBUG_POSTFIX_STR})
set_target_properties(${LIBRARY_NAME_STATIC}                PROPERTIES  LINKER_LANGUAGE CXX  OUTPUT_NAME "${LIBRARY_NAME}${STATIC_POSTFIX_STR}"                         DEBUG_POSTFIX ${DEBUG_POSTFIX_STR})
set_target_properties(${LIBRARY_NAME_STATIC_PROFILE}        PROPERTIES  LINKER_LANGUAGE CXX  OUTPUT_NAME "${LIBRARY_NAME}${STATIC_POSTFIX_STR}${PROFILING_POSTFIX_STR}" DEBUG_POSTFIX ${DEBUG_POSTFIX_STR})


target_link_libraries(${LIBRARY_NAME_SHARED}            ${LIBRARY_DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME_STATIC}            ${LIBRARY_DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME_STATIC_PROFILE}    ${LIBRARY_DEPENDENCIES} ${EASY_PROFILER_DEPS})


target_include_directories(${LIBRARY_NAME_SHARED} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
    ${CMAKE_CURRENT_BINARY_DIR}
	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
)
target_include_directories(${LIBRARY_NAME_STATIC}  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
    ${CMAKE_CURRENT_BINARY_DIR}
	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
)
target_include_directories(${LIBRARY_NAME_STATIC_PROFILE} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
    ${CMAKE_CURRENT_BINARY_DIR}
	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
)

# Define the install path
message("Installation to path: ${CMAKE_INSTALL_PREFIX}/bin")
message("Installation to path: ${INSTALL_LIB_PATH}")
install(TARGETS ${LIBRARY_NAME_SHARED} ${LIBRARY_NAME_STATIC}  ${LIBRARY_NAME_STATIC_PROFILE} ${EASY_PROFILER_DEPS} DESTINATION "${INSTALL_BIN_PATH}")
install(TARGETS ${LIBRARY_NAME_SHARED} ${LIBRARY_NAME_STATIC}  ${LIBRARY_NAME_STATIC_PROFILE} DESTINATION "${INSTALL_LIB_PATH}")

copyLibraryHeaders("${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}" "${CMAKE_INSTALL_PREFIX}/include" "${LIBRARY_NAME}")


if(QT_ENABLE AND QT_DEPLOY)
    DEPLOY_QT(${QT_PATH} "$<TARGET_FILE_DIR:profiler_gui>/$<TARGET_FILE_NAME:profiler_gui>" ${INSTALL_BIN_PATH})
endif()
