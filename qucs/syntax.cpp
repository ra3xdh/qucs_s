/***************************************************************************
                              syntax.cpp
                             ------------
    begin                : Sat Mar 11 2006
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// *****************************************************************
// *********                                              **********
// *********  The class that does the syntax highlighting **********
// *********                                              **********
// *****************************************************************

#include "textdoc.h"
#include "syntax.h"


SyntaxHighlighter::SyntaxHighlighter(TextDoc *textEdit) : QSyntaxHighlighter(textEdit)
{
  Doc = textEdit;
  language = LANG_NONE;

  reservedWordFormat.setForeground(Qt::darkBlue);
  reservedWordFormat.setFontWeight(QFont::Bold);

  unitFormat.setForeground(Qt::darkRed);;
  unitFormat.setFontWeight(QFont::StyleItalic);;

  datatypeFormat.setForeground(Qt::darkBlue);;
  datatypeFormat.setFontWeight(QFont::Bold);;

  directiveFormat.setForeground(Qt::darkBlue);;
  directiveFormat.setFontWeight(QFont::Bold);;

  functionFormat.setForeground(Qt::darkGreen);;
  functionFormat.setFontWeight(QFont::Bold);;

  commentFormat.setForeground(Qt::gray);;
  commentFormat.setFontWeight(QFont::StyleItalic);;

}

SyntaxHighlighter::~SyntaxHighlighter()
{
}

// ---------------------------------------------------
void SyntaxHighlighter::setLanguage(int lang)
{
  language = lang;

  HighlightingRule rule;

  QStringList reservedWordPattern;
  QStringList unitPattern;
  QStringList datatypePattern;
  QStringList directivePattern;
  QStringList functionPattern;
  QStringList commentPattern;

  switch (language) {
    case LANG_VHDL:
        reservedWordPattern << "\\babs\\b" << "\\baccess\\b" << "\\bafter\\b" << "\\balias\\b" << "\\ball\\b" << "\\band\\b" <<
                        "\\barchitecture\\b\\b"<< "\\barray\\b"<< "\\bassert\\b" << "\\battribute\\b"<<
                        "\\bbegin\\b\\b"<< "\\bblock\\b"<< "\\bbody\\b"<< "\\bbuffer\\b"<< "\\bbus\\b"<<
                        "\\bcase\\b"<< "\\bcomponent\\b"<< "\\bconfiguration\\b"<< "\\bconstant\\b"<<
                        "\\bdisconnect\\b"<< "\\bdownto\\b"<<
                        "\\belse\\b"<< "\\belsif\\b"<< "\\bend\\b"<< "\\bentity\\b"<< "\\bexit\\b"<<
                        "\\bfile\\b"<< "\\bfor\\b"<< "\\bfunction"
                        "\\bgenerate\\b"<< "\\bgeneric\\b"<< "\\bgroup\\b"<< "\\bguarded"
                        "\\bif\\b"<< "\\bimpure\\b"<< "\\bin\\b"<< "\\binertial\\b"<< "\\binout\\b"<< "\\bis\\b"<<
                        "\\blabel\\b"<< "\\blibrary\\b"<< "\\blinkage\\b"<< "\\bliteral\\b"<< "\\bloop\\b"<<
                        "\\bmap\\b"<< "\\bmod\\b"<< 
                        "\\bnand\\b"<< "\\bnew\\b"<< "\\bnext\\b"<< "\\bnor\\b"<< "\\bnot\\b"<< "\\bnull"
                        "\\bof\\b"<< "\\bon\\b"<< "\\bopen\\b"<< "\\bor\\b"<< "\\bothers\\b"<< "\\bout\\b"<<
                        "\\bpackage\\b"<< "\\bport\\b"<< "\\bpostponed\\b"<< "\\bprocedure\\b"<< "\\bprocess\\b"<< "\\bpure\\b"<<
                        "\\brange\\b"<< "\\brecord\\b"<< "\\bregister\\b"<< "\\breject\\b"<< "\\brem\\b"<< "\\breport\\b"<< "\\breturn\\b"<< "\\brol\\b"<< "\\bror\\b"<<
                        "\\bselect\\b"<< "\\bseverity\\b"<< "\\bshared\\b"<< "\\bsignal\\b"<< "\\bsla\\b"<< "\\bsll\\b"<< "\\bsra\\b"<< "\\bsrl\\b"<< "\\bsubtype\\b"<<
                        "\\bthen\\b"<< "\\bto\\b"<< "\\btransport\\b"<< "\\btype\\b"<<
                        "\\bunaffected\\b"<< "\\bunits\\b"<< "\\buntil\\b"<< "\\buse\\b"<<
                        "\\bvariable\\b"<<
                        "\\bwait\\b"<< "\\bwhen\\b"<< "\\bwhile\\b"<< "\\bwith\\b"<<
                        "\\bxnor\\b"<< "\\bxor\\b";

          unitPattern <<  "\\bfs\\b"<< "\\bps\\b"<< "\\bns\\b"<< "\\bus\\b"<< "\\bms\\b"<< "\\bsec\\b"<< "\\bmin\\b"<< "\\bhr\\b";

          datatypePattern << "\\bbit\\b"<< "\\bbit_vector\\b"<< "\\bboolean\\b"<< "\\bstd_logic\\b"<< "\\bstd_logic_vector\\b"<< "\\bstd_ulogic\\b"<< "\\bstd_ulogic_vector\\b"<< "\\bsigned\\b"<< "\\bunsigned\\b"<< "\\binteger\\b"<< "\\breal\\b"<< "\\btime\\b"<< "\\bcharacter\\b"<< "\\bnatural\\b";

          directivePattern << "\\bactive\\b"<< "\\bascending\\b" <<
                        "\\bbase\\b" <<
                        "\\bdelayed\\b" <<
                        "\\bevent\\b" <<
                        "\\bhigh\\b" <<
                        "\\bimage\\b" <<
                        "\\blast_active\\b"<< "\\blast_event\\b"<< "\\blast_value\\b"<< "\\bleft\\b"<< "\\bleftof\\b"<< "\\blength\\b"<< "\\blow\\b" <<
                        "\\bpos\\b"<< "\\bpred\\b"<<
                        "\\bquiet\\b"<<
                        "\\brange\\b"<< "\\breverse_range\\b"<< "\\bright\\b"<< "\\brightof\\b"<<
                        "\\bstable\\b"<< "\\bsucc\\b"<<
                        "\\btransaction\\b"<<
                        "\\bval\\b"<< "\\bvalue\\b";

          commentPattern << "--[^\n]*";

        break;

  case LANG_VERILOG:
        reservedWordPattern << "\\balways\\b"<< "\\band\\b"<< "\\bassign\\b"<< "\\battribute\\b"<<
                        "\\bbegin\\b"<< "\\bbuf\\b"<< "\\bbufif0\\b"<< "\\bbufif1\\b"<<
                        "\\bcase\\b"<< "\\bcasex\\b"<< "\\bcasez\\b" <<  "\\bcmos\\b"<< 
                        "\\bdeassign\\b"<< "\\bdefault\\b"<< "\\bdefparam\\b"<< "\\bdisable\\b"<<
                        "\\bedge\\b"<< "\\belse\\b"<< "\\bend\\b"<< "\\bendattribute\\b"<< "\\bendcase\\b"<< "\\bendfunction\\b"<< "\\bendmodule\\b"<< "\\bendprimitive\\b"<< "\\bendspecify\\b"<< "\\bendtable\\b"<< "\\bendtask\\b"<< "\\bevent\\b"<<
                        "\\bfor\\b"<< "\\bforce\\b"<< "\\bforever\\b"<< "\\bfork\\b"<< "\\bfunction\\b"<<
                        "\\bhighz0\\b"<< "\\bhighz1\\b"<<
                        "\\bif\\b"<< "\\bifnone\\b"<< "\\binitial\\b"<< "\\binout\\b"<< "\\binput\\b"<<
                        "\\bjoin\\b"<<
                        "\\blarge\\b"<<
                        "\\bmedium\\b"<< "\\bmodule\\b"<< "\\bmacromodule\\b"<<
                        "\\bnand\\b"<< "\\bnegedge\\b"<< "\\bnmos\\b"<< "\\bnor\\b"<< "\\bnot\\b"<< "\\bnotif0\\b"<< "\\bnotif1\\b"<<
                        "\\bor\\b"<< "\\boutput\\b"<<
                        "\\bpmos\\b"<< "\\bposedge\\b"<< "\\bprimitive\\b"<< "\\bpull0\\b"<< "\\bpull1\\b"<< "\\bpulldown\\b"<< "\\bpullup\\b"<<
                        "\\brcmos\\b"<< "\\brelease\\b"<< "\\brepeat\\b"<< "\\brnmos\\b"<< "\\brpmos\\b"<< "\\brtran\\b"<< "\\brtranif0\\b"<< "\\brtranif1\\b"<<
                        "\\bscalared\\b"<< "\\bsigned\\b"<< "\\bsmall\\b"<< "\\bspecify\\b"<< "\\bstrength\\b"<< "\\bstrong0\\b"<< "\\bstrong1\\b"<<
                        "\\btable\\b"<< "\\btask\\b"<< "\\btran\\b"<< "\\btranif0\\b"<< "\\btranif1\\b"<<
                        "\\bunsigned\\b"<<
                        "\\bvectored\\b"<<
                        "\\bwait\\b"<< "\\bweak0\\b"<< "\\bweak1\\b"<< "\\bwhile\\b"<<
                        "\\bxnor\\b"<< "\\bxor\\b";

        directivePattern << "\\breset_all\\b"<< "\\btimescale\\b"<< "\\bdefine\\b"<< "\\binclude\\b"<< "\\bifdef\\b"<< "\\belse\\b"<< "\\bendif\\b"<<
                          "\\bcelldefine\\b"<< "\\bendcelldefine\\b"<< "\\bdefault_nettype\\b"<< "\\bunconnected_drive\\b"<<
                          "\\bnounconnected_drive\\b"<< "\\bdelay_mode_zero\\b"<< "\\bdelay_mode_unit\\b"<<
                          "\\bdelay_mode_path\\b"<< "\\bdelay_mode_distributed\\b"<< "\\buselib\\b";

        datatypePattern << "\\breg\\b"<< "\\binteger\\b"<< "\\btime\\b"<< "\\breal\\b"<< "\\brealtime\\b"<< "\\bwire\\b"<< "\\btri\\b"<< "\\bwor\\b"<<
                          "\\btrior\\b"<< "\\bwand\\b"<< "\\btriand\\b"<< "\\btri0\\b"<< "\\btri1\\b"<< "\\bsupply0\\b"<< "\\bsupply1\\b"<< "\\btrireg\\b"<<
                          "\\bparameter\\b"<< "\\bspecparam\\b"<< "\\bevent\\b";

        functionPattern <<  "\\bsetup\\b"<< "\\bhold\\b"<< "\\bsetuphold\\b"<< "\\bskew\\b"<< "\\brecovery\\b"<< "\\bperiod\\b"<< "\\bwidth\\b"<<
                          "\\bmonitor\\b"<< "\\bdisplay\\b"<< "\\bwrite\\b"<< "\\bstrobe\\b"<< "\\bfopen\\b"<< "\\bfclose\\b"<< "\\btime\\b"<<
                          "\\bstime\\b"<< "\\brealtime\\b"<< "\\btimeformat\\b"<< "\\bprinttimescale\\b"<< "\\brandom\\b"<< "\\breadmemb\\b"<<
                          "\\breadmemh\\b"<< "\\bfinish\\b"<< "\\bstop\\b";

        commentPattern << "//[^\n]*";

    break;

  case LANG_VERILOGA:

        reservedWordPattern << "\\babstol\\b"<< "\\baccess\\b"<< "\\banalog\\b"<< "\\bac_stim\\b"<< "\\banalysis\\b"<<
                          "\\bbegin\\b"<< "\\bbranch\\b"<< "\\bbound_step\\b"<<
                          "\\bcase\\b"<<
                          "\\bdiscipline\\b"<< "\\bddt_nature\\b"<< "\\bddt\\b"<< "\\bdelay\\b"<< "\\bdiscontinuity\\b"<< "\\bdefault\\b"<< 
                          "\\benddiscipline\\b"<< "\\belse\\b"<< "\\bend\\b"<< "\\bendnature\\b"<< "\\bexclude\\b"<< "\\bendfunction\\b"<< "\\bendmodule\\b"<< "\\belectrical\\b"<< "\\bendcase\\b"<<
                          "\\bfor\\b"<< "\\bflow\\b"<< "\\bfrom\\b"<< "\\bfinal_step\\b"<< "\\bflicker_noise\\b"<< "\\bfunction\\b"<<
                          "\\bgenerate\\b"<< "\\bground\\b"<<
                          "\\bif\\b"<< "\\bidt_nature\\b"<< "\\binf\\b"<< "\\bidt\\b"<< "\\binitial_step\\b"<< "\\binput\\b"<< "\\binout\\b"<<
                          "\\blaplace_nd\\b"<< "\\blaplace_np\\b"<< "\\blaplace_zd\\b"<< "\\blaplace_zp\\b"<< "\\blast_crossing\\b"<<
                          "\\bmodule\\b"<<
                          "\\bnature\\b"<< "\\bnoise_table\\b"<<
                          "\\bpotential\\b"<< "\\bparameter\\b"<<
                          "\\bslew\\b"<<
                          "\\btimer\\b"<< "\\btransition\\b"<<
                          "\\bunits\\b"<<
                          "\\bwhite_noise\\b"<< "\\bwhile\\b"<<
                          "\\bzi_nd\\b"<< "\\bzi_np\\b"<< "\\bzi_zd\\b"<< "\\bzi_zp\\b";

        unitPattern << "\\bT\\b"<< "\\bG\\b"<< "\\bM\\b"<< "\\bK\\b"<< "\\bm\\b"<< "\\bu\\b"<< "\\bn\\b"<< "\\bp\\b"<< "\\bf\\b"<< "\\ba\\b";

        datatypePattern << "\\binteger\\b"<< "\\breal\\b";

        directivePattern << "\\bdefine\\b"<< "\\belse\\b"<< "\\bundef\\b"<< "\\bifdef\\b"<< "\\bendif\\b"<< "\\binclude\\b"<< "\\bresetall\\b";

        functionPattern << "\\brealtime\\b"<< "\\btemperature\\b"<< "\\bvt\\b"<< "\\bdisplay\\b"<< "\\bstrobe\\b";

        commentPattern << "//[^\n]*";

    break;
  case LANG_OCTAVE:
      
        reservedWordPattern << "\\bcase\\b"<< "\\bcatch\\b"<<
                          "\\belse\\b"<< "\\belseif\\b"<< "\\bend\\b"<< "\\bendfor\\b"<< "\\bendfunction\\b"<< "\\bendif\\b"<< "\\bendswitch\\b"<< "\\bend_try_catch\\b"<< "\\bendwhile\\b"<< "\\bend_unwind_protect\\b"
                          "\\bfor\\b" << "\\bfunction\\b" <<
                          "\\bif\\b" <<
                          "\\botherwise\\b" <<
                          "\\bswitch\\b" <<
                          "\\btry\\b" <<
                          "\\bunwind_protect\\b"<< "\\bunwind_protect_cleanup\\b" <<
                          "\\bwhile\\b";

        datatypePattern << "\\binf\\b"<< "\\bnan\\b"<< "\\bpi\\b";

        functionPattern << "\\bplot\\b";

        commentPattern << "//[^\n]*";

    break;
    }

  foreach (const QString &pattern, reservedWordPattern) {
    rule.pattern = QRegExp(pattern);
    rule.format = reservedWordFormat;
    highlightingRules.append(rule);
  }

  foreach (const QString &pattern, unitPattern) {
    rule.pattern = QRegExp(pattern);
    rule.format = unitFormat;
    highlightingRules.append(rule);
  }

  foreach (const QString &pattern, datatypePattern) {
    rule.pattern = QRegExp(pattern);
    rule.format = datatypeFormat;
    highlightingRules.append(rule);
  }

  foreach (const QString &pattern, directivePattern) {
    rule.pattern = QRegExp(pattern);
    rule.format = directiveFormat;
    highlightingRules.append(rule);
  }

  foreach (const QString &pattern, functionPattern) {
    rule.pattern = QRegExp(pattern);
    rule.format = functionFormat;
    highlightingRules.append(rule);
  }

  foreach (const QString &pattern, commentPattern) {
    rule.pattern = QRegExp(pattern);
    rule.format = commentFormat;
    highlightingRules.append(rule);
  }
}

// ---------------------------------------------------
void SyntaxHighlighter::highlightBlock(const QString& text) {


foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
  }

}
