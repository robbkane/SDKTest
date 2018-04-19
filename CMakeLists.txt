cmake_minimum_required(VERSION 3.10)
project(SDKTest)

set(CMAKE_CXX_STANDARD 11)

set(APPD_SDK_DIR "extlibs/appdynamics-cpp-sdk")
set(APPD_INCLUDE "${APPD_SDK_DIR}/include")
set(APPD_LIB "${APPD_SDK_DIR}/lib")
set(BOOST_INCLUDE "/usr/local/include")
set(BOOST_LIB "/usr/local/lib")

include_directories(${APPD_INCLUDE} ${BOOST_INCLUDE})

link_directories(${APPD_LIB} ${BOOST_LIB})

add_executable(SDKTest src/main/cpp/SDKTest.cpp)

IF (CMAKE_SYSTEM_NAME STREQUAL Darwin)
    target_link_libraries(SDKTest boost_thread-mt boost_system-mt boost_program_options-mt boost_timer-mt boost_chrono-mt boost_date_time-mt z gcrypt appdynamics pthread)
ELSE ()
    target_link_libraries(SDKTest boost_thread boost_system boost_program_options boost_timer boost_chrono boost_date_time z crypt appdynamics pthread rt dl)
ENDIF()

