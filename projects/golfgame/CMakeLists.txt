#--------------------------------------------------------------------------
# golfgame project
#--------------------------------------------------------------------------

PROJECT(golfgame)
FILE(GLOB project_headers code/*.h)
FILE(GLOB project_sources code/*.cc)

SET(files_project ${project_headers} ${project_sources})
SET(files_proto)
#flat_compile(
#	test.fbs
#	# add more flatbuffer files here
#) 

SOURCE_GROUP("golfgame" FILES ${files_project})

ADD_EXECUTABLE(golfgame ${files_project} ${files_proto})
target_include_directories(golfgame PRIVATE "${CMAKE_BINARY_DIR}/generated/flat")

TARGET_LINK_LIBRARIES(golfgame core render)
ADD_DEPENDENCIES(golfgame core render)

IF(MSVC)
    set_property(TARGET golfgame PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
ENDIF()
