
#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QMainWindow>
class MyResistanceBox;
class MyColorBox;
class QResistor;

// Application settings.
struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;          // font
  QString LangDir;     // translation directory
  QString Language;
};

//---------------------------class declarations------------------------------------//
class MyWidget: public QMainWindow
{
	Q_OBJECT
	QResistor *res;
	MyResistanceBox *resBox;
	MyColorBox *colorBox;

	public:
		MyWidget();
	public slots:

		void setResistanceValue();
		void setColorValue();
		void slotHelpAbout();
		void slotHelpAboutQt();
		void slotHelpIntro();
		void slotConfiguration();
	signals:
};

#endif /* MYWIDGET_H */
