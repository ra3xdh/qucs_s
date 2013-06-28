/***************************************************************************
    copyright            : (C) 2010 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

#ifndef OCTAVE_WINDOW_H
#define OCTAVE_WINDOW_H

#include <QWidget>
#include <QProcess>
#include <QStringList>
#include <QKeyEvent>
#include <Q3VBoxLayout>
#include <QDockWidget>

class QLineEdit;
class Q3TextEdit;
class Q3VBoxLayout;
class Q3DockWindow;


class OctaveWindow : public QWidget {
  Q_OBJECT
public:
  OctaveWindow(QDockWidget*);
 ~OctaveWindow();

  QSize sizeHint();
  bool startOctave();
  void runOctaveScript(const QString&);
  void sendCommand(const QString&);
  void adjustDirectory();

private slots:
  void slotDisplayMsg();
  void slotDisplayErr();
  void slotOctaveEnded(int status);
  void slotSendCommand();

protected:
  void keyPressEvent(QKeyEvent*);

private:
  Q3VBoxLayout *vBox;
  Q3TextEdit *output;
  QLineEdit *input;

  QProcess octProcess;

  QStringList cmdHistory;
  QStringList::Iterator histIterator;
};

#endif
