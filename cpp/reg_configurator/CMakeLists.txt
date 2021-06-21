cmake_minimum_required(VERSION 2.8)

    project(reg_configurator)
    set(CMAKE_C_COMPILER /usr/bin/g++)
    set(CMAKE_BUILD_TYPE Debug)

    set(CMAKE_C_FLAGS "-lrt")
    set(CMAKE_CXX_FLAGS "-lrt")
    set(SRCS "drp_unit.cpp" "main.cpp" "../../../linenoise_cpp/linenoise.cpp")
    set(HDRS "drp_unit.h" "../../../linenoise_cpp/linenoise.h")
	FIND_PACKAGE( Boost 1.53 COMPONENTS program_options REQUIRED )
	INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

	add_executable(reg_configurator ${SRCS} ${HDRS})
    set_target_properties(reg_configurator PROPERTIES COMPILE_FLAGS "-DMTF7")
	TARGET_LINK_LIBRARIES( reg_configurator LINK_PUBLIC "boost_regex" "boost_filesystem" "boost_system" )

