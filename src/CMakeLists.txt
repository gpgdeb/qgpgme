# CMakeLists.txt for QGpgME
# Copyright 2021 Daniel Vrátil <dvratil@kde.org>
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

add_compile_definitions(GPG_ERR_SOURCE_DEFAULT=GPG_ERR_SOURCE_GPGME)
if(WIN32)
  add_compile_definitions(GPG_ERR_ENABLE_GETTEXT_MACROS=1)
endif()

set(targetname QGpgme)
if (${QT_MAJOR_VERSION} EQUAL "6")
    set(targetname "${targetname}Qt6")
endif()

add_library(${targetname} SHARED)

target_sources(${targetname} PRIVATE
    abstractimportjob.cpp
    changeexpiryjob.cpp
    cleaner.cpp
    cryptoconfig.cpp
    dataprovider.cpp
    debug.cpp
    decryptverifyarchivejob.cpp
    decryptverifyjob.cpp
    defaultkeygenerationjob.cpp
    dn.cpp
    encryptarchivejob.cpp
    encryptjob.cpp
    filelistdataprovider.cpp
    importfromkeyserverjob.cpp
    importjob.cpp
    job.cpp
    listallkeysjob.cpp
    multideletejob.cpp
    qgpgme_debug.cpp
    qgpgmeaddexistingsubkeyjob.cpp
    qgpgmeadduseridjob.cpp
    qgpgmebackend.cpp
    qgpgmechangeexpiryjob.cpp
    qgpgmechangeownertrustjob.cpp
    qgpgmechangepasswdjob.cpp
    qgpgmedecryptjob.cpp
    qgpgmedecryptverifyarchivejob.cpp
    qgpgmedecryptverifyjob.cpp
    qgpgmedeletejob.cpp
    qgpgmedownloadjob.cpp
    qgpgmeencryptarchivejob.cpp
    qgpgmeencryptjob.cpp
    qgpgmeexportjob.cpp
    qgpgmegpgcardjob.cpp
    qgpgmeimportfromkeyserverjob.cpp
    qgpgmeimportjob.cpp
    qgpgmekeyformailboxjob.cpp
    qgpgmekeygenerationjob.cpp
    qgpgmekeylistjob.cpp
    qgpgmelistallkeysjob.cpp
    qgpgmenewcryptoconfig.cpp
    qgpgmequickjob.cpp
    qgpgmereceivekeysjob.cpp
    qgpgmerefreshsmimekeysjob.cpp
    qgpgmerevokekeyjob.cpp
    qgpgmesetprimaryuseridjob.cpp
    qgpgmesignarchivejob.cpp
    qgpgmesignencryptarchivejob.cpp
    qgpgmesignencryptjob.cpp
    qgpgmesignjob.cpp
    qgpgmesignkeyjob.cpp
    qgpgmetofupolicyjob.cpp
    qgpgmeverifydetachedjob.cpp
    qgpgmeverifyopaquejob.cpp
    qgpgmewkdlookupjob.cpp
    qgpgmewkdrefreshjob.cpp
    qgpgmewkspublishjob.cpp
    quickjob.cpp
    receivekeysjob.cpp
    signarchivejob.cpp
    signencryptarchivejob.cpp
    signencryptjob.cpp
    signjob.cpp
    threadedjobmixin.cpp
    util.cpp
    verifydetachedjob.cpp
    verifyopaquejob.cpp
    wkdlookupresult.cpp
    wkdrefreshjob.cpp
)

set(qgpgme_PRIVATE_HEADERS
    abstractimportjob_p.h
    changeexpiryjob_p.h
    cleaner.h
    decryptverifyarchivejob_p.h
    decryptverifyjob_p.h
    encryptarchivejob_p.h
    encryptjob_p.h
    importjob_p.h
    job_p.h
    listallkeysjob_p.h
    protocol_p.h
    qgpgmeaddexistingsubkeyjob.h
    qgpgmeadduseridjob.h
    qgpgmebackend.h
    qgpgmechangeexpiryjob.h
    qgpgmechangeownertrustjob.h
    qgpgmechangepasswdjob.h
    qgpgmedecryptjob.h
    qgpgmedecryptverifyarchivejob.h
    qgpgmedecryptverifyjob.h
    qgpgmedeletejob.h
    qgpgmedownloadjob.h
    qgpgmeencryptarchivejob.h
    qgpgmeencryptjob.h
    qgpgmeexportjob.h
    qgpgmegpgcardjob.h
    qgpgmeimportfromkeyserverjob.h
    qgpgmeimportjob.h
    qgpgmekeyformailboxjob.h
    qgpgmekeygenerationjob.h
    qgpgmekeylistjob.h
    qgpgmelistallkeysjob.h
    qgpgmequickjob.h
    qgpgmereceivekeysjob.h
    qgpgmerefreshsmimekeysjob.h
    qgpgmerevokekeyjob.h
    qgpgmesetprimaryuseridjob.h
    qgpgmesignarchivejob.h
    qgpgmesignencryptarchivejob.h
    qgpgmesignencryptjob.h
    qgpgmesignjob.h
    qgpgmesignkeyjob.h
    qgpgmetofupolicyjob.h
    qgpgmeverifydetachedjob.h
    qgpgmeverifyopaquejob.h
    qgpgmewkdlookupjob.h
    qgpgmewkdrefreshjob.h
    qgpgmewkspublishjob.h
    quickjob_p.h
    signarchivejob_p.h
    signencryptarchivejob_p.h
    signencryptjob_p.h
    signjob_p.h
    threadedjobmixin.h
    util.h
    verifydetachedjob_p.h
    verifyopaquejob_p.h
    wkdrefreshjob_p.h
)

if (${QT_MAJOR_VERSION} EQUAL "5")
    list(APPEND qgpgme_PRIVATE_HEADERS qt6compat_p.h)
endif()

# the public headers
set(qgpgme_CAMELCASE_HEADERS
    AbstractImportJob
    AddExistingSubkeyJob
    AddUserIDJob
    ChangeExpiryJob
    ChangeOwnerTrustJob
    ChangePasswdJob
    CryptoConfig
    DN
    DataProvider
    Debug
    DecryptJob
    DecryptVerifyArchiveJob
    DecryptVerifyJob
    DefaultKeyGenerationJob
    DeleteJob
    DownloadJob
    EncryptArchiveJob
    EncryptJob
    ExportJob
    FileListDataProvider
    GpgCardJob
    ImportFromKeyserverJob
    ImportJob
    Job
    KeyForMailboxJob
    KeyGenerationJob
    KeyListJob
    ListAllKeysJob
    MultiDeleteJob
    Protocol
    QGpgMENewCryptoConfig
    QuickJob
    ReceiveKeysJob
    RefreshKeysJob
    RevokeKeyJob
    SetPrimaryUserIDJob
    SignArchiveJob
    SignEncryptArchiveJob
    SignEncryptJob
    SignJob
    SignKeyJob
    SpecialJob
    TofuPolicyJob
    VerifyDetachedJob
    VerifyOpaqueJob
    WKDLookupJob
    WKDLookupResult
    WKDRefreshJob
    WKSPublishJob
)

ecm_generate_headers(dummy
    HEADER_NAMES
    ${qgpgme_CAMELCASE_HEADERS}
    REQUIRED_HEADERS qgpgme_HEADERS
    PREFIX QGpgME
)

target_sources(${targetname} PRIVATE
    ${qgpgme_HEADERS}
)

include(GenerateExportHeader)
generate_export_header(${targetname}
    BASE_NAME qgpgme
)

target_link_libraries(${targetname} PUBLIC Gpgmepp Gpgme::Gpgme LibGpgError::LibGpgError Qt${QT_MAJOR_VERSION}::Core)

string(TOLOWER ${targetname} targetname_lowercase)
set_target_properties(${targetname} PROPERTIES
    OUTPUT_NAME "${targetname_lowercase}"
    VERSION "${LIBQGPGME_SOVERSION}.${LIBQGPGME_LT_AGE}.${LIBQGPGME_LT_REVISION}"
    SOVERSION "${LIBQGPGME_SOVERSION}"
)

target_include_directories(${targetname} INTERFACE "$<INSTALL_INTERFACE:include/qgpgme-qt${QT_MAJOR_VERSION}>")

if (${QT_MAJOR_VERSION} EQUAL 6)
    set(config_suffix Qt6)
endif()

include(ECMSetupVersion)
ecm_setup_version(PROJECT
                  VARIABLE_PREFIX QGPGME
                  SOVERSION "${LIBQGPGME_SOVERSION}"
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/qgpgme_version.h"
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/QGpgme${config_suffix}ConfigVersion.cmake"
                  COMPATIBILITY AnyNewerVersion
)

include(CMakePackageConfigHelpers)
set(QGPGME_TARGETS_CMAKE_FILE QGpgme${config_suffix}Targets.cmake)
configure_package_config_file(QGpgmeConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/QGpgme${config_suffix}Config.cmake
                              INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QGpgme${config_suffix}
)

if (MSVC)
    install(TARGETS ${targetname} EXPORT ${targetname}Targets DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
else()
    install(TARGETS ${targetname} EXPORT ${targetname}Targets)
endif()

install(FILES ${qgpgme_HEADERS}
              ${CMAKE_CURRENT_BINARY_DIR}/qgpgme_version.h
              ${CMAKE_CURRENT_BINARY_DIR}/qgpgme_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qgpgme-qt${QT_MAJOR_VERSION}/qgpgme)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/QGpgME
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qgpgme-qt${QT_MAJOR_VERSION})

set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/QGpgme${config_suffix})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QGpgme${config_suffix}ConfigVersion.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/QGpgme${config_suffix}Config.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(EXPORT ${targetname}Targets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE QGpgme${config_suffix}Targets.cmake)
install(FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindGpgme.cmake
              ${CMAKE_SOURCE_DIR}/cmake/modules/FindLibGpgError.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
