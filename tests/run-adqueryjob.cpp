/*
    run-adqueryjob.cpp

    This file is part of QGpgME's test suite.
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

#include <adqueryjob.h>
#include <adqueryresult.h>
#include <debug.h>
#include <protocol.h>

#include <QCommandLineParser>
#include <QCoreApplication>

#include <gpgme++/context.h>

#include <iostream>

using namespace GpgME;

std::ostream &operator<<(std::ostream &os, const QString &s)
{
    return os << s.toLocal8Bit().constData();
}

struct CommandLineOptions {
    bool asynchronous = false;
    QString attributes;
    QString email;
};

CommandLineOptions parseCommandLine(const QStringList &arguments)
{
    CommandLineOptions options;

    QCommandLineParser parser;
    parser.setApplicationDescription("Test program for ADQueryJob");
    parser.addHelpOption();
    parser.addOptions({
        {"async", "Run the job asynchronously"},
    });
    parser.addPositionalArgument("attributes", "Comma separated list of attributes to ask for");
    parser.addPositionalArgument("email", "User to ask for");

    parser.process(arguments);

    const auto args = parser.positionalArguments();
    if (args.size() != 2) {
        parser.showHelp(1);
    }

    options.asynchronous = parser.isSet("async");
    options.attributes = args[0];
    options.email = args[1];

    return options;
}

static void printResult(const QGpgME::ADQueryResult &result)
{
    if (result.error()) {
        std::cout << "Error: " << result.error().asStdString() << std::endl;
    } else {
        std::cout << "Result: " << result << std::endl;
    }
}

int main(int argc, char **argv)
{
    GpgME::initializeLibrary();

    QCoreApplication app{argc, argv};
    app.setApplicationName("run-adqueryjob");

    const auto options = parseCommandLine(app.arguments());

    Error err;
    std::cout << "Querying AD for attributes " << options.attributes << " of user with email " << options.email << std::endl;

    auto job = std::unique_ptr<QGpgME::ADQueryJob>(QGpgME::openpgp()->adQueryJob());
    if (!job) {
        std::cerr << "Error: Could not create job to query AD" << std::endl;
        return 1;
    }
    const QString filter = QStringLiteral("(&(objectcategory=person)(objectclass=user) (|(userPrincipalName=%1) (mail=%1)))").arg(options.email);
    if (options.asynchronous) {
        QObject::connect(job.get(), &QGpgME::ADQueryJob::result, &app, [](const QGpgME::ADQueryResult &result, const QString &, const GpgME::Error &) {
            printResult(result);
            qApp->quit();
        });
        err = job->start(filter, options.attributes.split(u','), QGpgME::ADQueryOption::SubstituteVariables);
        if (err) {
            std::cerr << "Error: " << err << std::endl;
            return 1;
        }
        job.release(); // NOLINT(bugprone-unused-return-value); job will delete itself when it's done
        return app.exec();
    } else {
        printResult(job->exec(filter, options.attributes.split(u','), QGpgME::ADQueryOption::SubstituteVariables));
    }
}
