#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <QBuffer>
#include <Q3TextStream>
#include <QObject>
#include <QApplication>
#include <Q3TextEdit>
#include <QDir>
#include <QTextCodec>
#include <QClipboard>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_filter.h"
#include "qf_capacity.h"
#include "qf_tform.h"
#include "qf_box.h"
#include "qf_settings.h"
//Added by qt3to4:
#include <QTranslator>

void compute_lumped (qf_spec* spec_p, Q3TextStream& out) {

  qf_tform* T = qf_tform_apis [spec_p -> tform] -> cons (spec_p);

  T -> dump (out);
}

int main (int argc, char * argv []) {

  // apply default settings
  QucsSettings.x = 200;
  QucsSettings.y = 100;
  QucsSettings.font = QFont("Helvetica", 12);

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  QDir QucsDir;
  if (var != NULL) {
    QucsDir = QDir (var);
    QString QucsDirStr = QucsDir.canonicalPath ();
    QucsSettings.LangDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/lang/");
  } else {
    QString QucsApplicationPath = QCoreApplication::applicationDirPath();
#ifdef __APPLE__
    QucsDir = QDir(QucsApplicationPath.section("/bin",0,0));
#else
    QucsDir = QDir(QucsApplicationPath);
    QucsDir.cdUp();
#endif
    QucsSettings.LangDir = QucsDir.canonicalPath() + "/share/qucs/lang/";
  }

  loadSettings();

  QApplication	    app (argc, argv);
  qf_spec*	    spec_p;

  int               result = 0;
  
  app.setFont( QucsSettings.font );
  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QTextCodec::locale();
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  app.installTranslator( &tor );

  qf_box Filterbox;
  Filterbox.setFont (QucsSettings.font);
  Filterbox.move (QucsSettings.x, QucsSettings.y);
  Filterbox.show ();

  Filterbox.FilterName->setCurrentItem (QucsSettings.type);
  Filterbox.on_FilterName_activated (QucsSettings.form);
  Filterbox.TformName->setCurrentItem (QucsSettings.form);
  Filterbox.on_TformName_activated (QucsSettings.type);

  Filterbox.EnterCutoff->setText (QString::number (QucsSettings.cutoff));
  Filterbox.CutoffCombo->setCurrentItem (QucsSettings.cutoff_unit);
  Filterbox.EnterZin->setText (QString::number (QucsSettings.zin));
  Filterbox.EnterZout->setText (QString::number (QucsSettings.zout));
  Filterbox.OrderBox->setChecked (QucsSettings.specify);
  Filterbox.OrderCombo->setCurrentItem (QucsSettings.ord);
  Filterbox.SubOrderCombo->setCurrentItem (QucsSettings.subord);
  Filterbox.EnterBandwidth->setText (QString::number (QucsSettings.bw));
  Filterbox.BandwidthCombo->setCurrentItem (QucsSettings.bw_unit);
  Filterbox.EnterStopband->setText (QString::number (QucsSettings.sb));
  Filterbox.StopbandCombo->setCurrentItem (QucsSettings.sb_unit);
  Filterbox.EnterRipple->setText (QString::number (QucsSettings.ripple));
  Filterbox.EnterAngle->setText (QString::number (QucsSettings.angle));
  Filterbox.EnterAttenuation->setText (QString::number (QucsSettings.atten));

  while (true) {
    if (Filterbox. exec () != QDialog::Accepted) break;
    
    spec_p = Filterbox. get_spec ();

    QByteArray	buf;
    Q3TextStream s (buf, QIODevice::ReadWrite);

    compute_lumped (spec_p, s);
// FIXME #warning s. device () -> flush ();

    QClipboard *cb = QApplication::clipboard();
    cb->setText(buf);

  }

  saveSettings (&Filterbox);
  saveXmlSettings (&Filterbox);
  return result;
}
