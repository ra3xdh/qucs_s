#include <QPlainTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include "misc.h"
#include "component.h"

#include "fillfromspicedialog.h"

fillFromSpiceDialog::fillFromSpiceDialog(Component *pc, QWidget *w)
    : QDialog(w)
{
  Comp = pc;

  edtModel = new QPlainTextEdit;
  QLabel *lblModel = new QLabel(tr("Insert .MODEL text here"));
  btnOK = new QPushButton(tr("OK"));
  connect(btnOK,SIGNAL(clicked(bool)),this,SLOT(slotOK()));
  btnCancel = new QPushButton(tr("Cancel"));
  connect(btnCancel,SIGNAL(clicked(bool)),this,SLOT(reject()));
  chbNumNotation = new QCheckBox(tr("Convert number notation"));

  QVBoxLayout *top = new QVBoxLayout;
  top->addWidget(lblModel);
  top->addWidget(edtModel,4);
  top->addWidget(chbNumNotation);
  QHBoxLayout *l1 = new QHBoxLayout;
  l1->addWidget(btnOK);
  l1->addWidget(btnCancel);
  l1->addStretch();
  top->addLayout(l1);
  this->setLayout(top);
  this->setMinimumWidth(400);

}

int fillFromSpiceDialog::parseModelcard()
{
  QString ModelCard = edtModel->toPlainText();
  QStringList lstModelCard = ModelCard.split("\n");
  ModelCard.clear();
  for (auto &line : lstModelCard) { // assemble model in one line
    if (line.startsWith("*")) continue; // comment
    if (line.trimmed().isEmpty()) continue;
    if (line.startsWith("+")) line.remove(0,1); // line continuation
    ModelCard += line;
  }
  ModelCard = ModelCard.trimmed();
  ModelCard = ModelCard.toLower();

  if (!ModelCard.startsWith(".model")) {
    return noModel;
  }

  // tokenize modelcard
  QStringList tokens;
  QString tok;
  for (const auto &c: ModelCard) {
    if (c.isLetterOrNumber() || c == '_' ||
        c == '.' || c == '+' || c == '-') {
      tok += c;
      continue;
    }
    if (c == '(' || c == ')' || c == '=') {
      if (!tok.isEmpty())
        tokens.append(tok);
      tok = c;
      tokens.append(tok);
      tok.clear();
      continue;
    }
    if (c.isSpace()) {
      if (!tok.isEmpty())
        tokens.append(tok);
      tok.clear();
      continue;
    }
  }

  if (tokens.count() <= 3) {
    return wrongModel;
  }

  ModelName = tokens.at(1);
  ModelType = tokens.at(2);

  int cnt = tokens.count();
  // fill propperties list
  for (int i = 0; i < tokens.count(); i++) {
    if (tokens.at(i) == "=") {
      int idx_name = i - 1;
      int idx_val = i + 1;
      if (idx_name > cnt-1 || idx_name < 0
          || idx_val > cnt-1) {
        return wrongModel;
      }
      QString name = tokens.at(idx_name);
      QString value = tokens.at(idx_val);

      if (ModelType == "d") {  // Convert propperty names
        if (name == "cjo") name = "cj0";
      }
      if (ModelType == "nmos" || ModelType == "pmos") {
        if (name == "vto") name == "vt0";
      }
      if (ModelType == "njf" || ModelType == "pjf") {
        if (name == "vto") name == "vt0";
      }
      if (value.endsWith("meg")) {
        value.chop(3);
        value += "M";
      }

      if (chbNumNotation->isChecked()) {
        QString vv = convertNumNotation(value);
        if (!vv.isEmpty()) value = vv;
      }

      parsedProps[name] = value;
    }
  }

  return noError;
}

QString fillFromSpiceDialog::convertNumNotation(const QString &value)
{
  QString v;
  bool ok = false;
  double num = value.toDouble(&ok);
  if (ok) {
    v = misc::num2str(num);
  }
  return v;
}

void fillFromSpiceDialog::fillCompProps()
{
  for(Property *p = Comp->Props.first(); p != 0; p = Comp->Props.next()) {
    QString name = p->Name;
    name = name.toLower();
    if (parsedProps.contains(name)) {
      p->Value = parsedProps[name];
    }
  }
}

void fillFromSpiceDialog::showErrorMsg(int code)
{
  QString msg;
  switch (code) {
  case noModel:
    msg = tr("No .MODEL directive found");
    break;
  case wrongModel:
    msg = tr("SPICE model parse error");
  default:
    break;
  }
  QMessageBox::critical(this,tr("Error"),msg);
}

void fillFromSpiceDialog::slotOK()
{
  auto r = parseModelcard();
  if ( r == noError ) {
    fillCompProps();
    accept();
  } else {
    showErrorMsg(r);
    reject();
  }
}
