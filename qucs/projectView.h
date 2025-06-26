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

#include <QStandardItem>
#include <QString>
#include <QTreeView>

class QStandardItemModel;

class ProjectView : public QTreeView {
    Q_OBJECT
public:
    ProjectView(QWidget* parent);
    virtual ~ProjectView();

    QStandardItemModel* model() { return m_model; };

    // data related
    void setProjPath(const QString&);
    void refresh();
    QStringList exportSchematic();

signals:
    void filesSelected(const QStringList&);

private:
    QStandardItemModel* m_model;

    bool m_valid;
    QString m_projPath;
    QString m_projName;

    inline void appendChild(int category, const QList<QStandardItem*>& data)
    {
        if (auto* item = m_model->item(category, 0)) {
            item->appendRow(data);
        }
    }

    inline void appendRow(QStandardItem* parent, const QString& data0, const QString& data1)
    {
        auto* col0 = new QStandardItem(data0);
        auto* col1 = new QStandardItem(data1);

        col0->setFlags(col0->flags() & ~Qt::ItemIsSelectable);
        col1->setFlags(col1->flags() & ~Qt::ItemIsSelectable);

        QList<QStandardItem*> row { col0, col1 };
        parent->appendRow(row);
    }
};

#endif /* PROJECTVIEW_H_ */
