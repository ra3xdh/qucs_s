
#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QMainWindow>
class MyResistanceBox;
class MyColorBox;
class QResistor;

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
