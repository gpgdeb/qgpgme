# CMakeLists.txt for QGpgME
# Copyright 2025 g10 Code GmbH
# Software engineering by Carl Schwan <carl.schwan@gnupg.com>
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

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(FULL_VERSION "2.0.0")

string(REGEX MATCH "^[0-9]+.[0-9]+.[0-9]+" cmake_compat_version ${FULL_VERSION})
project(qgpgme VERSION ${cmake_compat_version})

set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_MICRO ${PROJECT_VERSION_PATCH})

# LT Version numbers, remember to change them just *before* a release.
#   (Code changed:			REVISION++)
#   (Interfaces added/removed/changed:	CURRENT++, REVISION=0)
#   (Interfaces added:			AGE++)
#   (Interfaces removed:		AGE=0)
set(LIBQGPGME_LT_CURRENT "22")
set(LIBQGPGME_LT_AGE "7")
set(LIBQGPGME_LT_REVISION "0")
math(EXPR LIBQGPGME_SOVERSION "${LIBQGPGME_LT_CURRENT} - ${LIBQGPGME_LT_AGE}")

set(GPG_ERROR_REQUIRED_VERSION "1.47")
set(GPGME_REQUIRED_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.0")
set(GPGMEPP_REQUIRED_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.0")
set(QT5_REQUIRED_VERSION "5.15.0")
set(QT6_REQUIRED_VERSION "6.5.0")

option(BUILD_WITH_QT5 "Build with Qt 5" ON)
option(BUILD_WITH_QT6 "Build with Qt 6" ON)
option(CONFIGURE_FOR_DISTRIBUTION "Configure only for `make dist`, etc., ignoring required dependencies" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

include(G10CMakeSettings)

include(G10AddDistTargets)
include(G10AddGenChangeLogTarget)
include(G10AddReleaseTargets)
include(G10CompilerSettings)
include(G10GetFullVersion)
include(ECMGenerateHeaders)
include(ECMUninstallTarget)
include(FeatureSummary)
include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(LibGpgError ${GPG_ERROR_REQUIRED_VERSION})
find_package(Gpgme ${GPGME_REQUIRED_VERSION})
find_package(Gpgmepp ${GPGMEPP_REQUIRED_VERSION} CONFIG)
if (CONFIGURE_FOR_DISTRIBUTION)
    message(WARNING "Configuring only for running `make dist`, etc., ignoring required dependencies")
else()
    set_package_properties(LibGpgError PROPERTIES TYPE REQUIRED)
    set_package_properties(Gpgme PROPERTIES TYPE REQUIRED)
    set_package_properties(Gpgmepp PROPERTIES TYPE REQUIRED)
endif()

# Check for tools for building the Qt binding docs
find_package(Doxygen OPTIONAL_COMPONENTS dot)
set_package_properties(Doxygen PROPERTIES DESCRIPTION "Used for building the documentation")

g10_get_full_version(TAG_PREFIX gpgmeqt)

# create the VERSION file
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    # get the current commit ID
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --verify HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE commit_id
    )
    string(STRIP "${commit_id}" commit_id) # strip trailing whitespace
    file(WRITE "${CMAKE_BINARY_DIR}/VERSION" "${G10_FULL_VERSION}\n${commit_id}\n")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/VERSION")
    configure_file("${CMAKE_SOURCE_DIR}/VERSION" "VERSION" COPYONLY)
else()
    file(WRITE "${CMAKE_BINARY_DIR}/VERSION" "${G10_FULL_VERSION}\nunknown\n")
endif()

if(UNIX)
    # start ChangeLog at import from KDE in gpgme repo
    g10_add_gen_changelog_target(
        SINCE "2016-03-08T00:00:00"
        FOOTER "${CMAKE_SOURCE_DIR}/build-aux/git-log-footer"
    )

    set(EXTRA_DIST
        "${CMAKE_BINARY_DIR}/ChangeLog"
        "${CMAKE_BINARY_DIR}/VERSION"
    )
    g10_add_dist_targets(
        VERSION "${G10_FULL_VERSION}"
        ARCHIVE_FORMAT "tar.xz"
        EXTRA_FILES ${EXTRA_DIST}
    )
    add_dependencies(dist gen-ChangeLog)

    g10_add_release_targets(
        VERSION "${G10_FULL_VERSION}"
        ARCHIVE_FORMAT "tar.xz"
        RELEASE_ARCHIVE_SUFFIX "gpgme"
    )
endif()

include(G10GitHooks)
g10_configure_git_hooks()

if(BUILD_WITH_QT5)
    set(QT_MAJOR_VERSION 5)
    find_package(Qt5 ${QT5_REQUIRED_VERSION} COMPONENTS Core)
    if (CONFIGURE_FOR_DISTRIBUTION)
        set_package_properties(Qt5 PROPERTIES TYPE REQUIRED)
    endif()
    if (LibGpgError_FOUND AND Gpgme_FOUND AND Gpgmepp_FOUND AND Qt5_FOUND)
        add_subdirectory(src 5/src)
        if(BUILD_TESTING)
            add_subdirectory(tests 5/tests)
        endif()
    endif()
endif()

if(BUILD_WITH_QT6)
    set(QT_MAJOR_VERSION 6)
    find_package(Qt6 ${QT6_REQUIRED_VERSION} CONFIG COMPONENTS Core CoreTools)
    if (CONFIGURE_FOR_DISTRIBUTION)
        set_package_properties(Qt6 PROPERTIES TYPE REQUIRED)
    endif()
    if (LibGpgError_FOUND AND Gpgme_FOUND AND Gpgmepp_FOUND AND Qt6_FOUND)
        add_subdirectory(src 6/src)
        if(BUILD_TESTING)
            add_subdirectory(tests 6/tests)
        endif()
    endif()
endif()

if(DOXYGEN_FOUND)
    add_subdirectory(doc)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
