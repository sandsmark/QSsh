About QSsh
==========

QSsh provides SSH and SFTP support for Qt applications without depending on
openssh or similar. The aim of this project is to provide an easy way to use
these protocols in any Qt application.

This project is based on Qt Creator's previous SSH plugin. All credits to
Qt Creator's team!

Unfortunately Qt Creator has decided to start using openssh instead (with some
hacks invoking it etc.), so this is now the most up to date maintained version.


Compiling QSsh
--------------

Prerequisites:
   * [Qt](https://www.qt.io/)
   * [Botan](https://botan.randombit.net/)

Steps:
```bash
git clone https://github.com/sandsmark/QSsh.git
cd QSsh
mkdir build
cd build
qmake ../qssh.pro
make (or mingw32-make or nmake depending on your platform)
```

Examples
--------

### Complete applications

 * [ssh shell](examples/ssh-shell/), similar to a normal command line `ssh` client.
 * [Graphical SFTP browser](tests/manual/ssh/sftpfsmodel/), how to use the SFTP file system model with a QTreeView.
 * [Secure Uploader](examples/SecureUploader/), how to upload a file.


### Various usage examples
 * [Tunneling and forwarding](examples/tunnel/)
 * [SFTP](examples/sftp/)
 * [Remote process handling](examples/remoteprocess/)


### Other

 * [Error handling](examples/errorhandling/)
 * [Auto tests](tests/auto/ssh/)
