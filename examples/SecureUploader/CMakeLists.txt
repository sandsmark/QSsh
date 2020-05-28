add_executable(SecureUploader
    main.cpp
    securefileuploader.cpp)

target_link_libraries(SecureUploader PRIVATE QSsh Qt5::Core Qt5::Network ${BOTAN_LIB})

