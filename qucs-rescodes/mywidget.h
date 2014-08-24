#include <QWidget>
class MyResistanceBox;
class MyColorBox;
class QResistor;

//---------------------------class declarations------------------------------------//
class MyWidget: public QWidget
{
	Q_OBJECT
	QResistor *res;
	MyResistanceBox *resBox;
	MyColorBox *colorBox;

	public:
		MyWidget( QWidget *parent=0, const char *name=0 );
	public slots:

		void setResistanceValue();
		void setColorValue();
		void slotHelpAbout();
		void slotHelpAboutQt();
		void slotHelpIntro();
		void slotConfiguration();
	signals:
};



