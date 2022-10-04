add_executable(ssh-shell
    argumentscollector.cpp  main.cpp  shell.cpp)

target_link_libraries(ssh-shell PRIVATE QSsh Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${BOTAN_LIB})

