#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QMessageBox>

#include "main.h"

#include "component.h"
#include "qucslib_common.h"

#include "selfromlibdialog.h"

SelFromLibDialog::SelFromLibDialog(Component *c)
{
  comp = c;

  QLabel *lblLib = new QLabel(tr("Library"));
  QLabel *lblComp =  new QLabel(tr("Component"));
  QLabel *lblDescr = new QLabel(tr("Description"));
  cbxLib = new QComboBox;
  lstComps = new QListWidget;
  edtDescr = new QPlainTextEdit;
  edtDescr->setReadOnly(true);

  btnOK = new QPushButton(tr("OK"));
  connect(btnOK,SIGNAL(clicked(bool)),this,SLOT(slotOK()));
  btnApply = new QPushButton(tr("Apply"));
  connect(btnApply,SIGNAL(clicked(bool)),this,SLOT(slotApply()));
  btnCancel = new QPushButton(tr("Cancel"));
  connect(btnCancel,SIGNAL(clicked(bool)),this,SLOT(reject()));

  QVBoxLayout *top = new QVBoxLayout;
  top->addWidget(lblLib);
  top->addWidget(cbxLib);
  top->addWidget(lblComp);
  top->addWidget(lstComps);
  top->addWidget(lblDescr);
  top->addWidget(edtDescr);

  QHBoxLayout *hl1 = new QHBoxLayout;
  hl1->addWidget(btnOK);
  hl1->addWidget(btnApply);
  hl1->addWidget(btnCancel);
  top->addLayout(hl1);

  this->setLayout(top);

  fillLibComboBox();
  slotShowComponents();
  slotShowDescription();

  connect(cbxLib,SIGNAL(currentIndexChanged(int)),this,SLOT(slotShowComponents()));
  connect(lstComps,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(slotShowDescription()));

  selectLibraryAndComp();
}

void SelFromLibDialog::selectLibraryAndComp()
{
  if (comp != nullptr) {
    Property *pp = comp->getProperty("LibName");
    if (pp != nullptr) {
      if (pp->Value != "Generic") {
        cbxLib->setCurrentText(pp->Value);
      }
    }
    pp = comp->getProperty("CompName");
    if (pp != nullptr) {
      if (pp->Value != "Generic") {
        QList<QListWidgetItem*> items = lstComps->findItems(pp->Value, Qt::MatchExactly);
        if (!items.isEmpty()) {
          lstComps->setCurrentItem(items.first());
        } else {
          lstComps->setCurrentRow(0);
        }
      } else {
        lstComps->setCurrentRow(0);
      }
    }
  }
}

void SelFromLibDialog::slotApply()
{
  if (lstComps->currentItem() == nullptr) {
    QMessageBox::information(this,tr("Select model"),tr("No model selected from list"));
  } else {
    fillCompProperties();
  }
}

void SelFromLibDialog::slotOK()
{
  slotApply();
  accept();
}

void SelFromLibDialog::fillCompProperties()
{
  QStringList lst_props = LibModel.split(" ");
  int idx  = 9;
  for (int i = 0; i < comp->Props.count(); i++) {
    QString val = lst_props.at(idx);
    val.remove(0,1); val.chop(1);
    bool display = (lst_props.at(idx+1) == "1");
    idx +=2;
    if (idx >= lst_props.count()) break;
    comp->Props[i]->Value = val;
    comp->Props[i]->display = display;
  }

  Property *pp = comp->getProperty("LibName");
  if (pp != nullptr) {
    pp->Value = cbxLib->currentText();
    pp->display = true;
  }
  pp = comp->getProperty("CompName");
  if (pp != nullptr) {
    pp->Value = lstComps->currentItem()->text();
    pp->display = true;
  }
}

void SelFromLibDialog::fillLibComboBox()
{
  checkAndParseLibrary(QucsSettings.LibDir,true);

  for(const auto &lib: parsedLibs) {
    cbxLib->addItem(lib->name);
  }
}

void SelFromLibDialog::slotShowComponents()
{
  QString libname = cbxLib->currentText();
  for(const auto &lib: parsedLibs) {
    if (lib->name == libname) {
      lstComps->clear();
      for (const auto &c: lib->components) {
        lstComps->addItem(c.name);
      }
      break;
    }
  }
}

void SelFromLibDialog::slotShowDescription()
{
  QString libname = cbxLib->currentText();
  auto itm = lstComps->currentItem();
  if (itm == nullptr) return;
  edtDescr->clear();
  QString compname = itm->text();
  for(const auto &lib: parsedLibs) {
    if (lib->name == libname) {
      for (const auto &c: lib->components) {
        if (compname == c.name) {
          QString desc;
          QString def = c.definition;
          getSection("Description", def, desc);
          edtDescr->setPlainText(desc);
          LibModel = c.modelString;
          LibModel.remove(0,1);
          LibModel.chop(1);
        }
      }
      break;
    }
  }
}

void SelFromLibDialog::checkAndParseLibrary(const QString &libdir, bool relpath)
{
  QDir LibDir(libdir);
  QStringList LibFiles = LibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

  // create top level library items, base on the library names
  for(auto it = LibFiles.begin(); it != LibFiles.end(); it++)
  {
    QString libPath(LibDir.absoluteFilePath(*it));
    libPath.chop(4); // remove extension

    ComponentLibrary *parsedlibrary = new ComponentLibrary;

    int result = parseComponentLibrary (libPath , *parsedlibrary, QUCS_COMP_LIB_FULL, relpath);

    if (result != QUCS_COMP_LIB_OK) return;

    bool found = false;
    for(const auto &c: parsedlibrary->components) {
      QString m = c.modelString;
      m.remove(0,1);
      m.chop(1);
      m = m.section(" ",0,0);
      if (m == comp->Model) {
        found = true;
        parsedLibs.append(parsedlibrary);
        break;
      }
    }

    if (!found) {
      delete parsedlibrary;
    }
  }
}
