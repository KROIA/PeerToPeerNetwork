

# Get a list of all subdirectories in the current directory
file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

include(ExampleMaster.cmake)

# Loop over each subdirectory and add it as a subdirectory in the project
foreach(subdirectory ${subdirectories})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory})
        add_subdirectory(${subdirectory})
    endif()
endforeach()