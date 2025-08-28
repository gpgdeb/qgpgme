/*
    signjob.cpp

    This file is part of qgpgme, the Qt API binding for gpgme
    Copyright (c) 2023 g10 Code GmbH
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

#include "signjob.h"
#include "signjob_p.h"

using namespace QGpgME;

SignJob::SignJob(std::unique_ptr<SignJobPrivate> dd, QObject *parent)
    : Job{std::move(dd), parent}
{
}

SignJob::~SignJob() = default;

void SignJob::setSigners(const std::vector<GpgME::Key> &signers)
{
    Q_D(SignJob);
    d->m_signers = signers;
}

std::vector<GpgME::Key> SignJob::signers() const
{
    Q_D(const SignJob);
    return d->m_signers;
}

void SignJob::setInputFile(const QString &path)
{
    Q_D(SignJob);
    d->m_inputFilePath = path;
}

QString SignJob::inputFile() const
{
    Q_D(const SignJob);
    return d->m_inputFilePath;
}

void SignJob::setOutputFile(const QString &path)
{
    Q_D(SignJob);
    d->m_outputFilePath = path;
}

QString SignJob::outputFile() const
{
    Q_D(const SignJob);
    return d->m_outputFilePath;
}

void SignJob::setSigningFlags(GpgME::SignatureMode flags)
{
    Q_D(SignJob);
    d->m_signingFlags = static_cast<GpgME::SignatureMode>(flags | GpgME::SignFile);
}

GpgME::SignatureMode SignJob::signingFlags() const
{
    Q_D(const SignJob);
    return d->m_signingFlags;
}

void SignJob::setAppendSignature(bool append)
{
    Q_D(SignJob);
    d->m_appendSignature = append;
}

bool SignJob::appendSignatureEnabled() const
{
    Q_D(const SignJob);
    return d->m_appendSignature;
}

#include "moc_signjob.cpp"
