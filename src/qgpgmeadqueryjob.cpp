/*
    qgpgmeadqueryjob.cpp

    This file is part of qgpgme, the Qt API binding for gpgme
    Copyright (c) 2026 g10 Code GmbH
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

#include "qgpgmeadqueryjob.h"

#include "adqueryjob_p.h"
#include "cryptoconfig.h"
#include "debug.h"
#include "protocol.h"
#include "qgpgme_debug.h"
#include "qt6compat_p.h"
#include "util.h"

#include <gpgme++/context.h>
#include <gpgme++/data.h>
#include <gpgme++/defaultassuantransaction.h>

#include <gpg-error.h>

using namespace QGpgME;
using namespace GpgME;
using namespace Qt::Literals;

namespace QGpgME
{

class QGpgMEADQueryJobPrivate : public ADQueryJobPrivate
{
public:
    Q_DECLARE_PUBLIC(QGpgMEADQueryJob)

    QGpgMEADQueryJobPrivate() = default;

    ~QGpgMEADQueryJobPrivate() override = default;

private:
    QGpgMEADQueryJob::result_type runADQuery(Context *ctx, const QString &filter, const QStringList &attributes, ADQueryOptions options);

    GpgME::Error startIt() override;

    void startNow() override
    {
        Q_Q(QGpgMEADQueryJob);
        q->run();
    }

    ADQueryResult execIt() override;
};

}

QGpgMEADQueryJob::QGpgMEADQueryJob(Context *context)
    : mixin_type{context}
{
    lateInitialization();
}

QGpgMEADQueryJob::~QGpgMEADQueryJob() = default;

static GpgME::Error startDirmngr(Context *assuanCtx)
{
    // don't start dirmngr if it is disabled for gpg _and_ gpgsm
    if (QGpgME::cryptoConfig()->entry("gpg", "disable-dirmngr")->boolValue()
        && QGpgME::cryptoConfig()->entry("gpgsm", "disable-dirmngr")->boolValue()) {
        return Error::fromCode(GPG_ERR_NO_DIRMNGR);
    }

    Error err;
    auto spawnCtx = std::unique_ptr<Context>{Context::createForEngine(SpawnEngine, &err)};
    if (err) {
        qCDebug(QGPGME_LOG) << "Error: Failed to get context for spawn engine (" << err << ")";
    }
    const auto gpgconfProgram = GpgME::dirInfo("gpgconf-name");
    // replace backslashes with forward slashes in homedir to work around bug T6833
    std::string homedir{GpgME::dirInfo("homedir")};
    std::replace(homedir.begin(), homedir.end(), '\\', '/');
    const char *argv[] = {
        gpgconfProgram,
        "--homedir",
        homedir.c_str(),
        "--launch",
        "dirmngr",
        NULL
    };
    auto ignoreIO = Data{Data::null};
    if (!err) {
        qCDebug(QGPGME_LOG) << "Starting dirmngr ...";
        err = spawnCtx->spawn(gpgconfProgram, argv,
                              ignoreIO, ignoreIO, ignoreIO,
                              Context::SpawnDetached);
    }

    if (!err) {
        // wait for socket to become available
        int cnt = 0;
        do {
            ++cnt;
            qCDebug(QGPGME_LOG) << "Waiting for dirmngr to start ...";
            QThread::msleep(250 * cnt);
            err = assuanCtx->assuanTransact("GETINFO version");
        } while (err.code() == GPG_ERR_ASS_CONNECT_FAILED && cnt < 5);
    }

    return err;
}

static GpgME::Error setUpDirmngrAssuanConnection(Context *ctx)
{
    Error err;

    const std::string dirmngrSocket = GpgME::dirInfo("dirmngr-socket");
    err = ctx->setEngineFileName(dirmngrSocket.c_str());

    if (!err) {
        err = ctx->setEngineHomeDirectory("");
    }

    if (!err) {
        // try to connect to dirmngr
        err = ctx->assuanTransact("GETINFO version");
        if (err.code() == GPG_ERR_ASS_CONNECT_FAILED) {
            err = startDirmngr(ctx);
        }
    }

    return err;
}

static QString _QStringFromStdStringView(std::string_view s)
{
    return s.empty() ? QString{} : QString::fromUtf8(s.data(), s.size());
}

static QList<QGpgME::ADQueryResult::Attribute> parse_query_result(const std::string &resultData)
{
    QList<QGpgME::ADQueryResult::Attribute> result;

    const auto lines = _qgpgme::split(resultData, '\n');
    for (auto line : lines) {
        if (line.empty()) {
            continue;
        }
        if (line.front() == ' ') {
            // line starts with space -> continuation of last value
            if (!result.empty()) {
                result.back().value += _QStringFromStdStringView(line.substr(1));
            }
        } else {
            const auto nameEnd = line.find(':');
            if ((nameEnd == line.npos) || (line[nameEnd + 1] != ' ')) {
                qCDebug(QGPGME_LOG) << "Unexpected line in AD_QUERY result:" << std::string{line};
                continue;
            }
            const std::string_view name = line.substr(0, nameEnd);
            const std::string_view value = line.substr(nameEnd + 2);
            result.push_back({_QStringFromStdStringView(name), _QStringFromStdStringView(value)});
        }
    }

    return result;
}

static QGpgMEADQueryJob::result_type run_ad_query(Context *ctx, const QString &filter, const QStringList &attributes, ADQueryOptions options)
{
    ADQueryResult result;

    Error err = setUpDirmngrAssuanConnection(ctx);

    if (!err) {
        QStringList arguments;
        if (options & ADQueryOption::RootDSE) {
            arguments.push_back(u"--rootdse"_s);
        }
        if (options & ADQueryOption::SubstituteVariables) {
            arguments.push_back(u"--subst"_s);
        }
        if (!attributes.empty()) {
            arguments.push_back("--attr="_L1 + attributes.join(u','));
        }
        arguments.push_back(u"--"_s);
        arguments.push_back(filter);
        const std::string cmd = std::string{"AD_QUERY "} += arguments.join(u' ').toStdString();
        err = ctx->assuanTransact(cmd.c_str());
        if (err) {
            qCDebug(QGPGME_LOG) << "AD_QUERY failed with" << err;
        }
    }

    if (!err) {
        const auto transaction = std::unique_ptr<DefaultAssuanTransaction>(dynamic_cast<DefaultAssuanTransaction*>(ctx->takeLastAssuanTransaction().release()));
        const std::string source = transaction->firstStatusLine("SOURCE");
        const std::string rawData = transaction->data();
        if (rawData.empty()) {
            qCDebug(QGPGME_LOG) << "No data returned for" << filter;
            result = ADQueryResult{};
        } else {
            qCDebug(QGPGME_LOG) << "Got a result for" << filter << "at" << source.c_str();
            result = ADQueryResult{parse_query_result(rawData), QString::fromStdString(source), {}};
        }
    }

    return std::make_tuple(err ? ADQueryResult{err} : result, QString{}, Error{});
}

GpgME::Error QGpgMEADQueryJobPrivate::startIt()
{
    Q_Q(QGpgMEADQueryJob);
    q->run([=](Context *ctx) {
        return run_ad_query(ctx, m_filter, m_attributes, m_options);
    });

    return {};
}

ADQueryResult QGpgMEADQueryJobPrivate::execIt()
{
    Q_Q(QGpgMEADQueryJob);
    const QGpgMEADQueryJob::result_type r = run_ad_query(q->context(), m_filter, m_attributes, m_options);
    return std::get<0>(r);
}

#include "moc_qgpgmeadqueryjob.cpp"
