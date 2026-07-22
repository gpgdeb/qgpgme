/*
    t-util.cpp

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

#include "util.h"

#include <qt6compat_p.h>

#include <QString>
#include <QTest>

using namespace std::literals;

class TestUtil : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

private Q_SLOTS:
    void test_split()
    {
        using VecSV = std::vector<std::string_view>;
        {
            const auto result = _qgpgme::split(""sv, ',');
            QCOMPARE(result, VecSV{""sv});
        }
        {
            const auto result = _qgpgme::split("abc"sv, ',');
            QCOMPARE(result, VecSV{"abc"sv});
        }
        {
            const auto result = _qgpgme::split("abc,def"sv, ',');
            const auto expected = VecSV{"abc"sv, "def"sv};
            QCOMPARE(result, expected);
        }
        {
            const auto result = _qgpgme::split(",abc"sv, ',');
            const auto expected = VecSV{""sv, "abc"sv};
            QCOMPARE(result, expected);
        }
        {
            const auto result = _qgpgme::split("abc,"sv, ',');
            const auto expected = VecSV{"abc"sv, ""sv};
            QCOMPARE(result, expected);
        }
        {
            const auto result = _qgpgme::split("abc,,def"sv, ',');
            const auto expected = VecSV{"abc"sv, ""sv, "def"sv};
            QCOMPARE(result, expected);
        }
        {
            const auto result = _qgpgme::split(","sv, ',');
            const auto expected = VecSV{""sv, ""sv};
            QCOMPARE(result, expected);
        }
    }
};

QTEST_GUILESS_MAIN(TestUtil)

#include "t-util.moc"
