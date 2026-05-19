/*
    t-dn.cpp

    This file is part of qgpgme, the Qt API binding for gpgme
    Copyright (c) 2023-2025 g10 Code GmbH
    Software engineering by Sune Vuorela <sune@vuorela.dk>

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
#include "dn.h"

#include <qt6compat_p.h>

#include <QString>
#include <QTest>

using namespace Qt::Literals::StringLiterals;

namespace QGpgME {
bool operator==(const QGpgME::DN::Attribute &first, const QGpgME::DN::Attribute &second) {
    return std::tie(first.name(), first.value()) == std::tie(second.name(), second.value());
}
}

class TestDistinguishedNameParser : public QObject
{
    Q_OBJECT
public:
    explicit TestDistinguishedNameParser(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
private Q_SLOTS:
    // The big set of input/output. Several of the helper functions can be tested independently
    void testParser();
    void testParser_data();
};


void TestDistinguishedNameParser::testParser()
{
    QFETCH(QString, inputData);
    QFETCH(QGpgME::DN::AttributeList, expectedResult);

    auto result = QGpgME::DN{inputData};
    QCOMPARE(std::distance(result.begin(), result.end()), expectedResult.size());
    QGpgME::DN::AttributeList resultAsList;
    for (auto it : result) {
        resultAsList.append(it);
    }
    QCOMPARE(resultAsList,expectedResult);
}

void TestDistinguishedNameParser::testParser_data()
{
    QTest::addColumn<QString>("inputData");
    QTest::addColumn<QGpgME::DN::AttributeList>("expectedResult");

    QTest::newRow("empty") << QString {} << QGpgME::DN::AttributeList {};
    QTest::newRow("CN=Simple") << u"CN=Simple"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Simple"_s}};
    QTest::newRow("CN=Name with spaces") << u"CN=Name with spaces"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Name with spaces"_s}};
    QTest::newRow("CN=Simple,O=Silly") << u"CN=Simple,O=Silly"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Simple"_s}, QGpgME::DN::Attribute{u"O"_s, u"Silly"_s}};
    QTest::newRow("CN=Steve Kille,O=Isode Limited,C=GB") << u"CN=Steve Kille,O=Isode Limited,C=GB"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Steve Kille"_s}, QGpgME::DN::Attribute{u"O"_s, u"Isode Limited"_s}, QGpgME::DN::Attribute{u"C"_s, u"GB"_s}};
    QTest::newRow("CN=some.user@example.com, O=MyCompany, L=San Diego,ST=California, C=US")
        << u"CN=some.user@example.com, O=MyCompany, L=San Diego,ST=California, C=US"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"some.user@example.com"_s}, QGpgME::DN::Attribute{u"O"_s, u"MyCompany"_s}, QGpgME::DN::Attribute{u"L"_s, u"San Diego"_s}, QGpgME::DN::Attribute{u"SP"_s, u"California"_s}, QGpgME::DN::Attribute{u"C"_s, u"US"_s}}; // Note. WE convert ST to SP for reasons. See dn.cpp
    QTest::newRow("Multi valued") << u"OU=Sales+CN=J. Smith,O=Widget Inc.,C=US"_s
                                  << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"OU"_s, u"Sales"_s}, QGpgME::DN::Attribute{u"CN"_s, u"J. Smith"_s}, QGpgME::DN::Attribute{u"O"_s, u"Widget Inc."_s}, QGpgME::DN::Attribute{u"C"_s, u"US"_s}}; // This is technically wrong, but probably good enough for now
    QTest::newRow("Escaping comma") << u"CN=L. Eagle,O=Sue\\, Grabbit and Runn,C=GB"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"L. Eagle"_s}, QGpgME::DN::Attribute{u"O"_s, u"Sue, Grabbit and Runn"_s}, QGpgME::DN::Attribute{u"C"_s, u"GB"_s}};
    QTest::newRow("Escaped trailing space") << u"CN=Trailing space\\ "_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Trailing space "_s}};
    QTest::newRow("Escaped quote") << u"CN=Quotation \\\" Mark"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Quotation \" Mark"_s}};

    QTest::newRow("CN=Simple with escaping") << u"CN=S\\69mpl\\65\\7A"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Simplez"_s}};
    QTest::newRow("SN=Lu\\C4\\8Di\\C4\\87") << u"SN=Lu\\C4\\8Di\\C4\\87"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"SN"_s, u"Lučić"_s}};
    QTest::newRow("CN=\"Quoted name\"") << u"CN=\"Quoted name\""_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Quoted name"_s}};
    QTest::newRow("CN=\" Leading and trailing spacees \"") << u"CN=\" Leading and trailing spaces \""_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u" Leading and trailing spaces "_s}};
    QTest::newRow("Comma in quotes") << u"CN=\"Comma, inside\""_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Comma, inside"_s}};
    QTest::newRow("forbidden chars in quotes") << u"CN=\"Forbidden !@#$%&*()<>[]{},.?/\\| chars\""_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Forbidden !@#$%&*()<>[]{},.?/\\| chars"_s}};
    QTest::newRow("Quoted quotation") << u"CN=\"Quotation \\\" Mark\""_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{"CN", u"Quotation \" Mark"_s}};
    QTest::newRow("Quoted quotation multiple") << u"CN=\"Quotation \\\" Mark\\\" Multiples\""_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Quotation \" Mark\" Multiples"_s}};

    QTest::newRow("frompdf1") << u"2.5.4.97=#5553742D49644E722E20444520313233343735323233,CN=TeleSec PKS eIDAS QES CA 5,O=Deutsche Telekom AG,C=DE"_s
                              << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"2.5.4.97"_s, u"USt-IdNr. DE 123475223"_s}, QGpgME::DN::Attribute{u"CN"_s, u"TeleSec PKS eIDAS QES CA 5"_s}, QGpgME::DN::Attribute{u"O"_s, u"Deutsche Telekom AG"_s}, QGpgME::DN::Attribute{u"C"_s, u"DE"_s}};
    QTest::newRow("frompdf1a") << u"2.5.4.97=#5553742d49644e722e20444520313233343735323233,CN=TeleSec PKS eIDAS QES CA 5,O=Deutsche Telekom AG,C=DE"_s
                              << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"2.5.4.97"_s, u"USt-IdNr. DE 123475223"_s}, QGpgME::DN::Attribute{u"CN"_s, u"TeleSec PKS eIDAS QES CA 5"_s}, QGpgME::DN::Attribute{u"O"_s, u"Deutsche Telekom AG"_s}, QGpgME::DN::Attribute{u"C"_s, u"DE"_s}};
    QTest::newRow("frompdf2") << u"2.5.4.5=#34,CN=Koch\\, Werner,2.5.4.42=#5765726E6572,2.5.4.4=#4B6F6368,C=DE"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"SerialNumber"_s, u"4"_s}, QGpgME::DN::Attribute{u"CN"_s, u"Koch, Werner"_s}, QGpgME::DN::Attribute{u"GN"_s, u"Werner"_s}, QGpgME::DN::Attribute{u"SN"_s, u"Koch"_s}, QGpgME::DN::Attribute{u"C"_s, u"DE"_s}};
    QTest::newRow("frompdf2a") << u"2.5.4.5=#34,CN=Koch\\, Werner,oid.2.5.4.42=#5765726E6572,OID.2.5.4.4=#4B6F6368,C=DE"_s
                               << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"SerialNumber"_s, u"4"_s}, QGpgME::DN::Attribute{u"CN"_s, u"Koch, Werner"_s}, QGpgME::DN::Attribute{u"GN"_s, u"Werner"_s}, QGpgME::DN::Attribute{u"SN"_s, u"Koch"_s}, QGpgME::DN::Attribute{u"C"_s, u"DE"_s}};
    QTest::newRow("ends with hex string") << u"2.5.4.5=#34"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"SerialNumber"_s, u"4"_s}};

    // weird spacing
    QTest::newRow("CN =Simple") << u"CN =Simple"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Simple"_s}};
    QTest::newRow("CN= Simple") << u"CN= Simple"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Simple"_s}};
    QTest::newRow("CN=Simple ") << u"CN=Simple "_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Simple"_s}};
    QTest::newRow("CN=Simple,") << u"CN=Simple,"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Simple"_s}};
    QTest::newRow("CN=Simple, O=Silly") << u"CN=Simple, O=Silly"_s << QGpgME::DN::AttributeList {QGpgME::DN::Attribute{u"CN"_s, u"Simple"_s}, QGpgME::DN::Attribute{u"O"_s, u"Silly"_s}};

    // various malformed
    QTest::newRow("CN=Simple\\") << u"CN=Simple\\"_s << QGpgME::DN::AttributeList {};
    QTest::newRow("CN=") << u"CN="_s << QGpgME::DN::AttributeList {};
    QTest::newRow("CN=Simple\\X") << u"CN=Simple\\X"_s << QGpgME::DN::AttributeList {};
    QTest::newRow("CN=Simple, O") << u"CN=Simple, O"_s << QGpgME::DN::AttributeList {};
    QTest::newRow("CN=Sim\"ple") << u"CN=Sim\"ple, O"_s << QGpgME::DN::AttributeList {};
    QTest::newRow("CN=Simple\\a") << u"CN=Simple\\a"_s << QGpgME::DN::AttributeList {};
    QTest::newRow("=Simple") << u"=Simple"_s << QGpgME::DN::AttributeList {};
    QTest::newRow("CN=\"Simple") << u"CN=\"Simple"_s << QGpgME::DN::AttributeList {};
    QTest::newRow("CN=\"Simple\\") << u"CN=\"Simple\\"_s << QGpgME::DN::AttributeList {};
    QTest::newRow("unquoted quotation in quotation") << u"CN=\"Quotation \" Mark\""_s << QGpgME::DN::AttributeList {};
}

QTEST_GUILESS_MAIN(TestDistinguishedNameParser)


#include "t-dn.moc"
