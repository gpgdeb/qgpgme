/*  qgpgmequickjob.cpp

    This file is part of qgpgme, the Qt API binding for gpgme
    Copyright (c) 2017 Intevation GmbH
    Copyright (c) 2020 g10 Code GmbH
    Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>

    QGpgME is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    QGpgME is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

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

#include "qgpgmequickjob.h"

#include "qgpgme_debug.h"
#include "quickjob_p.h"
#include "util.h"

#include <gpgme++/context.h>
#include <gpgme++/key.h>
#include <gpgme++/keygenerationresult.h>


using namespace QGpgME;
using namespace GpgME;

namespace QGpgME
{

class QGpgMEQuickJobPrivate : public QuickJobPrivate
{
public:
    Q_DECLARE_PUBLIC(QGpgMEQuickJob)

    QGpgMEQuickJobPrivate() = default;

    ~QGpgMEQuickJobPrivate() override = default;

private:
    GpgME::Error startIt() override
    {
        Q_ASSERT(!"Not supported by this Job class.");
        return Error::fromCode(GPG_ERR_NOT_SUPPORTED);
    }

    void startNow() override
    {
        Q_ASSERT(!"Not supported by this Job class.");
        Q_Q(QGpgMEQuickJob);
        q->run();
    }

    GpgME::Error startCreate(const QString &uid,
                             const QByteArray &algo,
                             const QDateTime &expires,
                             GpgME::Context::CreationFlags flags) override;

    GpgME::Error startAddSubkey(const GpgME::Key &key,
                                const QByteArray &algo,
                                const QDateTime &expires,
                                GpgME::Context::CreationFlags flags) override;

    GpgME::Error startSetKeyEnabled(const GpgME::Key &key, bool enable) override;
};

}

QGpgMEQuickJob::QGpgMEQuickJob(Context *context)
    : mixin_type(context)
{
    lateInitialization();
}

QGpgMEQuickJob::~QGpgMEQuickJob() = default;

static QGpgMEQuickJob::result_type createWorker(GpgME::Context *ctx,
                                                const QString &uid,
                                                const QByteArray &algo,
                                                const QDateTime &expires,
                                                GpgME::Context::CreationFlags flags)
{
    const unsigned long expiration = expires.isValid()
        ? expires.toMSecsSinceEpoch() / 1000 - QDateTime::currentSecsSinceEpoch()
        : 0;
    const auto result = ctx->createKey(uid.toStdString(), algo.toStdString(), expiration, flags);
    return std::make_tuple(result.error(), QString(), Error());
}

static QGpgMEQuickJob::result_type addSubkeyWorker(GpgME::Context *ctx,
                                                   const GpgME::Key &key,
                                                   const QByteArray &algo,
                                                   const QDateTime &expires,
                                                   GpgME::Context::CreationFlags flags)
{
    const unsigned long expiration = expires.isValid()
        ? expires.toMSecsSinceEpoch() / 1000 - QDateTime::currentSecsSinceEpoch()
        : 0;
    const auto result = ctx->createSubkey(key, algo.toStdString(), expiration, flags);
    return std::make_tuple(result.error(), QString(), Error());
}

static QGpgMEQuickJob::result_type addUidWorker(GpgME::Context *ctx,
                                                const GpgME::Key &key,
                                                const QString &uid)
{
    auto err = ctx->addUid(key, uid.toUtf8().constData());
    return std::make_tuple(err, QString(), Error());
}

static QGpgMEQuickJob::result_type revUidWorker(GpgME::Context *ctx,
                                                const GpgME::Key &key,
                                                const QString &uid)
{
    auto err = ctx->revUid(key, uid.toUtf8().constData());
    return std::make_tuple(err, QString(), Error());
}

static QGpgMEQuickJob::result_type revokeSignatureWorker(Context *ctx,
                                                         const Key &key,
                                                         const Key &signingKey,
                                                         const std::vector<UserID> &userIds)
{
    const auto err = ctx->revokeSignature(key, signingKey, userIds);
    return std::make_tuple(err, QString(), Error());
}

static QGpgMEQuickJob::result_type addAdskWorker(Context *ctx, const Key &key, const char *adsk)
{
    const auto err = ctx->addAdsk(key, adsk);
    return std::make_tuple(err, QString(), Error());
}

Error QGpgMEQuickJobPrivate::startCreate(const QString &uid,
                                 const QByteArray &algo,
                                 const QDateTime &expires,
                                 GpgME::Context::CreationFlags flags)
{
    Q_Q(QGpgMEQuickJob);
    q->run([=](Context *ctx) {
        return createWorker(ctx, uid, algo, expires, flags);
    });

    return {};
}

void QGpgMEQuickJob::startAddUid(const GpgME::Key &key, const QString &uid)
{
    run(std::bind(&addUidWorker, std::placeholders::_1, key, uid));
}

void QGpgMEQuickJob::startRevUid(const GpgME::Key &key, const QString &uid)
{
    run(std::bind(&revUidWorker, std::placeholders::_1, key, uid));
}

Error QGpgMEQuickJobPrivate::startAddSubkey(const GpgME::Key &key,
                                    const QByteArray &algo,
                                    const QDateTime &expires,
                                    GpgME::Context::CreationFlags flags)
{
    if (key.isNull()) {
        return Error::fromCode(GPG_ERR_INV_VALUE);
    }

    Q_Q(QGpgMEQuickJob);
    q->run([=](Context *ctx) {
        return addSubkeyWorker(ctx, key, algo, expires, flags);
    });

    return {};
}

void QGpgMEQuickJob::startRevokeSignature(const Key &key, const Key &signingKey, const std::vector<UserID> &userIds)
{
    run(std::bind(&revokeSignatureWorker, std::placeholders::_1, key, signingKey, userIds));
}

void QGpgMEQuickJob::startAddAdsk(const GpgME::Key &key, const char *adsk)
{
    run(std::bind(&addAdskWorker, std::placeholders::_1, key, adsk));
}

static QGpgMEQuickJob::result_type set_key_enabled(Context *ctx, const Key &key, bool enabled)
{
    const auto err = ctx->setKeyEnabled(key, enabled);
    return std::make_tuple(err, QString(), Error());
}

Error QGpgMEQuickJobPrivate::startSetKeyEnabled(const Key &key, bool enabled)
{
    if (key.isNull()) {
        return Error::fromCode(GPG_ERR_INV_VALUE);
    }

    Q_Q(QGpgMEQuickJob);
    q->run([=](Context *ctx) {
        return set_key_enabled(ctx, key, enabled);
    });

    return {};
}

#include "moc_qgpgmequickjob.cpp"
