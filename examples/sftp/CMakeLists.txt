add_executable(sftp
    main.cpp sftptest.cpp argumentscollector.cpp)

target_link_libraries(sftp PRIVATE QSsh Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${BOTAN_LIB})

