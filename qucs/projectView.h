/*
 * projectView.h - declaration of project view
 *   and the model that manage files in project
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

#ifndef PROJECTVIEW_H_
#define PROJECTVIEW_H_ value

#include <QTreeView>
#include <QString>

#define FILETYPE1 tr("Datasets")
#define FILETYPE2 tr("Data Displays")
#define FILETYPE3 tr("Verilog")
#define FILETYPE4 tr("Verilog-A")
#define FILETYPE5 tr("VHDL")
#define FILETYPE6 tr("Octave")
#define FILETYPE7 tr("Schematics")
#define FILETYPE8 tr("Others")

#define APPEND_ROW(parent, data) \
({ \
  QList<QStandardItem*> c; \
  c.append(new QStandardItem(data)); \
  parent->appendRow(c); \
})

class QStandardItemModel;

class ProjectView : public QTreeView
{
  Q_OBJECT
public:
  ProjectView (QWidget *parent);
  virtual ~ProjectView ();

  QStandardItemModel *model() { return m_model; };

  //data related
  void setProjPath(const QString &);
  void refresh();
  QStringList exportSchematic();
private:
  QStandardItemModel *m_model;

  bool m_valid;
  QString m_projPath;
  QString m_projName;
};

#endif /* PROJECTVIEW_H_ */
