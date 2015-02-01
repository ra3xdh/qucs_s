/*
 * aboutdialog.cpp - customary about dialog showing various info
 *
 * Copyright (C) 2015, Qucs team (see AUTHORS file)
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*!
 * \file aboutdialog.cpp
 * \brief Implementation of the About dialog
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "aboutdialog.h"

#include <QObject>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QLabel>
#include <QPushButton>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
  QLabel *lbl;

  setWindowTitle(tr("About Qucs"));

  all = new QVBoxLayout(this);

  QLabel *iconLabel = new QLabel();
  iconLabel->setPixmap(QPixmap(QString(":/bitmaps/hicolor/128x128/apps/qucs.png")));

  QWidget *hbox = new QWidget();
  QHBoxLayout *hl = new QHBoxLayout(hbox);

  hl->addWidget(iconLabel);
  all->addWidget(hbox);

  QWidget *vbox = new QWidget();
  QVBoxLayout *vl = new QVBoxLayout(vbox);
  hl->addWidget(vbox);

  QString versionText;
  versionText = tr("Version")+" "+PACKAGE_VERSION+
#ifdef GIT
    " ("+GIT+") " +
#endif
    "\n";

  vl->addWidget(new QLabel("<span style='font-size:x-large; font-weight:bold;'>Quite Universal Circuit Simulator</span>"));
  lbl = new QLabel(versionText);
  lbl->setAlignment(Qt::AlignHCenter);
  vl->addWidget(lbl);
  vl->addWidget(new QLabel(tr("Copyright (C)")+" 2003-2009 "+
			   tr("by Michael Margraf")+"\n"+
			   tr("Copyright (C)")+" 2011-2015 "+
			   tr("Qucs Team")));

  lbl = new QLabel("\nThis is free software; see the source for copying conditions."
		   "\nThere is NO warranty; not even for MERCHANTABILITY or "
		   "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
  lbl->setAlignment(Qt::AlignHCenter);
  all->addWidget(lbl);

  QTabWidget *t = new QTabWidget();
  all->addWidget(t);

  QString authorsText;
  authorsText =
    tr("Current Qucs Team:") +
    "<ul>" +
    "<li>" + tr("Guilherme Brondani Torri - GUI programmer, Verilog-A dynamic loader") + "</li>" +
    "<li>" + tr("Mike Brinson - testing, modelling and documentation, tutorial contributor") + "</li>" +
    "<li>" + tr("Richard Crozier - testing, modelling, Octave.") + "</li>" +
    "<li>" + tr("Bastien Roucaries - bondwire and rectangular waveguide model implementation") + "</li>" +
    "<li>" + tr("Frans Schreuder - GUI programmer, release") + "</li>" +
    "<li>" + tr("Clemens Novak - GUI programmer") + "</li>" +
    "<li>" + tr("Vadim Kuznetsov - filter synthesis (qucs-activefilter), SPICE integration (NGSPICE, Xyce)") + "</li>" +
    "<li>" + tr("You-Tang Lee (YodaLee) - GUI programmer, Qt4 porter") + "</li>" +
    "</ul>" +
    tr("Previous Developers") +
    "<ul>" +
    "<li>" + tr("Michael Margraf - founder of the project, GUI programmer") + "</li>" +
    "<li>" + tr("Stefan Jahn - Programmer of simulator") + "</li>" +
    "<li>" + tr("Jens Flucke - webpages and translator") + "</li>" +
    "<li>" + tr("Raimund Jacob - tester and applyer of Stefan's patches, author of documentation") + "</li>" +
    "<li>" + tr("Vincent Habchi - coplanar line and filter synthesis code, documentation contributor") + "</li>" +
    "<li>" + tr("Toyoyuki Ishikawa - some filter synthesis code and attenuator synthesis") + "</li>" +
    "<li>" + tr("Gopala Krishna A - GUI programmer, Qt4 porter") + "</li>" +
    "<li>" + tr("Helene Parruitte - programmer of the Verilog-AMS interface") + "</li>" +
    "<li>" + tr("Gunther Kraut - equation solver contributions, exponential sources, author of documentation") + "</li>" +
    "<li>" + tr("Andrea Zonca - temperature model for rectangular waveguide") + "</li>" +
    "</ul>";

  QTextBrowser *authorsBrowser = new QTextBrowser;

  authorsBrowser->setHtml(authorsText);

  QString thanksText;
  thanksText = "TBD";

  QString trText;
  trText = tr("GUI translations :") +
    "<ul>" +
    "<li>" + tr("German by Stefan Jahn") + "</li>"+
    "<li>" + tr("Polish by Dariusz Pienkowski") + "</li>" +
    "<li>" + tr("Romanian by Radu Circa") + "</li>" +
    "<li>" + tr("French by Vincent Habchi, F5RCS") + "</li>" +
    "<li>" + tr("Portuguese by Luciano Franca, Helio de Sousa, Guilherme Brondani Torri") + "</li>" +
    "<li>" + tr("Spanish by Jose L. Redrejo Rodriguez") + "</li>" +
    "<li>" + tr("Japanese by Toyoyuki Ishikawa") + "</li>" +
    "<li>" + tr("Italian by Giorgio Luparia and Claudio Girardi") + "</li>" +
    "<li>" + tr("Hebrew by Dotan Nahum") + "</li>" +
    "<li>" + tr("Swedish by Markus Gothe and Peter Landgren") + "</li>" +
    "<li>" + tr("Turkish by Onur and Ozgur Cobanoglu") + "</li>" +
    "<li>" + tr("Hungarian by Jozsef Bus") + "</li>" +
    "<li>" + tr("Russian by Igor Gorbounov") + "</li>" +
    "<li>" + tr("Czech by Marek Straka and Martin Stejskal") + "</li>" +
    "<li>" + tr("Catalan by Antoni Subirats") + "</li>" +
    "<li>" + tr("Ukrainian by Dystryk") + "</li>" +
    "<li>" + tr("Arabic by Chabane Noureddine") + "</li>" +
    "<li>" + tr("Kazakh by Erbol Keshubaev") + "</li>" +
    "</ul>";

  QTextBrowser *trBrowser = new QTextBrowser;
  trBrowser->setHtml(trText);

  QString supportText;
  // link to home page, help mailing list, IRC ?
  supportText = tr("Home Page") + " : <a href='http://qucs.sourceforge.net/'>http://qucs.sourceforge.net/</a><br/>"+
    tr("Documentation start page") + " : <a href='http://qucs.sourceforge.net/docs.html'>http://qucs.sourceforge.net/docs.html</a><br/>" +
    tr("Components reference manual") + " : <a href='http://qucs.sourceforge.net/doc/0.0.19/html/index.html'>http://qucs.sourceforge.net/doc/0.0.19/html/index.html</a><br/>" +
    tr("Help mailing list") + " : <a href='https://sourceforge.net/p/qucs/mailman/qucs-help/'>" + tr("qucs-help on SourceForge") + "</a>";

  QTextBrowser *supportBrowser = new QTextBrowser;
  supportBrowser->setOpenExternalLinks(true);
  supportBrowser->setHtml(supportText);

  QString licenseText;
  licenseText = "Qucs is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2, or (at your option) any later version.<br/><br/>This software is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details..<br/><br/> You should have received a copy of the GNU General Public License along with Qucs, see the file COPYING. If not see <a href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a> or write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,Boston, MA 02110-1301, USA.";

  QTextBrowser *licenseBrowser = new QTextBrowser;
  licenseBrowser->setOpenExternalLinks(true);
  licenseBrowser->setHtml(licenseText);

  t->addTab(authorsBrowser, tr("Authors"));
  t->addTab(trBrowser, tr("Translations"));
  t->addTab(supportBrowser, tr("Support"));
  t->addTab(licenseBrowser, tr("License"));

  QWidget *hbBtn = new QWidget();
  QHBoxLayout *hlBtn = new QHBoxLayout(hbBtn);
  all->addWidget(hbBtn);

  QPushButton *okButton = new QPushButton(tr("&OK"), parent);
  okButton->setFocus();
  connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
  hlBtn->addStretch();
  hlBtn->addWidget(okButton);
}
