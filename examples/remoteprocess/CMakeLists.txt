add_executable(remoteprocess
    main.cpp remoteprocesstest.cpp argumentscollector.cpp)

target_link_libraries(remoteprocess PRIVATE QSsh Qt5::Core Qt5::Network ${BOTAN_LIB})

