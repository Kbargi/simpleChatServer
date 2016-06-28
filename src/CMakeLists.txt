find_package(Boost 1.60.0 COMPONENTS system thread program_options) 
find_package(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR} )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRC PROTO_INCL chat.proto)

# get all *.cpp files recursively
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/Config.cpp
                            ${PROJECT_SOURCE_DIR}/RoomManager.cpp
                            ${PROJECT_SOURCE_DIR}/App.cpp
                            ${PROJECT_SOURCE_DIR}/SpecificTasks.cpp
                            ${PROJECT_SOURCE_DIR}/ThreadPool.cpp
                            ${PROJECT_SOURCE_DIR}/Listener.cpp)
                            
add_executable(${PROJECT_NAME} main.cpp ${SRC_FILES} ${PROTO_SRC} ${PROTO_INCL})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES} 
                                      ${Boost_SYSTEM_LIBRARY}
                                      ${Boost_THREAD_LIBRARY}
                                      ${Boost_PROGRAM_OPTIONS_LIBRARY})

