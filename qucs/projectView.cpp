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
#include <QDir>
#include <QStandardItemModel>

ProjectView::ProjectView(QWidget *parent)
  : QTreeView(parent)
{
  m_projPath = QString();
  m_projPath = QString();
  m_valid = false;
  m_model = new QStandardItemModel(8, 2, this);

  refresh();

  this->setModel(m_model);
  this->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
  m_model->clear();

  QStringList header;
  header << tr("Content of %1").arg(m_projName) << tr("Note");
  m_model->setHorizontalHeaderLabels(header);

  APPEND_ROW(m_model, FILETYPE1);
  APPEND_ROW(m_model, FILETYPE2);
  APPEND_ROW(m_model, FILETYPE3);
  APPEND_ROW(m_model, FILETYPE4);
  APPEND_ROW(m_model, FILETYPE5);
  APPEND_ROW(m_model, FILETYPE6);
  APPEND_ROW(m_model, FILETYPE7);
  APPEND_ROW(m_model, FILETYPE8);

  setExpanded(m_model->index(6, 0), true);

  if (!m_valid) {
    return;
  }

  // put all files into "Content"-ListView
  QDir workPath(m_projPath);
  QStringList files = workPath.entryList("*", QDir::Files, QDir::Name);
  QStringList::iterator it;
  QString extName, fileName;
  QList<QStandardItem *> columnData;

#define APPEND_CHILD(category, data) \
  m_model->item(category, 0)->appendRow(data);

  for(it = files.begin(); it != files.end(); ++it) {
    fileName = (*it).toAscii();
    extName = QFileInfo(workPath.filePath(fileName)).completeSuffix();

    columnData.clear();
    columnData.append(new QStandardItem(fileName));

    if(extName == "dat") {
      APPEND_CHILD(0, columnData);
    }
    else if(extName == "dpl") {
      APPEND_CHILD(1, columnData);
    }
    else if(extName == "v") {
      APPEND_CHILD(2, columnData);
    }
    else if(extName == "va") {
      APPEND_CHILD(3, columnData);
    }
    else if((extName == "vhdl") || (extName == "vhd")) {
      APPEND_CHILD(4, columnData);
    }
    else if((extName == "m") || (extName == "oct")) {
      APPEND_CHILD(5, columnData);
    }
    else if(extName == "sch") {
      int n = Schematic::testFile(workPath.filePath(fileName));
      if(n >= 0) {
        if(n > 0) {
          columnData.append(new QStandardItem(QString::number(n)+tr("-port")));
        }
      }
      APPEND_CHILD(6, columnData);
    }
    else {
      APPEND_CHILD(7, columnData);
    }
  }

  resizeColumnToContents(0);
}
