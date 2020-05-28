add_executable(sftp
    main.cpp sftptest.cpp argumentscollector.cpp)

target_link_libraries(sftp PRIVATE QSsh Qt5::Core Qt5::Network ${BOTAN_LIB})

