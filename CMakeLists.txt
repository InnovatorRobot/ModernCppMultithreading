cmake_minimum_required(VERSION 3.5)

project(MultiThreadingCpp17 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.45.0 COMPONENTS *boost libraries here*)
find_package(CURL REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(MultiThreadingCpp17
    main.cpp
    ConditionVariables.hpp
    ProducerConsumer.hpp
    Semaphore.hpp
    RaceCondition.hpp
    Latch.hpp
    ThreadPool.hpp
    Future.hpp
    DevideConquer.hpp
    MeasureSpeedUp.hpp
    MatrixMultiplication.hpp
    MergeSort.hpp
    DownloadImage.hpp
    Mutex.hpp
    Promise.hpp
    JThread.hpp
)

target_link_libraries(MultiThreadingCpp17 ${Boost_LIBRARIES} ${CURL_LIBRARIES})

install(TARGETS MultiThreadingCpp17
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
