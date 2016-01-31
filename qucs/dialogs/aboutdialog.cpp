/*
 * aboutdialog.cpp - customary about dialog showing various info
 *
 * Copyright (C) 2015-2016, Qucs team (see AUTHORS file)
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
#include <QApplication>
#include <QDebug>


AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
  currAuths = {{
    "Guilherme Brondani Torri - " + tr("GUI programmer, Verilog-A dynamic loader"),
    "Mike Brinson - " + tr("testing, modelling and documentation, tutorial contributor"),
    "Richard Crozier - " + tr("testing, modelling, Octave."),
    "Bastien Roucaries - " + tr("bondwire and rectangular waveguide model implementation"),
    "Frans Schreuder - " + tr("GUI programmer, release"),
    "Vadim Kuznetsov - " + tr("filter synthesis (qucs-activefilter), SPICE integration (NGSPICE, Xyce)"),
    "Claudio Girardi - " + tr("testing, general fixes"),
    "Felix Salfelder - " + tr("refactoring, modularity"),
    "Andr\xe9s Mart\xednez Mera - " + tr("RF design tools")
  }};
  
  prevDevs = {{
      "Michael Margraf - " + tr("founder of the project, GUI programmer"),
      "Stefan Jahn - " + tr("Programmer of simulator"),
      "Jens Flucke - " + tr("webpages and translator"),
      "Raimund Jacob - " + tr("tester and applyer of Stefan's patches, author of documentation"),
      "Vincent Habchi - " + tr("coplanar line and filter synthesis code, documentation contributor"),
      "Toyoyuki Ishikawa - " + tr("some filter synthesis code and attenuator synthesis"),
      "Gopala Krishna A - " + tr("GUI programmer, Qt4 porter"),
      "Helene Parruitte - " + tr("programmer of the Verilog-AMS interface"),
      "Gunther Kraut - " + tr("equation solver contributions, exponential sources, author of documentation"),
      "Andrea Zonca - " + tr("temperature model for rectangular waveguide"),
      "Clemens Novak - " + tr("GUI programmer"),
      "You-Tang Lee (YodaLee) - " + tr("GUI programmer, Qt4 porter")
  }};

  trAuths = {{
    tr("German by") + " Stefan Jahn",
    tr("Polish by") + " Dariusz Pienkowski",
    tr("Romanian by") + " Radu Circa",
    tr("French by") + " Vincent Habchi, F5RCS",
    tr("Portuguese by") + " Luciano Franca, Helio de Sousa, Guilherme Brondani Torri",
    tr("Spanish by") + " Jose L. Redrejo Rodriguez",
    tr("Japanese by") + " Toyoyuki Ishikawa",
    tr("Italian by") + " Giorgio Luparia, Claudio Girardi",
    tr("Hebrew by") + " Dotan Nahum",
    tr("Swedish by") + " Markus Gothe, Peter Landgren",
    tr("Turkish by") + " Onur Cobanoglu, Ozgur Cobanoglu",
    tr("Hungarian by") + " Jozsef Bus",
    tr("Russian by") + " Igor Gorbounov",
    tr("Czech by") + " Marek Straka,Martin Stejskal",
    tr("Catalan by") + " Antoni Subirats",
    tr("Ukrainian by") + " Dystryk",
    tr("Arabic by") + " Chabane Noureddine",
    tr("Kazakh by") + " Erbol Keshubaev"
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
  vl->addWidget(new QLabel(tr("Copyright (C)")+" 2011-2016 Qucs Team\n"+
			   tr("Copyright (C)")+" 2003-2009 Michael Margraf"));

  lbl = new QLabel("\nThis is free software; see the source for copying conditions."
		   "\nThere is NO warranty; not even for MERCHANTABILITY or "
		   "\nFITNESS FOR A PARTICULAR PURPOSE.\n");
  lbl->setAlignment(Qt::AlignHCenter);
  all->addWidget(lbl);

  QTabWidget *t = new QTabWidget();
  all->addWidget(t);
  connect(t, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));

  authorsBrowser = new QTextBrowser;
  // the Ctrl-Wheel event we would like to filter is handled by the viewport
  authorsBrowser->viewport()->installEventFilter(this);
  trBrowser = new QTextBrowser;
  trBrowser->viewport()->installEventFilter(this);

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
  supportBrowser->viewport()->installEventFilter(this);
  supportBrowser->setOpenExternalLinks(true);
  supportBrowser->setHtml(supportText);

  QString licenseText;
  licenseText = "Qucs is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2, or (at your option) any later version.<br/><br/>This software is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details..<br/><br/> You should have received a copy of the GNU General Public License along with Qucs, see the file COPYING. If not see <a href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a> or write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,Boston, MA 02110-1301, USA.";

  QTextBrowser *licenseBrowser = new QTextBrowser;
  licenseBrowser->viewport()->installEventFilter(this);
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

// event filter to remove the Ctrl-Wheel (text zoom) event
bool AboutDialog::eventFilter(QObject *obj, QEvent *event) {
  if ((event->type() == QEvent::Wheel) &&
      (QApplication::keyboardModifiers() & Qt::ControlModifier )) {    
    return true; // eat Ctrl-Wheel event
  } else {
    // pass the event on to the parent class
    return QDialog::eventFilter(obj, event);
  }
}


