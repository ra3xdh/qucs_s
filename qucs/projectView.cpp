/*
 * ProjectView.cpp - implementation of project model
 *   the model manage the files in project directory
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
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

#include "projectView.h"
#include "schematic.h"

#include <QString>
#include <QList>
#include <QDir>
#include <QStandardItem>
#include <QStandardItemModel>

ProjectView::ProjectView(QWidget *parent)
  : QTreeView(parent)
{
  m_projPath = QString();
  m_projPath = QString();
  m_valid = false;
  m_model = new QStandardItemModel(8, 2);

  refresh();

  this->setModel(m_model);
}

ProjectView::~ProjectView()
{
  delete m_model;
}

void
ProjectView::setProjPath(const QString &path)
{
  m_valid = QDir(path).exists();

  if (m_valid) {
    //test path exist
    m_projPath = path;
    m_projName = path;

    // cut off trailing '/'
    if (m_projName.endsWith(QDir::separator())) {
      m_projName.chop(1);
    }
    int i = m_projName.lastIndexOf(QDir::separator());
    if(i > 0) {
      m_projName = m_projName.mid(i+1);   // cut out the last subdirectory
    }
    m_projName.remove("_prj");

    refresh();
  }
}

// refresh using projectPath
void
ProjectView::refresh()
{
  QStringList header;
  header << tr("Content of %1").arg(m_projName) << tr("Note");
  m_model->setHorizontalHeaderLabels(header);

  m_model->clear();

  APPEND_ROW(m_model, FILETYPE1);
  APPEND_ROW(m_model, FILETYPE2);
  APPEND_ROW(m_model, FILETYPE3);
  APPEND_ROW(m_model, FILETYPE4);
  APPEND_ROW(m_model, FILETYPE5);
  APPEND_ROW(m_model, FILETYPE6);
  APPEND_ROW(m_model, FILETYPE7);
  APPEND_ROW(m_model, FILETYPE8);

  if (!m_valid) {
    return;
  }
  
//  QList<QVariant> data;
//
//  data << tr("Content of %1").arg(m_projName) << tr("Note");
//  rootItem = new ProjectItem(data);
//#define appendCategory(content) \
//  ({ data.clear(); \
//     data << content << ""; \
//     rootItem->appendChild(new ProjectItem(data, rootItem)); \
//  })
//  appendCategory(FILETYPE1);
//  appendCategory(FILETYPE2);
//  appendCategory(FILETYPE3);
//  appendCategory(FILETYPE4);
//  appendCategory(FILETYPE5);
//  appendCategory(FILETYPE6);
//  appendCategory(FILETYPE7);
//  appendCategory(FILETYPE8);
//#undef appendCategory
//
//  int n;
//  // put all files into "Content"-ListView
//  QDir workPath(m_projPath);
//  QStringList files = workPath.entryList("*", QDir::Files, QDir::Name);
//  QStringList::iterator it;
//  QString extName, fileName;
//
//#define appendFile(category) \
//  ({ rootItem->child(category)->appendChild(new ProjectItem(columnData, rootItem->child(category))); })
//
//  for(it = files.begin(); it != files.end(); ++it) {
//    fileName = (*it).toAscii();
//    extName = QFileInfo(workPath.filePath(fileName)).completeSuffix();
//    QList<QVariant> columnData;
//    columnData << fileName;
//
//    if(extName == "dat") {
//      appendFile(0);
//    }
//    else if(extName == "dpl") {
//      appendFile(1);
//    }
//    else if(extName == "v") {
//      appendFile(2);
//    }
//    else if(extName == "va") {
//      appendFile(3);
//    }
//    else if((extName == "vhdl") || (extName == "vhd")) {
//      appendFile(4);
//    }
//    else if((extName == "m") || (extName == "oct")) {
//      appendFile(5);
//    }
//    else if(extName == "sch") {
//      n = Schematic::testFile(workPath.filePath(fileName));
//      if(n >= 0) {
//        if(n > 0) {
//          columnData << QString::number(n)+tr("-port");
//        }
//      }
//      appendFile(6);
//    }
//    else {
//      appendFile(7);
//    }
//  }
//
//#undef appendFile
//  reset();
}
