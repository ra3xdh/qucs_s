#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_filter.h"
#include "qf_capacity.h"
#include "qf_tform.h"
#include "qf_box.h"
#include "qf_dialog.h"

#include <Q3Button>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QValidator>
#include <QPushButton>
#include <Q3ButtonGroup>
#include <qradiobutton.h>

// static  QButton::ToggleState  OrderBoxState = QButton::Off;

// Constructor

qf_box::qf_box (QWidget* parent) : FilterDialog (parent) {

  unsigned  i;

  // Setup comboboxes according to qf_common.h contents
  for (i = 0; qf_filter_apis [i] != NULL ; i ++)
    FilterName -> insertItem (qf_filter_apis [i] -> name);

  for (i = 0; qf_tform_apis [i] != NULL; i ++)
    TformName -> insertItem (qf_tform_apis [i] -> name);
    
  filter_index = 0;
  tform_index = 0;
  struct qf_filter_api*	fapi = qf_filter_apis [filter_index];
  struct qf_tform_api*	tapi = qf_tform_apis [tform_index];

  fflags = fapi -> gen_flags;
  tflags = tapi -> gen_flags;
  cflags = fflags & tflags;


  // Setup validators for frequencies
  EnterCutoff -> setValidator (new QDoubleValidator (1e-3, 1e6, 6, this));
  EnterBandwidth -> setValidator (new QDoubleValidator (1e-3, 1e6, 6, this));
  EnterStopband -> setValidator (new QDoubleValidator (1e-3, 1e6, 6, this));
  // Ripple cannot exceed 3 dB and is given with three decimal max
  EnterRipple -> setValidator (new QDoubleValidator (1e-6, 3, 3, this));
  // The angle of Cauer filters must lie in [1, 89]
  EnterAngle -> setValidator (new QDoubleValidator (1, 89, 1, this));
  // Attenuation cannot go below 3 dB and exceed 100 dB
  EnterAttenuation -> setValidator (new QDoubleValidator (3, 100, 1, this));
  // Impedance cannot exceed 10 kOhm
  EnterZin -> setValidator (new QDoubleValidator (1, 10000, 1, this));
  EnterZout -> setValidator (new QDoubleValidator (1, 10000, 1, this));

  // Clear all fields
  EnterCutoff -> setText (QString::null);
  EnterRipple -> setText (QString::null);
  EnterAngle -> setText (QString::null);
  EnterBandwidth -> setText (QString::null);
  EnterStopband -> setText (QString::null);
  EnterAttenuation -> setText (QString::null);
  EnterZin -> setText (QString::null);
  EnterZout -> setText (QString::null);

  // Hide Optimizing checkboxes

  Cboxes -> hide ();
  Lboxes -> hide ();

  // Connect radio buttons update to semanticCheck

  connect (NoC, SIGNAL (clicked (void)), this, SLOT (semanticCheck (void)));
  connect (NoL, SIGNAL (clicked (void)), this, SLOT (semanticCheck (void)));
  connect (Cmin, SIGNAL (clicked (void)), this, SLOT (semanticCheck (void)));
  connect (Cmax, SIGNAL (clicked (void)), this, SLOT (semanticCheck (void)));
  connect (Lmin, SIGNAL (clicked (void)), this, SLOT (semanticCheck (void)));
  connect (Lmax, SIGNAL (clicked (void)), this, SLOT (semanticCheck (void)));

//
  
  connect (FilterName, SIGNAL (activated (int)), 
		  this, SLOT (on_FilterName_activated (int)));
  connect (TformName, SIGNAL (activated (int)), 
		  this, SLOT (on_TformName_activated (int)));
  connect (OrderCombo, SIGNAL (activated (int)), 
		  this, SLOT (on_OrderCombo_activated (int)));
  connect (SubOrderCombo, SIGNAL (activated (int)), 
		  this, SLOT (on_SubOrderCombo_activated (int)));
  connect (CutoffCombo, SIGNAL (activated (int)), 
		  this, SLOT (on_CutoffCombo_activated (int)));
  connect (StopbandCombo, SIGNAL (activated (int)), 
		  this, SLOT (on_StopbandCombo_activated (int)));
  connect (BandwidthCombo, SIGNAL (activated (int)), 
		  this, SLOT (on_BandwidthCombo_activated (int)));
//
  connect (OrderBox, SIGNAL (stateChanged (int)), 
		  this, SLOT (on_OrderBox_stateChanged (int)));
  connect (DualBox, SIGNAL (stateChanged (int)), 
		  this, SLOT (on_DualBox_stateChanged (int)));
  connect (CauerPoleBox, SIGNAL (stateChanged (int)), 
		  this, SLOT (on_CauerPoleBox_stateChanged (int)));
  connect (OptimizeCauerBox, SIGNAL (stateChanged (int)), 
		  this, SLOT (on_OptimizeCauerBox_stateChanged (int)));

//

  connect (EnterCutoff, SIGNAL (textChanged (const QString &)), 
		  this, SLOT (on_EnterCutoff_textChanged (const QString &)));
  connect (EnterStopband, SIGNAL (textChanged (const QString &)), 
		  this, SLOT (on_EnterStopband_textChanged (const QString &)));
  connect (EnterAttenuation, SIGNAL (textChanged (const QString &)), 
	      this, SLOT (on_EnterAttenuation_textChanged (const QString &)));
  connect (EnterBandwidth, SIGNAL (textChanged (const QString &)), 
		  this, SLOT (on_EnterBandwidth_textChanged (const QString &)));
  connect (EnterRipple, SIGNAL (textChanged (const QString &)), 
		  this, SLOT (on_EnterRipple_textChanged (const QString &)));
  connect (EnterAngle, SIGNAL (textChanged (const QString &)), 
		  this, SLOT (on_EnterAngle_textChanged (const QString &)));
  connect (EnterZin, SIGNAL (textChanged (const QString &)), 
		  this, SLOT (on_EnterZin_textChanged (const QString &)));
  connect (EnterZout, SIGNAL (textChanged (const QString &)), 
		  this, SLOT (on_EnterZout_textChanged (const QString &)));


  on_FilterName_activated (0);
}

// Perform a consistency check on the values typed in by the user
// Return false if there is a problem, e.g. the ripple is higher than
// attenuation, etc.

// otherwise prepare a suitable structure to be used by the main program

bool  qf_box::semanticCheck (void) {

  qf_double_t cutoff = (EnterCutoff -> text ()). toDouble ();
  qf_double_t ripple = (EnterRipple -> text ()). toDouble ();
  qf_double_t bdwth =  (EnterBandwidth -> text ()). toDouble ();
  qf_double_t stop =   (EnterStopband -> text ()). toDouble ();
  qf_double_t atten =  (EnterAttenuation -> text ()). toDouble ();
  qf_double_t zin =  (EnterZin -> text ()). toDouble ();
  qf_double_t zout =  (EnterZout -> text ()). toDouble ();

  // Scales frequencies to correct unit (Hz)
  cutoff *= pow (10.0, CutoffCombo -> currentItem () * 3);
  bdwth *= pow (10.0, BandwidthCombo -> currentItem () * 3);
  stop *= pow (10.0, StopbandCombo -> currentItem () * 3);

  // Fill in the specifications structure

  spec. fc = cutoff * 2 * M_PI;
  spec. fs = stop * 2 * M_PI;
  spec. bw = bdwth * 2 * M_PI;
  spec. ord_given = OrderBox -> isChecked ();

  if (spec. ord_given) {
    spec. ord = OrderCombo -> currentText (). toInt ();

    if (SubOrderCombo -> isEnabled ())
      spec. subord = SubOrderCombo -> currentItem () + 'b';
    else
      spec. subord = ' ';

    spec. ang = (EnterAngle -> text ()). toDouble () * M_PI / 180;
  }

  spec. amin = ripple;
  spec. amax = atten;
  spec. r1 = zin;
  spec. r2 = zout;
  if (spec. r2 == 0) spec.r2 = spec.r1;
    

  spec. dual = (DualBox -> state () == QCheckBox::On);
  spec. all_ind_equal = (EqualInductorBox -> state () == QCheckBox::On);
  spec. use_cross = (UseCrossBox -> state () == QCheckBox::On);
  spec. stop_is_pole = (CauerPoleBox -> state () == QCheckBox::On);

  // Optimization subdialog
  if ((spec. optc = OptimizeCauerBox -> isChecked ())) {
    if (NoC  -> isChecked ()) spec. Copt = 0;
    if (Cmin -> isChecked ()) spec. Copt = -1;
    if (Cmax -> isChecked ()) spec. Copt = 1;

    if (NoL  -> isChecked ()) spec. Lopt = 0;
    if (Lmin -> isChecked ()) spec. Lopt = -1;
    if (Lmax -> isChecked ()) spec. Lopt = 1;
  }

  spec. tform = tform_index;
  spec. filter = filter_index;

  // Check filter and transform private validator
  bool	res = true;

  if (qf_tform_apis [tform_index] -> valid != NULL)
    res = qf_tform_apis [tform_index] -> valid (& spec);
  
  if (qf_filter_apis [filter_index] -> valid != NULL)
    res = res && qf_filter_apis [filter_index] -> valid (& spec);

  return res;
}

// Are all inputs acceptable?
// If yes, enable OK button, otherwise disable it.

bool  qf_box::dialogOk (void) {

  if (! EnterCutoff -> isEnabled ()) {
    okButton -> setEnabled (false);
    return false;
  }

  if ((EnterCutoff -> hasAcceptableInput ())	  &&
     ((! EnterRipple -> isEnabled ())      ||
       EnterRipple -> hasAcceptableInput ())	  &&
     ((! EnterAngle -> isEnabled ())       ||
       EnterAngle -> hasAcceptableInput ())	  &&
     ((! EnterBandwidth -> isEnabled ())   ||
       EnterBandwidth -> hasAcceptableInput ())	  &&
     ((! EnterStopband -> isEnabled ())    ||
       EnterStopband -> hasAcceptableInput ())	  &&
     ((! EnterAttenuation -> isEnabled ()) ||
       EnterAttenuation -> hasAcceptableInput ()) &&
     ((! EnterZout -> isEnabled ())	   ||
       EnterZout -> hasAcceptableInput ())	  &&
     EnterZin -> hasAcceptableInput ())
	  
	{if (semanticCheck ()) {okButton -> setEnabled (true);
				return true;}
	    else {okButton -> setEnabled (false);
		  return false;} }
  else	{okButton -> setEnabled (false);
	 return false;}
}

// This function updates the dialog according to the common capacities of
// the current transform and filter

void  qf_box::updateDialog (void) {

  struct qf_filter_api*	fapi = qf_filter_apis [filter_index];
  struct qf_tform_api*	tapi = qf_tform_apis [tform_index];

  fflags = fapi -> gen_flags;
  tflags = tapi -> gen_flags;
  cflags = fflags & tflags;

  // The capacity flags are the logical OR of general capacity flags
  // and the flags corresponding to the current order (if selected)

  if (OrderBox -> state () == QCheckBox::Off ||
      ! (cflags & CAN_ORDER)) {
    fflags |= fapi -> if_no_order;
    tflags |= tapi -> if_no_order;
  }
  else {
    if (OrderCombo -> currentText (). toInt () % 2) { 
      fflags |= fapi -> odd;
      tflags |= tapi -> odd;
    }
    else {
      fflags |= fapi -> even;
      tflags |= tapi -> even;
    }
  }

  cflags = fflags & tflags;

  // Now enables all fields valid both in f- and tflags
  StopbandLabel -> setEnabled (cflags & CAN_STOPBAND);
  EnterStopband -> setEnabled (cflags & CAN_STOPBAND);
  StopbandCombo -> setEnabled (cflags & CAN_STOPBAND);

  RippleLabel -> setEnabled (cflags & CAN_RIPPLE);
  EnterRipple -> setEnabled (cflags & CAN_RIPPLE);

  AttenuationLabel -> setEnabled (cflags & CAN_ATTENUATION);
  EnterAttenuation -> setEnabled (cflags & CAN_ATTENUATION);

  ZoutLabel -> setEnabled (cflags & CAN_IMPEDANCE2);
  EnterZout -> setEnabled (cflags & CAN_IMPEDANCE2);
  on_EnterZin_textChanged ("");

  AngleLabel -> setEnabled (cflags & CAN_ANGLE);
  EnterAngle -> setEnabled (cflags & CAN_ANGLE);

  BandwidthLabel -> setEnabled (cflags & CAN_BANDPASS);
  EnterBandwidth -> setEnabled (cflags & CAN_BANDPASS);
  BandwidthCombo -> setEnabled (cflags & CAN_BANDPASS);

  // Enables boxes. This is done based only on filter capacity
  OptimizeCauerBox -> setEnabled (fflags & CAN_OPTIMIZE);
  CauerPoleBox -> setEnabled (fflags & CAN_POLE);

  // Order stuff

  OrderBox -> setEnabled (cflags & CAN_ORDER);
  if (!(cflags & CAN_ORDER)) {
    OrderBox -> setChecked (false);
  }

  if (cflags & CAN_ORDER_ONLY) {
    if (OrderBox -> state () == QCheckBox::Off)
      OrderBox -> setChecked (true);
    OrderBox -> setEnabled (false);
  }

  SubOrderCombo -> setEnabled (cflags & CAN_SUBORDER);
  if (SubOrderCombo -> isEnabled ()) 
    on_SubOrderCombo_activated (SubOrderCombo -> currentItem ());

}

// This function populates the order combo according to what is
// possible (logical AND between filter and transform possibilities)

void  qf_box::populateOrderCombo (void) {

  // At first, nothing is permitted

  vector <bool> ok (QF_MAX_ORD + 1, false);

  OrderCombo -> clear ();

  // First, we consider filter restrictions
  // Unless otherwise specified, there is no order 2 filter

  if (fflags & CAN_ALL_ORDERS) 
    for (unsigned i = 3; i <= QF_MAX_ORD; i ++) ok [i] = true;

  else {
    if (fflags & CAN_ORDER_EVEN)
      for (unsigned i = 4; i <= QF_MAX_ORD; i += 2) ok [i] = true;

    if (fflags & CAN_ORDER_ODD)
      for (unsigned i = 3; i <= QF_MAX_ORD; i += 2) ok [i] = true;
  }

  // Check for forbidden orders

  if (qf_filter_apis [filter_index] -> forbid_orders && 
	(! qf_filter_apis [filter_index] -> only_some_orders)) 
    for (unsigned i = 0; 
	 qf_filter_apis [filter_index] -> f_ord_list [i] != 0; i ++)
      ok [qf_filter_apis [filter_index] -> f_ord_list [i]] = false;

  // Check for only some orders allowed

  if (qf_filter_apis [filter_index] -> only_some_orders && 
	(! qf_filter_apis [filter_index] -> forbid_orders)) 
    for (unsigned i = 0;
       	 qf_filter_apis [filter_index] -> f_ord_list [i] != 0; i ++)
      ok [qf_filter_apis [filter_index] -> f_ord_list [i]] = true;

  // Next, transformation restrictions (AND of the restrictions above)

  if ((tflags & CAN_ALL_ORDERS) == 0) {
    if ((tflags & CAN_ORDER_EVEN) == 0)
      for (unsigned i = 4; i <= QF_MAX_ORD; i += 2)
	ok [i] = false;

    if ((tflags & CAN_ORDER_ODD) == 0)
      for (unsigned i = 3; i <= QF_MAX_ORD; i += 2)
	ok [i] = false;
  }

  // Now populate combobox
  // and verifies that there is at least one order allowed

  bool	at_least_one = false;

  for (unsigned i = 2; i <= QF_MAX_ORD; i ++) 
    if (ok [i]) {
      at_least_one = true;
      QString Sval;
      Sval. setNum (i);
      OrderCombo -> insertItem (Sval);
    }

  // Disable dialog is no order permitted
  if (OrderCombo -> isEnabled ())
    EnterCutoff -> setEnabled (at_least_one);

}

void  qf_box::on_FilterName_activated (int) {

  // First find the index of the selected filter
  for (unsigned i = 0; qf_filter_apis [i] != NULL; i ++) {
    if (qf_filter_apis [i] -> name == FilterName -> currentText ()) {
      filter_index = i;
      break;
    }
  }

   // Update capabilities
  struct qf_filter_api*	fapi = qf_filter_apis [filter_index];
  struct qf_tform_api*	tapi = qf_tform_apis [tform_index];


  // First update the TformName combo to avoid incompatible transforms
  bool is_cptbl = false;

  TformName -> clear ();
  // First the current transform
  if (! (fapi -> forbid_tform & tapi -> id)) {
    
    is_cptbl = true;
    TformName -> insertItem (tapi -> name);
  }

  for (unsigned i = 0; qf_tform_apis [i] != NULL; i ++) {
    if ((i == tform_index) && is_cptbl) continue;
    if (fapi -> forbid_tform & qf_tform_apis [i] -> id) continue;
    TformName -> insertItem (qf_tform_apis [i] -> name);
    if (! is_cptbl) {
      tform_index = i;
      is_cptbl = true;
    }
  }
  TformName -> setCurrentItem (0);

  tapi = qf_tform_apis [tform_index];
  fflags = fapi -> gen_flags;
  tflags = tapi -> gen_flags;
  cflags = fflags & tflags;

  populateOrderCombo ();
  updateDialog ();
  
  // Synthesize a fake event for suborder combo handling
  on_OrderBox_stateChanged (OrderBox -> state ());

  return;
}

// This function handles the various passband topologies

void  qf_box::on_TformName_activated (int) {

  // Find the index of the selected transform
  for (unsigned i = 0; qf_tform_apis [i] != NULL; i ++) {
    if (qf_tform_apis [i] -> name == TformName -> currentText ()) {
      tform_index = i;
      break;
    }
  }

  tflags = qf_tform_apis [tform_index] -> gen_flags;
  cflags = fflags & tflags;

/*
  // Update the Filter combo to avoid incompatible filters

  FilterName -> clear ();
  // First the current filter
  if (! (qf_filter_apis [filter_index] -> forbid_tform 
       	 & qf_tform_apis [tform_index] -> id)) 
    FilterName -> insertItem (qf_filter_apis [filter_index] -> name);

  for (unsigned i = 0; qf_filter_apis [i] != NULL; i ++) {
    if (i == filter_index) continue;
    if (qf_filter_apis [i] -> forbid_tform 
	& qf_tform_apis [tform_index] -> id) continue;

    FilterName -> insertItem (qf_filter_apis [i] -> name);
  }
  FilterName -> setCurrentItem (0);

  updateDialog ();
*/
  populateOrderCombo ();
  updateDialog ();
  dialogOk ();
  return;
}

void  qf_box::on_OrderCombo_activated (int) {

  updateDialog ();
  dialogOk ();
}

// The following slot handles 'b' and 'c' suborder combo
// Since it is used only by Cauer filters, we hardcode the behavior
// instead of making it flexible through the api usage. That would
// complexify the whole stuff unnecessarily.

void qf_box::on_SubOrderCombo_activated (int type) {
  
  EnterZout -> setEnabled (type == 0);
  ZoutLabel -> setEnabled (type == 0);
  if (type == 0) {
    CauerPoleBox -> setChecked (false);
  }
  CauerPoleBox -> setEnabled (type == 1);
  EnterRipple -> setEnabled (type == 1);
  RippleLabel -> setEnabled (type == 1);
  DualBox -> setEnabled (type == 1);

  dialogOk ();
}

void  qf_box::on_OrderBox_stateChanged (int state) {

  if ((state == QCheckBox::On) && (cflags & CAN_ORDER)) {
    OrderCombo -> setEnabled (true);
    on_OrderCombo_activated (OrderCombo -> currentItem ());
  }
  else {
    OrderCombo -> setEnabled (false);
    SubOrderCombo -> setEnabled (false);
  }

  // Enable SubOrderCombo if necessary and synthesize a fake activate event
  SubOrderCombo -> setEnabled (cflags & CAN_SUBORDER);
  if (SubOrderCombo -> isEnabled ()) 
    on_SubOrderCombo_activated (SubOrderCombo -> currentItem ());

  // Validates this state of affairs
  updateDialog ();
  dialogOk ();
}

void  qf_box::on_EnterCutoff_textChanged  (const QString &) {

  dialogOk ();
}
  
void  qf_box::on_EnterStopband_textChanged  (const QString &) {

  dialogOk ();
}
  
void  qf_box::on_EnterAttenuation_textChanged  (const QString &) {

  dialogOk ();
}
  
void  qf_box::on_EnterBandwidth_textChanged  (const QString &) {

  dialogOk ();
}
  
void  qf_box::on_EnterRipple_textChanged  (const QString &) {

  dialogOk ();
}
  
void  qf_box::on_EnterAngle_textChanged  (const QString &) {

  dialogOk ();
}

void  qf_box::on_EnterZin_textChanged  (const QString &) {

  // Zout follows Zin if disabled
  if (! EnterZout -> isEnabled ()) {
    EnterZout -> setText (EnterZin -> text ());
  }
  dialogOk ();
}

void  qf_box::on_EnterZout_textChanged  (const QString &) {

  dialogOk ();
}

void  qf_box::on_CutoffCombo_activated (int) {

  dialogOk ();
}

void  qf_box::on_StopbandCombo_activated (int) {

  dialogOk ();
}

void  qf_box::on_BandwidthCombo_activated (int) {

  dialogOk ();
}

void  qf_box::on_DualBox_stateChanged (int) {

  dialogOk ();
}

void  qf_box::on_CauerPoleBox_stateChanged (int) {

  dialogOk ();
}

void  qf_box::on_OptimizeCauerBox_stateChanged (int) {

  if (OptimizeCauerBox -> isChecked ())
    Cboxes -> show ();
  else
    Cboxes -> hide ();
  if (OptimizeCauerBox -> isChecked ())
    Lboxes -> show ();
  else
    Lboxes -> hide ();
  
  // Handles dialog box vertical size change
  setMaximumSize (width (), 10000);
  setMinimumSize (width (), 1);
  adjustSize ();
  dialogOk ();
}
