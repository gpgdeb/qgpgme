/*
    dn.cpp

    This file is part of qgpgme, the Qt API binding for gpgme
    Copyright (c) 2004 Klarälvdalens Datakonsult AB
    Copyright (c) 2016 by Bundesamt für Sicherheit in der Informationstechnik
    Software engineering by Intevation GmbH
    Copyright (c) 2023-2025 g10 Code GmbH
    Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>
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

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

// Note: part of this file is also duplicated in kde:okular.git and freedesktop:poppler.git

#include "dn.h"

#include <QByteArray>

#include <gpg-error.h>

#include <optional>
#include <string_view>
#include <vector>

using namespace std::literals;

static const std::vector<std::pair<std::string_view, std::string_view>> &oidmap()
{
    static const std::vector<std::pair<std::string_view, std::string_view>> oidmap_ = {
        // clang-format off
        { "SP",               "ST" }, // hack to show the Sphinx-required/desired SP for
        // StateOrProvince, otherwise known as ST or even S
        // keep them ordered by oid:
        {"NameDistinguisher", "0.2.262.1.10.7.20"   },
        {"EMAIL",             "1.2.840.113549.1.9.1"},
        {"CN",                "2.5.4.3"             },
        {"SN",                "2.5.4.4"             },
        {"SerialNumber",      "2.5.4.5"             },
        {"T",                 "2.5.4.12"            },
        {"D",                 "2.5.4.13"            },
        {"BC",                "2.5.4.15"            },
        {"ADDR",              "2.5.4.16"            },
        {"PC",                "2.5.4.17"            },
        {"GN",                "2.5.4.42"            },
        {"Pseudo",            "2.5.4.65"            },
        // clang-format on
    };
    return oidmap_;
}

class QGpgME::DN::Private
{
public:
    Private() : mRefCount(0) {}
    Private(const Private &other)
        : attributes(other.attributes),
          reorderedAttributes(other.reorderedAttributes),
          order{"CN", "L", "_X_", "OU", "O", "C"},
          mRefCount(0)
    {
    }

    int ref()
    {
        return ++mRefCount;
    }

    int unref()
    {
        if (--mRefCount <= 0) {
            delete this;
            return 0;
        } else {
            return mRefCount;
        }
    }

    int refCount() const
    {
        return mRefCount;
    }

    DN::Attribute::List attributes;
    DN::Attribute::List reorderedAttributes;
    QStringList order;
private:
    int mRefCount;
};

namespace detail
{

static std::string_view removeLeadingSpaces(std::string_view view)
{
    auto pos = view.find_first_not_of(' ');
    if (pos == std::string_view::npos) {
        return {};
    }
    return view.substr(pos);
}

static std::string_view removeTrailingSpaces(std::string_view view)
{
    auto pos = view.find_last_not_of(' ');
    if (pos == std::string_view::npos) {
        return {};
    }
    return view.substr(0, pos + 1);
}

static unsigned char xtoi(unsigned char c)
{
    if (c <= '9') {
        return c - '0';
    }
    if (c <= 'F') {
        return c - 'A' + 10;
    }
    return c - 'a' + 10;
}

static unsigned char xtoi(unsigned char first, unsigned char second)
{
    return 16 * xtoi(first) + xtoi(second);
}

// Parses a hex string into actual content
static std::optional<std::string> parseHexString(std::string_view view)
{
    const auto size = view.size();
    if (size == 0 || (size % 2 == 1)) {
        return {};
    }
    // It is only supposed to be called with actual hex strings
    // but this is just to be extra sure
    auto endHex = view.find_first_not_of("1234567890abcdefABCDEF"sv);
    if (endHex != std::string_view::npos) {
        return {};
    }
    std::string result;
    result.reserve(size / 2);
    for (size_t i = 0; i < (size - 1); i += 2) {
        result.push_back(xtoi(view[i], view[i + 1]));
    }
    return result;
}

static std::string_view attributeNameForOID(std::string_view oid)
{
    if (oid.substr(0, 4) == "OID."sv || oid.substr(0, 4) == "oid."sv) { // c++20 has starts_with. we don't have that yet.
        oid.remove_prefix(4);
    }
    for (const auto &m : oidmap()) {
        if (oid == m.second) {
            return m.first;
        }
    }
    return {};
}

/* Parse a DN attribute key/value pair and return the remaining unparsed string and the
   parsed attribute. This is not a validating parser and it does not support any old-stylish
   syntax; gpgme is expected to return only rfc2253 compatible strings. */
static std::pair<std::optional<std::string_view>, std::pair<std::string, std::string>> parse_dn_part(std::string_view stringv)
{
    std::pair<std::string, std::string> dnPair;
    auto separatorPos = stringv.find_first_of('=');
    if (separatorPos == 0 || separatorPos == std::string_view::npos) {
        return {}; /* empty key */
    }

    std::string_view key = stringv.substr(0, separatorPos);
    key = removeTrailingSpaces(key);
    // map OIDs to their names:
    if (auto name = attributeNameForOID(key); !name.empty()) {
        key = name;
    }

    dnPair.first = std::string {key};
    stringv = removeLeadingSpaces(stringv.substr(separatorPos + 1));
    if (stringv.empty()) {
        return {};
    }

    if (stringv.front() == '#') {
        /* hexstring */
        stringv.remove_prefix(1);
        auto endHex = stringv.find_first_not_of("1234567890abcdefABCDEF"sv);
        auto value = parseHexString(stringv.substr(0, endHex));
        if (!value.has_value()) {
            return {};
        }
        stringv.remove_prefix(endHex);
        dnPair.second = value.value();
    } else if (stringv.front() == '"') {
        stringv.remove_prefix(1);
        std::string value;
        bool stop = false;
        while (!stringv.empty() && !stop) {
            switch (stringv.front()) {
            case '\\': {
                if (stringv.size() < 2) {
                    return {};
                }
                if (stringv[1] == '"') {
                    value.push_back('"');
                    stringv.remove_prefix(2);
                } else {
                    // it is a bit unclear in rfc2253 if escaped hex chars should
                    // be decoded inside quotes. Let's just forward them verbatim
                    // for now
                    value.push_back(stringv.front());
                    value.push_back(stringv[1]);
                    stringv.remove_prefix(2);
                }
                break;
            }
            case '"': {
                stop = true;
                stringv.remove_prefix(1);
                break;
            }
            default: {
                value.push_back(stringv.front());
                stringv.remove_prefix(1);
            }
            }
        }
        if (!stop) {
            // we have reached end of string, but never an actual ", so error out
            return {};
        }
        dnPair.second = value;
    } else {
        std::string value;
        bool stop = false;
        bool lastAddedEscapedSpace = false;
        while (!stringv.empty() && !stop) {
            switch (stringv.front()) {
            case '\\': // escaping
            {
                stringv.remove_prefix(1);
                if (stringv.empty()) {
                    return {};
                }
                switch (stringv.front()) {
                case ',':
                case '=':
                case '+':
                case '<':
                case '>':
                case '#':
                case ';':
                case '\\':
                case '"':
                case ' ': {
                    if (stringv.front() == ' ') {
                        lastAddedEscapedSpace = true;
                    } else {
                        lastAddedEscapedSpace = false;
                    }
                    value.push_back(stringv.front());
                    stringv.remove_prefix(1);
                    break;
                }
                default: {
                    if (stringv.size() < 2) {
                        // this should be double hex-ish, but isn't.
                        return {};
                    }
                    if (std::isxdigit(stringv.front()) && std::isxdigit(stringv[1])) {
                        lastAddedEscapedSpace = false;
                        value.push_back(xtoi(stringv.front(), stringv[1]));
                        stringv.remove_prefix(2);
                        break;
                    } else {
                        // invalid escape
                        return {};
                    }
                }
                }
                break;
            }
            case '"':
                // unescaped " in the middle; not allowed
                return {};
            case ',':
            case '=':
            case '+':
            case '<':
            case '>':
            case '#':
            case ';': {
                stop = true;
                break;
            }
            default:
                lastAddedEscapedSpace = false;
                value.push_back(stringv.front());
                stringv.remove_prefix(1);
            }
        }
        if (lastAddedEscapedSpace) {
            dnPair.second = value;
        } else {
            dnPair.second = std::string{removeTrailingSpaces(value)};
        }
    }
    return {stringv, dnPair};
}
}

using Result = std::vector<std::pair<std::string, std::string>>;

/* Parse a DN and return an array-ized one.  This is not a validating
   parser and it does not support any old-stylish syntax; gpgme is
   expected to return only rfc2253 compatible strings. */
static Result parseString(std::string_view string)
{
    Result result;
    while (!string.empty()) {
        string = detail::removeLeadingSpaces(string);
        if (string.empty()) {
            break;
        }

        auto [partResult, dnPair] = detail::parse_dn_part(string);
        if (!partResult.has_value()) {
            return {};
        }

        if (dnPair.first.size() && dnPair.second.size()) {
            result.emplace_back(std::move(dnPair));
        }

        string = detail::removeLeadingSpaces(partResult.value());
        if (string.empty()) {
            break;
        }
        switch (string.front()) {
        case ',':
        case ';':
        case '+':
            string.remove_prefix(1);
            break;
        default:
            // some unexpected characters here
            return {};
        }
    }
    return result;
}

QGpgME::DN::AttributeList parse_dn(std::string_view view) {
    const auto parsed = parseString(view);
    QGpgME::DN::AttributeList list;
    list.reserve(parsed.size());
    for (auto &item : parsed) {
        list.append(QGpgME::DN::Attribute(QString::fromStdString(item.first),
                                          QString::fromStdString(item.second)));
    }
    return  list;
}


static QString dn_escape(const QString &s)
{
    QString result;
    for (unsigned int i = 0, end = s.length(); i != end; ++i) {
        const QChar ch = s[i];
        switch (ch.unicode()) {
        case ',':
        case '+':
        case '"':
        case '\\':
        case '<':
        case '>':
        case ';':
            result += QLatin1Char('\\');
        // fall through
        default:
            result += ch;
        }
    }
    return result;
}

static QStringList
listAttributes(const QVector<QGpgME::DN::Attribute> &dn)
{
    QStringList result;
    result.reserve(dn.size());
    for (const auto &attribute : dn) {
        if (!attribute.name().isEmpty() && !attribute.value().isEmpty()) {
            result.push_back(attribute.name().trimmed() + QLatin1Char('=') + dn_escape(attribute.value().trimmed()));
        }
    }
    return result;
}

static QString
serialise(const QVector<QGpgME::DN::Attribute> &dn, const QString &sep)
{
    return listAttributes(dn).join(sep);
}

static QGpgME::DN::Attribute::List
reorder_dn(const QGpgME::DN::Attribute::List &dn, const QStringList &attrOrder)
{
    QGpgME::DN::Attribute::List unknownEntries;
    QGpgME::DN::Attribute::List result;
    unknownEntries.reserve(dn.size());
    result.reserve(dn.size());

    // find all unknown entries in their order of appearance
    for (QGpgME::DN::const_iterator it = dn.begin(); it != dn.end(); ++it)
        if (!attrOrder.contains((*it).name())) {
            unknownEntries.push_back(*it);
        }

    // process the known attrs in the desired order
    for (QStringList::const_iterator oit = attrOrder.begin(); oit != attrOrder.end(); ++oit)
        if (*oit == QLatin1String("_X_")) {
            // insert the unknown attrs
            std::copy(unknownEntries.begin(), unknownEntries.end(),
                      std::back_inserter(result));
            unknownEntries.clear(); // don't produce dup's
        } else {
            for (QGpgME::DN::const_iterator dnit = dn.begin(); dnit != dn.end(); ++dnit)
                if ((*dnit).name() == *oit) {
                    result.push_back(*dnit);
                }
        }

    return result;
}

//
//
// class DN
//
//

QGpgME::DN::DN()
{
    d = new Private();
    d->ref();
}

QGpgME::DN::DN(const QString &dn)
{
    d = new Private();
    d->ref();
    d->attributes = parse_dn(dn.toStdString());
}

QGpgME::DN::DN(const char *utf8DN)
{
    d = new Private();
    d->ref();
    if (utf8DN) {
        d->attributes = parse_dn(std::string_view(utf8DN, strlen(utf8DN)));
    }
}

QGpgME::DN::DN(const DN &other)
    : d(other.d)
{
    if (d) {
        d->ref();
    }
}

QGpgME::DN::~DN()
{
    if (d) {
        d->unref();
    }
}

const QGpgME::DN &QGpgME::DN::operator=(const DN &that)
{
    if (this->d == that.d) {
        return *this;
    }

    if (that.d) {
        that.d->ref();
    }
    if (this->d) {
        this->d->unref();
    }

    this->d = that.d;

    return *this;
}

QString QGpgME::DN::prettyDN() const
{
    if (!d) {
        return QString();
    }
    if (d->reorderedAttributes.empty()) {
        d->reorderedAttributes = reorder_dn(d->attributes, d->order);
    }
    return serialise(d->reorderedAttributes, QStringLiteral(","));
}

QString QGpgME::DN::dn() const
{
    return d ? serialise(d->attributes, QStringLiteral(",")) : QString();
}

QString QGpgME::DN::dn(const QString &sep) const
{
    return d ? serialise(d->attributes, sep) : QString();
}

QStringList QGpgME::DN::prettyAttributes() const
{
    if (!d) {
        return {};
    }

    if (d->reorderedAttributes.empty()) {
        d->reorderedAttributes = reorder_dn(d->attributes, d->order);
    }
    return listAttributes(d->reorderedAttributes);
}

// static
QString QGpgME::DN::escape(const QString &value)
{
    return dn_escape(value);
}

void QGpgME::DN::detach()
{
    if (!d) {
        d = new QGpgME::DN::Private();
        d->ref();
    } else if (d->refCount() > 1) {
        QGpgME::DN::Private *d_save = d;
        d = new QGpgME::DN::Private(*d);
        d->ref();
        d_save->unref();
    }
}

void QGpgME::DN::append(const Attribute &attr)
{
    detach();
    d->attributes.push_back(attr);
    d->reorderedAttributes.clear();
}

QString QGpgME::DN::operator[](const QString &attr) const
{
    if (!d) {
        return QString();
    }
    const QString attrUpper = attr.toUpper();
    for (QVector<Attribute>::const_iterator it = d->attributes.constBegin();
            it != d->attributes.constEnd(); ++it)
        if ((*it).name() == attrUpper) {
            return (*it).value();
        }
    return QString();
}

static QVector<QGpgME::DN::Attribute> empty;

QGpgME::DN::const_iterator QGpgME::DN::begin() const
{
    return d ? d->attributes.constBegin() : empty.constBegin();
}

QGpgME::DN::const_iterator QGpgME::DN::end() const
{
    return d ? d->attributes.constEnd() : empty.constEnd();
}

void QGpgME::DN::setAttributeOrder (const QStringList &order) const
{
    d->order = order;
}

const QStringList & QGpgME::DN::attributeOrder () const
{
    return d->order;
}
