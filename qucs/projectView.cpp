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
#include <QStringList>
#include <QDir>
#include <QStandardItemModel>
#include <QDebug>

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
  // check if path exist
  m_valid = !path.isEmpty() && QDir(path).exists();

  if (m_valid) {
    m_projPath = path; // full path
    m_projName = QDir(m_projPath).dirName(); // only project directory name
    if (m_projName.endsWith("_prj")) {
      m_projName.chop(4);// remove "_prj" from name
    } else { // should not happen
      qWarning() << "ProjectView::setProjPath() : path does not end in '_prj' (" << m_projName << ")";
    }
  }
  refresh();
}

// refresh using projectPath
void
ProjectView::refresh()
{
  m_model->clear();

  QStringList header;
  header << tr("Content of %1").arg(m_projName) << tr("Note");
  m_model->setHorizontalHeaderLabels(header);

  APPEND_ROW(m_model, tr("Datasets")     );
  APPEND_ROW(m_model, tr("Data Displays"));
  APPEND_ROW(m_model, tr("Verilog")      );
  APPEND_ROW(m_model, tr("Verilog-A")    );
  APPEND_ROW(m_model, tr("VHDL")         );
  APPEND_ROW(m_model, tr("Octave")       );
  APPEND_ROW(m_model, tr("Schematics")   );
  APPEND_ROW(m_model, tr("XSPICE")       );
  APPEND_ROW(m_model, tr("Others")       );

  setExpanded(m_model->index(6, 0), true);

  if (!m_valid) {
    return;
  }

  // put all files into "Content"-ListView
  QDir workPath(m_projPath);
  QStringList files = workPath.entryList(QStringList() << "*", QDir::Files, QDir::Name);
  QStringList::iterator it;
  QString extName, fileName;
  QList<QStandardItem *> columnData;

#define APPEND_CHILD(category, data) \
  m_model->item(category, 0)->appendRow(data);

  for(it = files.begin(); it != files.end(); ++it) {
    fileName = (*it).toAscii();
    extName = QFileInfo(workPath.filePath(fileName)).suffix();

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
    else if ((extName == "mod")||(extName=="ifs")) {
        APPEND_CHILD(7,columnData);
    }
    else {
      APPEND_CHILD(8, columnData);
    }
  }

  resizeColumnToContents(0);
}

QStringList
ProjectView::exportSchematic()
{
  QStringList list;
  QStandardItem *item = m_model->item(6, 0);
  for (int i = 0; i < item->rowCount(); ++i) {
    if (item->child(i,1)) {
      list.append(item->child(i,0)->text());
    }
  }
  return list;
}
