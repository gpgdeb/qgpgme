/*
    adqueryjob.h

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

#ifndef __QGPGME_ADQUERYJOB_H__
#define __QGPGME_ADQUERYJOB_H__

#include "job.h"

#include "qgpgme_export.h"

class QString;

namespace GpgME
{
class Error;
}

namespace QGpgME
{

enum class ADQueryOption {
    Default = 0x00,
    RootDSE = 0x01,
    SubstituteVariables = 0x02,
};
Q_DECLARE_FLAGS(ADQueryOptions, ADQueryOption);

class ADQueryResult;

class ADQueryJobPrivate;

class QGPGME_EXPORT ADQueryJob : public Job
{
    Q_OBJECT
protected:
    ADQueryJob(std::unique_ptr<ADQueryJobPrivate>, QObject *parent);

public:
    ~ADQueryJob() override;

    /**
     * Starts an AD query with filter \a filter asking for the attributes \a attributes.
     */
    GpgME::Error start(const QString &filter, const QStringList &attributes, ADQueryOptions options = ADQueryOption::Default);

    /**
     * Runs an AD query with filter \a filter asking for the attributes \a attributes.
     */
    ADQueryResult exec(const QString &filter, const QStringList &attributes, ADQueryOptions options = ADQueryOption::Default);

Q_SIGNALS:
    void result(const ADQueryResult &result, const QString &auditLogAsHtml = {}, const GpgME::Error &auditLogError = {});

private:
    Q_DECLARE_PRIVATE(ADQueryJob)
};

}

#endif // __QGPGME_ADQUERYJOB_H__
