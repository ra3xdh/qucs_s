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

#include <array>
#include <algorithm>
#include <random>

#include <iostream>
#include <string>

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
#include <QDebug>


AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
  currAuths = {{
    tr("Guilherme Brondani Torri - GUI programmer, Verilog-A dynamic loader"),
    tr("Mike Brinson - testing, modelling and documentation, tutorial contributor"),
    tr("Richard Crozier - testing, modelling, Octave."),
    tr("Bastien Roucaries - bondwire and rectangular waveguide model implementation"),
    tr("Frans Schreuder - GUI programmer, release"),
    tr("Clemens Novak - GUI programmer"),
    tr("Vadim Kuznetsov - filter synthesis (qucs-activefilter), SPICE integration (NGSPICE, Xyce)"),
    tr("You-Tang Lee (YodaLee) - GUI programmer, Qt4 porter"),
    tr("Claudio Girardi - testing, general fixes")
  }};
  
  prevDevs = {{
    tr("Michael Margraf - founder of the project, GUI programmer"),
    tr("Stefan Jahn - Programmer of simulator"),
    tr("Jens Flucke - webpages and translator"),
    tr("Raimund Jacob - tester and applyer of Stefan's patches, author of documentation"),
    tr("Vincent Habchi - coplanar line and filter synthesis code, documentation contributor"),
    tr("Toyoyuki Ishikawa - some filter synthesis code and attenuator synthesis"),
    tr("Gopala Krishna A - GUI programmer, Qt4 porter"),
    tr("Helene Parruitte - programmer of the Verilog-AMS interface"),
    tr("Gunther Kraut - equation solver contributions, exponential sources, author of documentation"),
    tr("Andrea Zonca - temperature model for rectangular waveguide")
  }};

  trAuths = {{
    tr("German by Stefan Jahn"),
    tr("Polish by Dariusz Pienkowski"),
    tr("Romanian by Radu Circa"),
    tr("French by Vincent Habchi, F5RCS"),
    tr("Portuguese by Luciano Franca, Helio de Sousa, Guilherme Brondani Torri"),
    tr("Spanish by Jose L. Redrejo Rodriguez"),
    tr("Japanese by Toyoyuki Ishikawa"),
    tr("Italian by Giorgio Luparia and Claudio Girardi"),
    tr("Hebrew by Dotan Nahum"),
    tr("Swedish by Markus Gothe and Peter Landgren"),
    tr("Turkish by Onur and Ozgur Cobanoglu"),
    tr("Hungarian by Jozsef Bus"),
    tr("Russian by Igor Gorbounov"),
    tr("Czech by Marek Straka and Martin Stejskal"),
    tr("Catalan by Antoni Subirats"),
    tr("Ukrainian by Dystryk"),
    tr("Arabic by Chabane Noureddine"),
    tr("Kazakh by Erbol Keshubaev")
  }};
  

  std::shuffle(currAuths.begin(), currAuths.end(), rng);

  QLabel *lbl;

  setWindowTitle(tr("About Qucs"));

  all = new QVBoxLayout(this);
  //all->setContentsMargins(0,0,0,0);
  //all->setSpacing(0);

  QLabel *iconLabel = new QLabel();
  iconLabel->setPixmap(QPixmap(QString(":/bitmaps/hicolor/128x128/apps/qucs.png")));

  QWidget *hbox = new QWidget();
  QHBoxLayout *hl = new QHBoxLayout(hbox);
  hl->setContentsMargins(0,0,0,0);

  hl->addWidget(iconLabel);
  all->addWidget(hbox);
 
  QWidget *vbox = new QWidget();
  QVBoxLayout *vl = new QVBoxLayout(vbox);
  //vl->setContentsMargins(0,0,0,0);
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
		   "\nFITNESS FOR A PARTICULAR PURPOSE.\n");
  lbl->setAlignment(Qt::AlignHCenter);
  all->addWidget(lbl);

  QTabWidget *t = new QTabWidget();
  all->addWidget(t);
  connect(t, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));
  

  authorsBrowser = new QTextBrowser;
  trBrowser = new QTextBrowser;



  QString supportText;
  // link to home page, help mailing list, IRC ?
  supportText = tr("Home Page") + " : <a href='http://qucs.sourceforge.net/'>http://qucs.sourceforge.net/</a><br/>"+
    tr("Documentation start page") + " : <a href='http://qucs.sourceforge.net/docs.html'>http://qucs.sourceforge.net/docs.html</a><br/>" +
    tr("Components reference manual") + " : <a href='http://qucs.github.io/qucs-manual/'>http://qucs.github.io/qucs-manual/</a><br/><br/>" +
    tr("If you need help on using Qucs, please join the") + "<br/>" +
    tr("help mailing list") + " : <a href='https://sourceforge.net/p/qucs/mailman/qucs-help/'>" + tr("qucs-help on SourceForge") + "</a><br/>" +
    "<small>(" + tr("please attach the schematic you are having problems with") + ")</small><br/><br/>" + 
    // use http://webchat.freenode.net/?channels=qucs ?
    tr("IRC general discussion channel") + " : <a href='irc://irc.freenode.net/qucs'>#qucs on freenode.net</a><br/><br/>" +
    tr("Additional resources") + " : <a href='https://github.com/Qucs/qucs#resources'>https://github.com/Qucs/qucs#resources</a>";

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
  hlBtn->setContentsMargins(0,0,0,0);
  all->addWidget(hbBtn);

  QPushButton *okButton = new QPushButton(tr("&OK"), parent);
  okButton->setFocus();
  connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
  hlBtn->addStretch();
  hlBtn->addWidget(okButton);

  setAuthorsText();
  setTrText();
  prevTab = 0; // first Tab is selected by default
}

void AboutDialog::currentChangedSlot(int index) {
  if (prevTab == 0) { // deselected tab with current and previous authors
    // shuffle them
    std::shuffle(currAuths.begin(), currAuths.end(), rng);
    std::shuffle(prevDevs.begin(), prevDevs.end(), rng);
    setAuthorsText();
  } else if (prevTab == 1) {// deselected tab with translators
    std::shuffle(trAuths.begin(), trAuths.end(), rng);
    setTrText();
  }
  
  prevTab = index;
}

void AboutDialog::setAuthorsText() {
 
  QString authorsText;
  authorsText = tr("Current Qucs Team:") + "<ul>";
  
  for(QString& tStr : currAuths) {
    authorsText += ("<li>" + tStr + "</li>");
  }
  authorsText += "</ul>";
  authorsText += tr("Previous Developers") + "<ul>";
  for(QString& tStr : prevDevs) {
    authorsText += ("<li>" + tStr + "</li>");
  }
  authorsText += "</ul>";
  
  authorsBrowser->setHtml(authorsText);	
}

void AboutDialog::setTrText() {
  QString trText;
  trText = tr("GUI translations :") + "<ul>";
  for(QString& tStr : trAuths) {
    trText += ("<li>" + tStr + "</li>");
  }
  trText += "</ul>";
  
  trBrowser->setHtml(trText);	
}
