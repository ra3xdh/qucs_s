/***************************************************************************
                               changedialog.cpp
                              ------------------
    begin                : Fri Jul 22 2005
    copyright            : (C) 2005 by Michael Margraf
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
#include "changedialog.h"
#include "node.h"
#include "schematic.h"
#include "components/component.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QValidator>
#include <QPushButton>
#include <QScrollArea>
#include <QCheckBox>
#include <QMessageBox>
#include <QGridLayout>
#include <QList>
#include <QListIterator>
#include <QVBoxLayout>
#include <QDebug>


ChangeDialog::ChangeDialog(Schematic *Doc_)
			: QDialog(Doc_) //, 0, TRUE, Qt::WDestructiveClose)
{
  Doc = Doc_;
  setWindowTitle(tr("Change Component Properties"));

  Expr.setPattern("[^\"=]+");  // valid expression for property value
  Validator = new QRegExpValidator(Expr, this);
  Expr.setPattern("[\\w_]+");  // valid expression for property name
  ValRestrict = new QRegExpValidator(Expr, this);


  // ...........................................................
  all = new QGridLayout(this);//, 6,2,3,3);
  all->setMargin(5);

  all->addWidget(new QLabel(tr("Components:"), this), 0,0);
  CompTypeEdit = new QComboBox(this);
  CompTypeEdit->insertItem(tr("all components"));
  CompTypeEdit->insertItem(tr("resistors"));
  CompTypeEdit->insertItem(tr("capacitors"));
  CompTypeEdit->insertItem(tr("inductors"));
  CompTypeEdit->insertItem(tr("transistors"));
  all->addWidget(CompTypeEdit, 0,1);

  all->addWidget(new QLabel(tr("Component Names:"), this), 1,0);
  CompNameEdit = new QLineEdit(this);
  CompNameEdit->setValidator(Validator);
  CompNameEdit->setText("*");
  all->addWidget(CompNameEdit, 1,1);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtReplace()));

  all->addWidget(new QLabel(tr("Property Name:"), this), 2,0);
  PropNameEdit = new QComboBox(this);
  PropNameEdit->setEditable(true);
  PropNameEdit->setValidator(ValRestrict);
  PropNameEdit->insertItem("Temp");
  PropNameEdit->insertItem("Subst");
  PropNameEdit->insertItem("Model");
  all->addWidget(PropNameEdit, 2,1);
  connect(PropNameEdit, SIGNAL(activated(int)), SLOT(slotButtReplace()));

  all->addWidget(new QLabel(tr("New Value:"), this), 3,0);
  NewValueEdit = new QLineEdit(this);
  NewValueEdit->setValidator(Validator);
  NewValueEdit->setText("-273.15");
  all->addWidget(NewValueEdit, 3,1);
  connect(NewValueEdit, SIGNAL(returnPressed()), SLOT(slotButtReplace()));

  // ...........................................................
  QPushButton *pushReplace = new QPushButton(tr("Replace"));
  QPushButton *pushCancel = new QPushButton(tr("Cancel"));
  all->addWidget(pushReplace, 4,0);
  all->addWidget(pushCancel, 4,1);
  connect(pushReplace, SIGNAL(clicked()), SLOT(slotButtReplace()));
  connect(pushCancel, SIGNAL(clicked()), SLOT(reject()));
}

ChangeDialog::~ChangeDialog()
{
  delete all;
  delete Validator;
  delete ValRestrict;
}

// -----------------------------------------------------------------------
// Returns "true" if the component model matches the user selection
// in "CompTypeEdit".
bool ChangeDialog::matches(const QString& CompModel)
{
  switch(CompTypeEdit->currentItem()) {
    case 0: return true;
    case 1: if(CompModel == "R") return true;
            return false;
    case 2: if(CompModel == "C") return true;
            return false;
    case 3: if(CompModel == "L") return true;
            return false;
    case 4: if(CompModel == "BJT") return true;
            if(CompModel == "_BJT") return true;
            if(CompModel == "JFET") return true;
            if(CompModel == "MOSFET") return true;
            if(CompModel == "_MOSFET") return true;
            return false;
  }

  return false;
}

// -----------------------------------------------------------------------
// Is called if the "Replace"-button is pressed.
void ChangeDialog::slotButtReplace()
{
  Expr.setWildcard(true);  // switch into wildcard mode
  Expr.setPattern(CompNameEdit->text());
  if(!Expr.isValid()) {
    QMessageBox::critical(this, tr("Error"),
	  tr("Regular expression for component name is invalid."));
    return;
  }

  // create dialog showing all found components
  QDialog *Dia = new QDialog(this);
  Dia->setCaption(tr("Found Components"));
  QVBoxLayout *Dia_All = new QVBoxLayout(Dia);
  Dia_All->setSpacing(3);
  Dia_All->setMargin(5);
  
  QScrollArea *Dia_Scroll = new QScrollArea(Dia);
  //Dia_Scroll->setMargin(5);
  Dia_All->addWidget(Dia_Scroll);
  
  QVBoxLayout *Dia_Box = new QVBoxLayout(Dia_Scroll->viewport());
  Dia_Scroll->insertChild(Dia_Box);
  QLabel *Dia_Label = new QLabel(tr("Change properties of\n")
                               + tr("these components ?"), Dia);
  Dia_All->addWidget(Dia_Label);
  
  QHBoxLayout *Dia_h = new QHBoxLayout(Dia);
  Dia_h->setSpacing(5);
  QPushButton *YesButton = new QPushButton(tr("Yes"));
  QPushButton *CancelButton = new QPushButton(tr("Cancel"));
  Dia_h->addWidget(YesButton);
  Dia_h->addWidget(CancelButton);
  connect(YesButton, SIGNAL(clicked()), Dia, SLOT(accept()));
  connect(CancelButton, SIGNAL(clicked()), Dia, SLOT(reject()));
  
  Dia_All->addLayout(Dia_h);

  QList<QCheckBox *> pList;
  QCheckBox *pb;
  Component *pc;
  QStringList List;
  QString str;
  int i1, i2;
  // search through all components
  for(pc = Doc->Components->first(); pc!=0; pc = Doc->Components->next()) {
    if(matches(pc->Model)) {
      if(Expr.search(pc->Name) >= 0)
        for(Property *pp = pc->Props.first(); pp!=0; pp = pc->Props.next())
          if(pp->Name == PropNameEdit->currentText()) {
            pb = new QCheckBox(pc->Name);
            Dia_Box->addWidget(pb);   
            pList.append(pb);
            pb->setChecked(true);
            i1 = pp->Description.find('[');
            if(i1 < 0)  break;  // no multiple-choice property

            i2 = pp->Description.findRev(']');
            if(i2-i1 < 2)  break;
            str = pp->Description.mid(i1+1, i2-i1-1);
            str.replace( QRegExp("[^a-zA-Z0-9_,]"), "" );
            List = List.split(',',str);
            if(List.findIndex(NewValueEdit->text()) >= 0)
              break;    // property value is okay

            pb->setChecked(false);
            pb->setEnabled(false);
            break;
          }
    }
  }
/*
  QColor theColor;
  if(pList.isEmpty()) {
    YesButton->setEnabled(false);
    theColor =
       (new QLabel(tr("No match found!"), Dia_Box))->paletteBackgroundColor();
  }
  else  theColor = pList.current()->paletteBackgroundColor();
*/
  //Dia_Scroll->viewport()->setPaletteBackgroundColor(theColor);
  Dia->resize(50, 300);


  // show user all components found
  int Result = Dia->exec();
  if(Result != QDialog::Accepted) return;


  bool changed = false;
  // change property values
  
  QListIterator<QCheckBox *> i(pList);
  while(i.hasNext()){
    pb = i.next();
    if(!pb->isChecked())  continue;

    for(pc = Doc->Components->first(); pc!=0; pc = Doc->Components->next()) {
      if(pb->text() != pc->Name)  continue;

      for(Property *pp = pc->Props.first(); pp!=0; pp = pc->Props.next()) {
        if(pp->Name != PropNameEdit->currentText())  continue;

        int tx_Dist, ty_Dist, tmp;
        pc->textSize(tx_Dist, ty_Dist);
        tmp = pc->tx+tx_Dist - pc->x1;
        if((tmp > 0) || (tmp < -6))  tx_Dist = 0; // remember text position
        tmp = pc->ty+ty_Dist - pc->y1;
        if((tmp > 0) || (tmp < -6))  ty_Dist = 0;

        pp->Value = NewValueEdit->text();

        int dx, dy;
        pc->textSize(dx, dy);   // correct text position
        if(tx_Dist != 0) {
          pc->tx += tx_Dist-dx;
          tx_Dist = dx;
        }
        if(ty_Dist != 0) {
          pc->ty += ty_Dist-dy;
          ty_Dist = dy;
        }

        // apply changes to schematic symbol
        Doc->recreateComponent(pc);
        changed = true;
        break;
      }
      break;
    }
  }

  delete Dia_All;
  delete Dia;
  if(changed) accept();
  else reject();
}
