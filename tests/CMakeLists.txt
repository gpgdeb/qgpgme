# CMakeLists.txt for QGpgME tests
# Copyright 2024 g10 Code GmbH
# Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>
#
# This file is part of QGpgME, the Qt API binding for GpgME.
#
# QGpgME is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# QGpgME is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <https://gnu.org/licenses/>.
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(Qt${QT_MAJOR_VERSION}Test ${QT_REQUIRED_VERSION} CONFIG QUIET)

if(NOT TARGET Qt::Test)
    message(STATUS "Qt${QT_MAJOR_VERSION}Test not found, tests will not be built.")
    return()
endif()

set(qgpgme_library QGpgme)
if (${QT_MAJOR_VERSION} EQUAL "6")
    set(qgpgme_library "${qgpgme_library}Qt6")
endif()

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMAddTests)

macro(_g10_add_test _source)
    get_filename_component(_name ${_source} NAME_WE)
    set(_name ${_name}${QT_MAJOR_VERSION})
    ecm_add_test(
        t-support.cpp
        t-support.h
        fixture.qrc
        ${_source}
        TEST_NAME ${_name}
        LINK_LIBRARIES ${qgpgme_library} Gpgmepp Qt::Test
    )
endmacro()

_g10_add_test(t-addexistingsubkey.cpp)
_g10_add_test(t-changeexpiryjob.cpp)
_g10_add_test(t-config.cpp)
_g10_add_test(t-decryptverify.cpp)
_g10_add_test(t-disablekey.cpp)
_g10_add_test(t-encrypt.cpp)
_g10_add_test(t-import.cpp)
_g10_add_test(t-keylist.cpp)
_g10_add_test(t-keylocate.cpp)
_g10_add_test(t-ownertrust.cpp)
_g10_add_test(t-remarks.cpp)
_g10_add_test(t-revokekey.cpp)
_g10_add_test(t-setprimaryuserid.cpp)
_g10_add_test(t-tofuinfo.cpp)
_g10_add_test(t-trustsignatures.cpp)
_g10_add_test(t-various.cpp)
_g10_add_test(t-verify.cpp)
_g10_add_test(t-wkdlookup.cpp)

_g10_add_test(t-dn.cpp)

# disabled test:
# _g10_add_test(t-wkspublish.cpp)

macro(_g10_add_testprogram _source)
    get_filename_component(_name ${_source} NAME_WE)
    set(_name ${_name}${QT_MAJOR_VERSION})
    add_executable(${_name} ${_source})
    target_link_libraries(${_name} ${qgpgme_library} Gpgmepp)
endmacro()

_g10_add_testprogram(run-decryptverifyarchivejob.cpp)
_g10_add_testprogram(run-decryptverifyjob.cpp)
_g10_add_testprogram(run-encryptarchivejob.cpp)
_g10_add_testprogram(run-encryptjob.cpp)
_g10_add_testprogram(run-exportjob.cpp)
_g10_add_testprogram(run-importjob.cpp)
_g10_add_testprogram(run-keyformailboxjob.cpp)
_g10_add_testprogram(run-receivekeysjob.cpp)
_g10_add_testprogram(run-refreshkeysjob.cpp)
_g10_add_testprogram(run-signarchivejob.cpp)
_g10_add_testprogram(run-signjob.cpp)
_g10_add_testprogram(run-verifydetachedjob.cpp)
_g10_add_testprogram(run-verifyopaquejob.cpp)
_g10_add_testprogram(run-wkdrefreshjob.cpp)
