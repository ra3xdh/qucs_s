/***************************************************************************
    copyright            : (C) 2010 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

#ifndef OCTAVE_WINDOW_H
#define OCTAVE_WINDOW_H

#include <qwidget.h>
#include <qprocess.h>
#include <qstringlist.h>

class QLineEdit;
class QTextEdit;
class QVBoxLayout;
class QDockWindow;


class OctaveWindow : public QWidget {
  Q_OBJECT
public:
  OctaveWindow(QDockWindow*);
 ~OctaveWindow();

  QSize sizeHint();
  bool startOctave();
  void runOctaveScript(const QString&);
  void sendCommand(const QString&);
  void adjustDirectory();

private slots:
  void slotDisplayMsg();
  void slotDisplayErr();
  void slotOctaveEnded();
  void slotSendCommand();

protected:
  void keyPressEvent(QKeyEvent*);

private:
  QVBoxLayout *vBox;
  QTextEdit *output;
  QLineEdit *input;

  QProcess octProcess;

  QStringList cmdHistory;
  QStringList::Iterator histIterator;
};

#endif
