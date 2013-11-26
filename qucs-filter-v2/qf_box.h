// API for the dialog box
// We need those Qt headers in order to compile properly
#include <QObject>
#include <QApplication>
#include "qf_common.h"
#include "qf_dialog.h"

class	qf_box : public FilterDialog {

  Q_OBJECT

  private:
    qf_spec	  spec;
    unsigned	  filter_index;			// Current filter index
    unsigned	  tform_index;			// Current transform index

    unsigned	  fflags;			// Current filter flags
    unsigned	  tflags;			// Current transform flags
    unsigned	  cflags;			// And of both

    bool    dialogOk				(void);

    void    updateDialog			(void);
    void    populateOrderCombo			(void);

  public slots:

    void    on_FilterName_activated		(int);
    void    on_TformName_activated		(int);

  private slots:

    bool    semanticCheck			(void);

    void    on_OrderBox_stateChanged		(int);
    void    on_OrderCombo_activated		(int);
    void    on_SubOrderCombo_activated		(int);
    void    on_EnterCutoff_textChanged		(const QString &);
    void    on_EnterStopband_textChanged	(const QString &);
    void    on_EnterBandwidth_textChanged	(const QString &);
    void    on_EnterAttenuation_textChanged	(const QString &);
    void    on_EnterZin_textChanged		(const QString &);
    void    on_EnterZout_textChanged		(const QString &);
    void    on_EnterAngle_textChanged		(const QString &);
    void    on_EnterRipple_textChanged		(const QString &);
    void    on_CutoffCombo_activated		(int);
    void    on_StopbandCombo_activated		(int);
    void    on_BandwidthCombo_activated		(int);
    void    on_DualBox_stateChanged		(int);
    void    on_CauerPoleBox_stateChanged	(int);
    void    on_OptimizeCauerBox_stateChanged	(int);

  public:
	    qf_box				(QWidget* parent = 0);
	    ~qf_box				(void) {};
    qf_spec* get_spec				(void) {return & spec; }

};
