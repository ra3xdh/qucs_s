/***************************************************************************
                              packagedialog.cpp
                             -------------------
    begin                : Sun Jun 25 2006
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <QtGui>
#include <stdlib.h>

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QDataStream>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "packagedialog.h"
#include "qucs.h"
#include "main.h"


#define HEADER_LENGTH   32
#define CODE_ERROR      0x0000
#define CODE_DIR        0x0010
#define CODE_DIR_END    0x0018
#define CODE_FILE       0x0020
#define CODE_LIBRARY    0x0040


PackageDialog::PackageDialog(QWidget *parent_, bool create_)
			: QDialog(parent_) //, 0, TRUE, Qt::WDestructiveClose)
{
  all = new QVBoxLayout(this);
  all->setMargin(5);
  all->setSpacing(6);

  QHBoxLayout *h2 = new QHBoxLayout();

  if(create_) {  // create or extract package ?
    setWindowTitle(tr("Create Project Package"));

    QHBoxLayout *h1 = new QHBoxLayout();
    all->addLayout(h1);
    QLabel *packLabel = new QLabel(tr("Package:"));
    NameEdit = new QLineEdit();
    QPushButton *ButtBrowse = new QPushButton(tr("Browse"));
    connect(ButtBrowse, SIGNAL(clicked()), SLOT(slotBrowse()));
    h1->addWidget(packLabel);
    h1->addWidget(NameEdit);
    h1->addWidget(ButtBrowse);

    LibraryCheck = new QCheckBox(tr("include user libraries"));
    all->addWidget(LibraryCheck);

    Group = new QGroupBox(tr("Choose projects:"));
    all->addWidget(Group);
    
    QScrollArea *scrollArea = new QScrollArea(Group);
    scrollArea->setWidgetResizable(true);
    
    QWidget *scrollWidget = new QWidget();
    
    QVBoxLayout *checkBoxLayout = new QVBoxLayout();
    scrollWidget->setLayout(checkBoxLayout);
    scrollArea->setWidget(scrollWidget);
    
    QVBoxLayout *areaLayout = new QVBoxLayout();
    areaLayout->addWidget(scrollArea);
    Group->setLayout(areaLayout);
    
    // ...........................................................
    all->addLayout(h2);
    QPushButton *ButtCreate = new QPushButton(tr("Create"));
    h2->addWidget(ButtCreate);
    connect(ButtCreate, SIGNAL(clicked()), SLOT(slotCreate()));
    QPushButton *ButtCancel = new QPushButton(tr("Cancel"));
    h2->addWidget(ButtCancel);
    connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

    // ...........................................................
    // insert all projects
    QStringList PrDirs = QucsSettings.QucsHomeDir.entryList("*", QDir::Dirs, QDir::Name);
    QStringList::iterator it;
    for(it = PrDirs.begin(); it != PrDirs.end(); it++)
       if((*it).right(4) == "_prj"){   // project directories end with "_prj"
         QCheckBox *subCheck = new QCheckBox((*it).left((*it).length()-4));
         checkBoxLayout->addWidget(subCheck);
         BoxList.append(subCheck);
       }
    
    //QColor theColor;
    if(BoxList.isEmpty()) {
      ButtCreate->setEnabled(false);
      //theColor =
      //   (new QLabel(tr("No projects!"), Dia_Box))->paletteBackgroundColor();
      QLabel *noProj = new QLabel(tr("No projects!"));
      checkBoxLayout->addWidget(noProj);              
    }
    //else
    //  theColor = BoxList.current()->paletteBackgroundColor();
    //Dia_Scroll->viewport()->setPaletteBackgroundColor(theColor);
  }

  else {  // of "if(create_)"
    setWindowTitle(tr("Extract Project Package"));

    MsgText = new QTextEdit(this);
    MsgText->setTextFormat(Qt::PlainText);
    MsgText->setWordWrapMode(QTextOption::NoWrap);
    MsgText->setReadOnly(true);
    all->addWidget(MsgText);

    all->addLayout(h2);
    h2->addStretch(5);
    ButtClose = new QPushButton(tr("Close"));
    h2->addWidget(ButtClose);
    ButtClose->setDisabled(true);
    connect(ButtClose, SIGNAL(clicked()), SLOT(accept()));

    resize(400, 250);
  }
}

PackageDialog::~PackageDialog()
{
  delete all;
}

// ---------------------------------------------------------------
void PackageDialog::slotBrowse()
{
  QString s = QFileDialog::getSaveFileName(
     lastDir.isEmpty() ? QString(".") : lastDir,
     tr("Qucs Packages")+" (*.qucs);;"+
     tr("Any File")+" (*)",
     this, 0, tr("Enter a Package File Name"));
  if(s.isEmpty()) return;

  QFileInfo Info(s);
  lastDir = Info.dirPath(true);  // remember last directory

  if(Info.extension().isEmpty())
    s += ".qucs";
  NameEdit->setText(s);
}

// ***************************************************************
// *****           Functions for creating package            *****
// ***************************************************************

int PackageDialog::insertFile(const QString& FileName, QFile& File,
                              QDataStream& Stream)
{
  QByteArray FileContent;

  if(!File.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("Error"),
                    tr("Cannot open \"%1\"!").arg(FileName));
    return -1;
  }

  Q_ULONG Count = File.size();
  char *p = (char*)malloc(Count+FileName.length()+2);
  strcpy(p, FileName.latin1());
  File.readBlock(p+FileName.length()+1, Count);
  File.close();

  Count += FileName.length()+1;
  FileContent = qCompress((unsigned char*)p, Count);
  free(p);

  Stream.writeBytes(FileContent.data(), FileContent.size());
  return 0;
}

// ---------------------------------------------------------------
int PackageDialog::insertDirectory(const QString& DirName,
                           QDataStream& Stream)
{
  QFile File;
  QDir myDir(DirName);

  // Put all files of this directory into the package.
  QStringList Entries = myDir.entryList("*", QDir::Files, QDir::Name);
  QStringList::iterator it;
  for(it = Entries.begin(); it != Entries.end(); ++it) {
    File.setName(myDir.absFilePath(*it));
    Stream << Q_UINT32(CODE_FILE);
    if(insertFile(*it, File, Stream) < 0)
      return -1;
  }

  // Put all subdirectories into the package.
  Entries = myDir.entryList("*", QDir::Dirs, QDir::Name);
  Entries.pop_front();  // delete "." from list
  Entries.pop_front();  // delete ".." from list
  for(it = Entries.begin(); it != Entries.end(); ++it) {
    Stream << Q_UINT32(CODE_DIR) << (*it).latin1();
    if(insertDirectory(myDir.absPath()+QDir::separator()+(*it), Stream) < 0)
      return -1;
    Stream << Q_UINT32(CODE_DIR_END) << Q_UINT32(0);
  }
  return 0;
}

// ---------------------------------------------------------------
int PackageDialog::insertLibraries(QDataStream& Stream)
{
  QFile File;
  QDir myDir(QucsSettings.QucsHomeDir.absPath() + QDir::separator() + "user_lib");
  QStringList Entries = myDir.entryList("*", QDir::Files, QDir::Name);
  QStringList::iterator it;
  for(it = Entries.begin(); it != Entries.end(); ++it) {
    File.setName(myDir.absFilePath(*it));
    Stream << Q_UINT32(CODE_LIBRARY);
    if(insertFile(*it, File, Stream) < 0)
      return -1;
  }
  return 0;
}

// ---------------------------------------------------------------
void PackageDialog::slotCreate()
{
  if(NameEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please insert a package name!"));
    return;
  }

  QCheckBox *p;
  QListIterator<QCheckBox *> i(BoxList);
  if(!LibraryCheck->isChecked()) {
    int count=0;
    while(i.hasNext()){
      p = i.next();
      if(p->isChecked())
        count++;
    }
    
    if(count < 1) {
      QMessageBox::critical(this, tr("Error"), tr("Please choose at least one project!"));
      return;
    }
  }

  QString s(NameEdit->text());
  QFileInfo Info(s);
  if(Info.extension().isEmpty())
    s += ".qucs";
  NameEdit->setText(s);

  QFile PkgFile(s);
  if(PkgFile.exists())
    if(QMessageBox::information(this, tr("Info"),
          tr("Output file already exists!")+"\n"+tr("Overwrite it?"),
          tr("&Yes"), tr("&No"), 0,1,1))
      return;

  if(!PkgFile.open(QIODevice::ReadWrite)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot create package!"));
    return;
  }
  QDataStream Stream(&PkgFile);

  // First write header.
  char Header[HEADER_LENGTH];
  memset(Header, 0, HEADER_LENGTH);
  strcpy(Header, "Qucs package " PACKAGE_VERSION);
  PkgFile.writeBlock(Header, HEADER_LENGTH);


  // Write project files to package.
  i.toFront();
  while(i.hasNext()) {
    p = i.next();  
    if(p->isChecked()) {
      s = p->text() + "_prj";
      Stream << Q_UINT32(CODE_DIR) << s.latin1();
      s = QucsSettings.QucsHomeDir.absPath() + QDir::separator() + s;
      if(insertDirectory(s, Stream) < 0) {
        PkgFile.close();
        PkgFile.remove();
        return;
      }
      Stream << Q_UINT32(CODE_DIR_END) << Q_UINT32(0);
    }
  }

  // Write user libraries to package if desired.
  if(LibraryCheck->isChecked())
    if(insertLibraries(Stream) < 0) {
      PkgFile.close();
      PkgFile.remove();
      return;
    }

  // Calculate checksum and write it to package file.
  PkgFile.at(0);
  QByteArray Content = PkgFile.readAll();
  Q_UINT16 Checksum = qChecksum(Content.data(), Content.size());
  PkgFile.at(HEADER_LENGTH-sizeof(Q_UINT16));
  Stream << Checksum;
  PkgFile.close();

  QMessageBox::information(this, tr("Info"),
          tr("Successfully created Qucs package!"));
  accept();
}



// ***************************************************************
// *****          Functions for extracting package           *****
// ***************************************************************

void PackageDialog::extractPackage()
{
  QString s = QFileDialog::getOpenFileName(
     lastDir.isEmpty() ? QString(".") : lastDir,
     tr("Qucs Packages")+" (*.qucs);;"+
     tr("Any File")+" (*)",
     this, 0, tr("Enter a Package File Name"));

  if(s.isEmpty()) {
    reject();
    return;
  }

  QFileInfo Info(s);
  lastDir = Info.dirPath(true);  // remember last directory

  QFile PkgFile(s);
  if(!PkgFile.open(QIODevice::ReadOnly)) {
    if(Info.extension().isEmpty()) s += ".qucs";
    PkgFile.setName(s);
    if(!PkgFile.open(QIODevice::ReadOnly)) {
      MsgText->append(tr("ERROR: Cannot open package!"));
      ButtClose->setDisabled(false);
      return;
    }
  }
  QDataStream Stream(&PkgFile);

  QDir currDir = QucsSettings.QucsHomeDir;
  QString Version;
  Q_UINT16 Checksum;
  Q_UINT32 Code, Length;

  // First read and check header.
  QByteArray Content = PkgFile.readAll();
  if(strncmp(Content.data(), "Qucs package ", 13) != 0) {
    MsgText->append(tr("ERROR: File contains wrong header!"));
    goto ErrorEnd;
  }

  Version = QString(Content.data()+13);
  if(!checkVersion(Version)) {
    MsgText->append(tr("ERROR: Wrong version number!"));
    goto ErrorEnd;
  }

  // checksum correct ?
  PkgFile.at(HEADER_LENGTH-2);
  Stream >> Checksum;
  *((Q_UINT16*)(Content.data()+HEADER_LENGTH-2)) = 0;
  if(Checksum != qChecksum(Content.data(), Content.size())) {
    MsgText->append(tr("ERROR: Checksum mismatch!"));
    goto ErrorEnd;
  }
  Content.resize(0);   // dispose memory


  // work on all files and directories in the package
  for(;;) {
    if(PkgFile.atEnd()) break;
    Stream >> Code >> Length;

    switch(Code) {
      case CODE_DIR:
        if(extractDirectory(PkgFile, Length, currDir) > 0)
          break;
        goto ErrorEnd;
      case CODE_DIR_END:
        MsgText->append(tr("Leave directory \"%1\"").arg(currDir.absPath()));
        currDir.cdUp();
        break;
      case CODE_FILE:
        if(extractFile(PkgFile, Length, currDir) > 0)
          break;
        goto ErrorEnd;
      case CODE_LIBRARY:
        if(extractLibrary(PkgFile, Length) > 0)
          break;
        goto ErrorEnd;
      default:
        MsgText->append(tr("ERROR: Package is corrupt!"));
        goto ErrorEnd;
    }
  }


  MsgText->append(" ");
  MsgText->append(tr("Successfully extracted package!"));
ErrorEnd:
  MsgText->append(" ");
  ButtClose->setDisabled(false);
  PkgFile.close();
}

// ---------------------------------------------------------------
int PackageDialog::extractDirectory(QFile& PkgFile, Q_UINT32 Count, QDir& currDir)
{
  char *p = (char*)malloc(Count);
  PkgFile.readBlock(p, Count);

  if(currDir.cd(QString(p))) { // directory exists ?
    MsgText->append(tr("ERROR: Project directory \"%1\" already exists!").arg(QString(p)));
    return -1;
  }

  if(!currDir.mkdir(QString(p))) {
    MsgText->append(tr("ERROR: Cannot create directory \"%1\"!").arg(QString(p)));
    return -2;
  }
  currDir.cd(QString(p));
  MsgText->append(tr("Create and enter directory \"%1\"").arg(currDir.absPath()));

  free(p);
  return 1;
}

// ---------------------------------------------------------------
int PackageDialog::extractFile(QFile& PkgFile, Q_UINT32 Count, QDir& currDir)
{
  char *p = (char*)malloc(Count);
  PkgFile.readBlock(p, Count);
  QByteArray Content = qUncompress((unsigned char*)p, Count);
  free(p);

  p = Content.data();
  QFile File(currDir.absFilePath(QString(p)));
  if(!File.open(QIODevice::WriteOnly)) {
    MsgText->append(tr("ERROR: Cannot create file \"%1\"!").arg(QString(p)));
    return -1;
  }

  File.writeBlock(p+strlen(p)+1, Content.size()-strlen(p)-1);
  File.close();
  MsgText->append(tr("Create file \"%1\"").arg(QString(p)));
  return 1;
}

// ---------------------------------------------------------------
int PackageDialog::extractLibrary(QFile& PkgFile, Q_UINT32 Count)
{
  char *p = (char*)malloc(Count);
  PkgFile.readBlock(p, Count);
  QByteArray Content = qUncompress((unsigned char*)p, Count);
  free(p);

  p = Content.data();
  QFile File(QucsSettings.QucsHomeDir.absPath() +
             QDir::convertSeparators("/user_lib/") + QString(p));
  if(File.exists()) {
    MsgText->append(tr("ERROR: User library \"%1\" already exists!").arg(QString(p)));
    return -1;
  }

  if(!File.open(QIODevice::WriteOnly)) {
    MsgText->append(tr("ERROR: Cannot create library \"%1\"!").arg(QString(p)));
    return -1;
  }

  File.writeBlock(p+strlen(p)+1, Content.size()-strlen(p)-1);
  File.close();
  MsgText->append(tr("Create library \"%1\"").arg(QString(p)));
  return 1;
}
