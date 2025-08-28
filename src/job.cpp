/*
    job.cpp

    This file is part of qgpgme, the Qt API binding for gpgme
    Copyright (c) 2004,2005 Klarälvdalens Datakonsult AB
    Copyright (c) 2016 by Bundesamt für Sicherheit in der Informationstechnik
    Software engineering by Intevation GmbH
    Copyright (c) 2021 g10 Code GmbH
    Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>

    QGpgME is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    QGpgME is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include "job.h"
#include "job_p.h"

#include "addexistingsubkeyjob.h"
#include "adduseridjob.h"
#include "changeownertrustjob.h"
#include "changepasswdjob.h"
#include "decryptjob.h"
#include "deletejob.h"
#include "downloadjob.h"
#include "exportjob.h"
#include "gpgcardjob.h"
#include "keyformailboxjob.h"
#include "keygenerationjob.h"
#include "keylistjob.h"
#include "refreshkeysjob.h"
#include "revokekeyjob.h"
#include "setprimaryuseridjob.h"
#include "signkeyjob.h"
#include "specialjob.h"
#include "tofupolicyjob.h"
#include "wkdlookupjob.h"
#include "wkspublishjob.h"

#include <QCoreApplication>
#include <QDebug>

#include <gpg-error.h>

QGpgME::Job::Job(std::unique_ptr<JobPrivate> dd, QObject *parent)
    : QObject(parent)
    , d_ptr{std::move(dd)}
{
    if (d_ptr) {
        d_ptr->q_ptr = this;
    }

    if (QCoreApplication *app = QCoreApplication::instance()) {
        connect(app, &QCoreApplication::aboutToQuit, this, &Job::slotCancel);
    }
}

QGpgME::Job::Job(QObject *parent)
    : Job{{}, parent}
{
}

QGpgME::Job::~Job() = default;

QString QGpgME::Job::auditLogAsHtml() const
{
    qDebug() << "QGpgME::Job::auditLogAsHtml() should be reimplemented in Kleo::Job subclasses!";
    return QString();
}

GpgME::Error QGpgME::Job::auditLogError() const
{
    qDebug() << "QGpgME::Job::auditLogError() should be reimplemented in Kleo::Job subclasses!";
    return GpgME::Error::fromCode(GPG_ERR_NOT_IMPLEMENTED);
}

bool QGpgME::Job::isAuditLogSupported() const
{
    return auditLogError().code() != GPG_ERR_NOT_IMPLEMENTED;
}

QMap <QGpgME::Job *, GpgME::Context *> QGpgME::g_context_map;

/* static */
GpgME::Context *QGpgME::Job::context(QGpgME::Job *job)
{
    return QGpgME::g_context_map.value (job, nullptr);
}

GpgME::Error QGpgME::Job::startIt()
{
    Q_D(Job);
    Q_ASSERT(d && "This Job class has no JobPrivate class");
    return d->startIt();
}

void QGpgME::Job::startNow()
{
    Q_D(Job);
    Q_ASSERT(d && "This Job class has no JobPrivate class");
    d->startNow();
}

#define make_job_subclass(x)                       \
    QGpgME::x::x(QObject *parent) : Job{parent} {} \
    QGpgME::x::~x() {}

make_job_subclass(AddExistingSubkeyJob)
make_job_subclass(AddUserIDJob)
make_job_subclass(ChangeOwnerTrustJob)
make_job_subclass(ChangePasswdJob)
make_job_subclass(DecryptJob)
make_job_subclass(DeleteJob)
make_job_subclass(DownloadJob)
make_job_subclass(ExportJob)
make_job_subclass(GpgCardJob)
make_job_subclass(KeyForMailboxJob)
make_job_subclass(KeyGenerationJob)
make_job_subclass(KeyListJob)
make_job_subclass(RefreshKeysJob)
make_job_subclass(RevokeKeyJob)
make_job_subclass(SetPrimaryUserIDJob)
make_job_subclass(SignKeyJob)
make_job_subclass(SpecialJob)
make_job_subclass(TofuPolicyJob)
make_job_subclass(WKDLookupJob)
make_job_subclass(WKSPublishJob)

#undef make_job_subclass

#include "moc_job.cpp"

#include "moc_addexistingsubkeyjob.cpp"
#include "moc_adduseridjob.cpp"
#include "moc_changeownertrustjob.cpp"
#include "moc_changepasswdjob.cpp"
#include "moc_decryptjob.cpp"
#include "moc_deletejob.cpp"
#include "moc_downloadjob.cpp"
#include "moc_exportjob.cpp"
#include "moc_gpgcardjob.cpp"
#include "moc_keyformailboxjob.cpp"
#include "moc_keygenerationjob.cpp"
#include "moc_keylistjob.cpp"
#include "moc_refreshkeysjob.cpp"
#include "moc_revokekeyjob.cpp"
#include "moc_setprimaryuseridjob.cpp"
#include "moc_signkeyjob.cpp"
#include "moc_specialjob.cpp"
#include "moc_tofupolicyjob.cpp"
#include "moc_wkdlookupjob.cpp"
#include "moc_wkspublishjob.cpp"
