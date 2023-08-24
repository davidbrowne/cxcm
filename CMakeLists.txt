cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(cxcm VERSION 0.9.0)

# file(GLOB SRC_FILES  CONFIGURE_DEPENDS src/*.cxx)
# file(GLOB TEST_FILES CONFIGURE_DEPENDS tests/*.cxx)

set(SRC_FILES
	main.cxx
)

set(TEST_FILES
	cxcm_test.cxx
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${TEST_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/dev_3rd)

