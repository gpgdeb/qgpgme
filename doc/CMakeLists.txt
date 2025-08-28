# CMakeLists.txt for QGpgME docs
# Copyright 2025 g10 Code GmbH
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

set(DOXYGEN_PROJECT_NAME QGpgME)
set(DOXYGEN_PROJECT_NUMBER ${G10_FULL_VERSION})
set(DOXYGEN_PROJECT_BRIEF "Qt API for GpgME")
set(DOXYGEN_RECURSIVE NO)
set(DOXYGEN_FULL_PATH_NAMES NO)
set(DOXYGEN_QUIET YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)

doxygen_add_docs(docs
    ${CMAKE_SOURCE_DIR}/src
    ALL
)
