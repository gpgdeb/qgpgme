# Common CMake settings for g10 Code; based on KDECMakeSettings.cmake
#
# Copyright 2014 Alex Merry <alex.merry@kde.org>
# Copyright 2013 Aleix Pol <aleixpol@kde.org>
# Copyright 2012-2013 Stephen Kelly <steveire@gmail.com>
# Copyright 2007 Matthias Kretz <kretz@kde.org>
# Copyright 2006-2007 Laurent Montel <montel@kde.org>
# Copyright 2006-2013 Alex Neundorf <neundorf@kde.org>
# Copyright 2025 g10 Code GmbH
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# SPDX-License-Identifier: BSD-3-Clause

################# RPATH handling ##################################

if (UNIX)
    # Append the linker search paths to the RPATH for installed targets
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif (UNIX)

################ Testing setup ####################################

option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING)
    enable_testing()
endif()



################ Build-related settings ###########################

# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} in about every subdir
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# put the include dirs which are in the source or build tree
# before all other include dirs, so the headers in the sources
# are preferred over the already installed ones
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# Add the src and build dir to the BUILD_INTERFACE include directories
# of all targets. Similar to CMAKE_INCLUDE_CURRENT_DIR, but transitive.
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# When a shared library changes, but its includes do not, don't relink
# all dependencies. It is not needed.
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)

# Add the SOVERSION target property to the filename of generated DLL filenames
if (WIN32 AND NOT MSVC)
    # Official variable for enabling versioned DLL filenames; since cmake 3.27
    set(CMAKE_DLL_NAME_WITH_SOVERSION ON)
    # Undocumented variable which has the same effect (used internally for Cygwin)
    set(CMAKE_SHARED_LIBRARY_NAME_WITH_VERSION ON)
endif()
