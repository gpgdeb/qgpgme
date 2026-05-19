# Installation Instructions

Copyright 2025 g10 Code GmbH

This file is free software; as a special exception the author gives
unlimited permission to copy and/or distribute it, with or without
modifications, as long as this notice is preserved.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

## Installation

We recommend to make an out-of-source build, i.e. do something like

```
mkdir build
cd build
cmake ..
make
make install
```

to configure, build, and install the project.

## Configuration

A few useful project-specific options to pass to cmake are:

`-DBUILD_WITH_QT5=OFF`
    Disables building QGgpME for Qt 5. The default is `ON`.

`-DBUILD_WITH_QT6=OFF`
    Disables building QGgpME for Qt 6. The default is `ON`.

`-DBUILD_TESTING=OFF`
    Disables the build of the tests/examples in the tests folder.
    The default is `ON`.

Some useful general cmake options are:

`-DCMAKE_BUILD_TYPE=RelWithDebInfo`
    Changes the build type to `RelWithDebInfo`.

`-DCMAKE_INSTALL_PREFIX=/some/path`
    Changes the install directory to `/some/path`.

See the documentation of cmake (https://cmake.org/cmake/help/latest/)
for details.

## Packaging

Run
```
make dist
```
to create a tarball of the sources. This uses `git archive`, i.e. it
works on a git clone only. The current HEAD is packaged so that you
can run it safely on a dirty working copy. Additionally, to the sources
a VERSION file and a generated ChangeLog file are added to the tarball.

The following common GnuPG make targets are also supported:

`make distcheck`
    Creates a tarball and runs similar checks as the autotools target
    with the same name.

`make release`
    Essentially this runs `make distcheck` to create a release tarball
    and generates the data for GnuPG's SWDB. All output is written to a
    *.buildlog file.

`make sign-release`
    Signs the release tarball and uploads everything. The necessary
    information is read from `~/.gnupg-autogen.rc`.

`make gen-ChangeLog`
    Used implicitly by `make dist` to generate a ChangeLog from the
    git history.

`make gen-swdb`
    Used by `make release` to generate the data for GnuPG's SWDB.
