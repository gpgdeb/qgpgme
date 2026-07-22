/*
    adqueryresult.h - wraps the result of a ADQueryJob

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

#ifndef __QGPGME_ADQUERYRESULT_H__
#define __QGPGME_ADQUERYRESULT_H__

#include "qgpgme_export.h"

#include <QList>
#include <QString>

#include <gpgme++/result.h>

#include <memory>

namespace GpgME
{
class Error;
}

namespace QGpgME
{

class QGPGME_EXPORT ADQueryResult : public GpgME::Result
{
public:
    struct Attribute {
        QString name;
        QString value;

        Attribute(QString _name, QString _value)
            : name(std::move(_name))
            , value(std::move(_value))
        {
        }
    };

    ADQueryResult();
    ~ADQueryResult();

    explicit ADQueryResult(const GpgME::Error &err);
    explicit ADQueryResult(const QList<Attribute> &attributes, const QString &source, const GpgME::Error &err);

    ADQueryResult(const ADQueryResult &other);
    ADQueryResult &operator=(const ADQueryResult &other);

    ADQueryResult(ADQueryResult &&other);
    ADQueryResult &operator=(ADQueryResult &&other);

    void swap(ADQueryResult &other) noexcept;

    bool isNull() const;

    QList<Attribute> attributes() const;
    QString source() const;

private:
    class Private;
    std::unique_ptr<Private> d;
};

QGPGME_EXPORT void swap(ADQueryResult &a, ADQueryResult &b);

QGPGME_EXPORT std::ostream &operator<<(std::ostream &os, const ADQueryResult &result);
QGPGME_EXPORT std::ostream &operator<<(std::ostream &os, const ADQueryResult::Attribute &result);

}

#endif // __QGPGME_ADQUERYRESULT_H__
