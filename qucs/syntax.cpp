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
// **********                                             **********
// **********  The class that do the syntax highlighting  **********
// **********                                             **********
// *****************************************************************
#include <QtGui>
#include <q3syntaxhighlighter.h>

#include "textdoc.h"
#include "syntax.h"
#include "main.h"

SyntaxHighlighter::SyntaxHighlighter(TextDoc *textEdit) 
                 : Q3SyntaxHighlighter(textEdit)
{
  Doc = textEdit;
  language = LANG_NONE;
}

SyntaxHighlighter::~SyntaxHighlighter()
{
}

// ---------------------------------------------------
void SyntaxHighlighter::setLanguage(int lang)
{
  language = lang;
}

// ---------------------------------------------------
int SyntaxHighlighter::highlightParagraph(const QString& text, int state)
{
  QChar c;
  bool isFloat=false;
  int  iString=-1, iWord=-1, iNumber=-1, iExpo=-1, iComment=-1;
  QFont font = Doc->TextFont;
  font.setPointSize((int)Doc->Scale);
  setFormat(0, text.length(), font, Qt::black);

  if (state < 0)
    state = STATE_NONE;
  if (state >= STATE_COMMENT)
    iComment = 0;
  for(int i=0; i<text.size(); i++){
	c = text.at(i);
		
  //for(c = text.at(i); !c.isNull(); c = text.at(++i)) {
    // ----- current text is a comment ---------------------------
    if (iComment >= 0) {
      setFormat (iComment, i-iComment+1, QucsSettings.Comment);
      if (i > 0 && c == ')' && text.at (i-1) == '*') {
	if (--state < STATE_COMMENT) {
	  state = STATE_NONE;
	  iComment = -1;
	}
      }
    }
    // ----- current text is a string ----------------------------
    else if(iString >= 0) {
      setFormat(iString, i-iString+1, QucsSettings.String);
      if (language == LANG_OCTAVE)
	if(c == '\'')
	  iString = -1;
      if(c == '"')
        iString = -1;
      continue;
    }
    // ----- word that might become a reserved word --------------
    else if(iWord >= 0) {
      if(c.isLetterOrNumber())
        continue;
      if(c == '_')
        continue;
      do {
        if(iWord > 0)
          if(text.at(iWord-1) == '\'') {
            markAttribute(text, iWord, i-iWord);
            break;
          }
        markWord(text, iWord, i-iWord);
      } while(false);
      iWord = -1;
    }
    // ----- integer or floating point number --------------
    else if(iNumber >= 0) {
      if(c.isNumber())
        continue;
      if(c == '.') {
        if(iExpo < 0) {
          if(isFloat)
            iNumber = -1;
          isFloat = true;
        }
        else
          iNumber = -1;
        continue;
      }
      if((c == 'e') || (c == 'E')) {
        if(iExpo < 0) {
          iExpo = i;
          isFloat = true;
        }
        else
          iNumber = -1;
        continue;
      }
      if((c == '-') || (c == '+')) {
        if((iExpo+1) == i)
          continue;
      }
      if(c != '_')
        if(!c.isLetter()) {
          if(isFloat)
            setFormat(iNumber, i-iNumber, QucsSettings.Real);
          else
            setFormat(iNumber, i-iNumber, QucsSettings.Integer);
        }
      iNumber = -1;
    }
    // ----- maybe a VHDL comment -------------------------------
    else if(language == LANG_VHDL && c == '-') {
      if(i > 0)
        if(text.at(i-1) == '-') {  // VHDL comment starts with --
          setFormat(i-1, text.length()-i, QucsSettings.Comment);
          return state;
        }
      continue;
    }
    // ----- maybe a Verilog comment -------------------------------
    else if((language == LANG_VERILOG || language == LANG_VERILOGA)
	    && c == '/') {
      if(i > 0)
        if(text.at(i-1) == '/') {  // Verilog comment starts with //
          setFormat(i-1, text.length()-i, QucsSettings.Comment);
          return state;
        }
      continue;
    }
    // ----- maybe a Octave comment -------------------------------
    else if((language == LANG_OCTAVE) && (c == '%' || c == '#')) {
      setFormat(i, text.length()-i+1, QucsSettings.Comment);
      return state;
    }
    // ----- no special syntax yet (or anymore) --------------
    else {
      if((language == LANG_VERILOG || language == LANG_VERILOGA)
	 && c == '`' && text.at(i+1).isLetter())
	iWord = i;
      else if((language == LANG_VERILOG || language == LANG_VERILOGA)
	      && c == '$' && text.at(i+1).isLetter())
	iWord = i;
      else if(c.isLetter())
        iWord = i;     // start a word
      else if(c.isNumber()) {
        iExpo = -1;
        iNumber = i;   // start a number
        isFloat = false;
        c = text.at(i-1);
/*        if((c == '-') || (c == '+'))  // include sign into number
          iNumber--;*/
      }
    }

    if (state < STATE_COMMENT) {
      if(c == '\'' && language != LANG_OCTAVE) {
	if(i > 1)
	  if(text.at(i-2) == '\'')
	    setFormat(i-2, 3, QucsSettings.Character);
      }
      else if(c == '"' || (c == '\'' && language == LANG_OCTAVE))
	iString = i;
    }
    if ((language == LANG_VERILOG || language == LANG_VERILOGA)
	&& i > 0 && c == '*' && text.at(i-1) == '(') {
      if (state >= STATE_COMMENT) state++;
      else state = STATE_COMMENT;
      iComment = i-1;
    }
  }
    
  return state;
}

// ---------------------------------------------------
typedef const char*  pChar;
typedef const char** ppChar;

// ---------------------------------------------------
// reserved VHDL words in alphabetical order
pChar VHD_List_A[] = {"abs", "access", "after", "alias", "all", "and",
                      "architecture", "array", "assert", "attribute", 0};
pChar VHD_List_B[] = {"begin", "block", "body", "buffer", "bus", 0};
pChar VHD_List_C[] = {"case", "component", "configuration", "constant", 0};
pChar VHD_List_D[] = {"disconnect", "downto", 0};
pChar VHD_List_E[] = {"else", "elsif", "end", "entity", "exit", 0};
pChar VHD_List_F[] = {"file", "for", "function", 0};
pChar VHD_List_G[] = {"generate", "generic", "group", "guarded", 0};
pChar VHD_List_I[] = {"if", "impure", "in", "inertial", "inout", "is", 0};
pChar VHD_List_L[] = {"label", "library", "linkage", "literal", "loop", 0};
pChar VHD_List_M[] = {"map", "mod", 0};
pChar VHD_List_N[] = {"nand", "new", "next", "nor", "not", "null", 0};
pChar VHD_List_O[] = {"of", "on", "open", "or", "others", "out", 0};
pChar VHD_List_P[] = {"package", "port", "postponed", "procedure", "process",
                      "pure", 0};
pChar VHD_List_R[] = {"range", "record", "register", "reject", "rem", "report",
                      "return", "rol", "ror", 0};
pChar VHD_List_S[] = {"select", "severity", "shared", "signal", "sla", "sll",
                      "sra", "srl", "subtype", 0};
pChar VHD_List_T[] = {"then", "to", "transport", "type", 0};
pChar VHD_List_U[] = {"unaffected", "units", "until", "use", 0};
pChar VHD_List_V[] = {"variable", 0};
pChar VHD_List_W[] = {"wait", "when", "while", "with", 0};
pChar VHD_List_X[] = {"xnor", "xor", 0};

ppChar VHD_WordList[] =
  {(ppChar)&VHD_List_A, (ppChar)&VHD_List_B, (ppChar)&VHD_List_C,
   (ppChar)&VHD_List_D, (ppChar)&VHD_List_E, (ppChar)&VHD_List_F,
   (ppChar)&VHD_List_G, 0,                   (ppChar)&VHD_List_I,
   0,                   0,                   (ppChar)&VHD_List_L,
   (ppChar)&VHD_List_M, (ppChar)&VHD_List_N, (ppChar)&VHD_List_O,
   (ppChar)&VHD_List_P, 0,                   (ppChar)&VHD_List_R,
   (ppChar)&VHD_List_S, (ppChar)&VHD_List_T, (ppChar)&VHD_List_U,
   (ppChar)&VHD_List_V, (ppChar)&VHD_List_W, (ppChar)&VHD_List_X,
   0,                   0};

pChar VHD_List_Units[] =
  {"fs", "ps", "ns", "us", "ms", "sec", "min", "hr", 0};

pChar VHD_List_DataTypes[] = {
   "bit", "bit_vector", "boolean", "std_logic", "std_logic_vector",
   "std_ulogic", "std_ulogic_vector", "signed", "unsigned", "integer",
   "real", "time", "character", "natural", 0};

// ---------------------------------------------------
// reserved Verilog-HDL words in alphabetical order
pChar V_List_A[] = {"always", "and", "assign", "attribute", 0};
pChar V_List_B[] = {"begin", "buf", "bufif0", "bufif1", 0};
pChar V_List_C[] = {"case", "casex", "casez", "cmos", 0};
pChar V_List_D[] = {"deassign", "default", "defparam", "disable", 0};
pChar V_List_E[] = {"edge", "else", "end", "endattribute", "endcase",
		    "endfunction", "endmodule", "endprimitive", "endspecify",
		    "endtable", "endtask", "event", 0};
pChar V_List_F[] = {"for", "force", "forever", "fork", "function", 0};
pChar V_List_H[] = {"highz0", "highz1", 0};
pChar V_List_I[] = {"if", "ifnone", "initial", "inout", "input", 0};
pChar V_List_J[] = {"join", 0};
pChar V_List_L[] = {"large", 0};
pChar V_List_M[] = {"medium", "module", "macromodule", 0};
pChar V_List_N[] = {"nand", "negedge", "nmos", "nor", "not", "notif0",
		    "notif1", 0};
pChar V_List_O[] = {"or", "output", 0};
pChar V_List_P[] = {"pmos", "posedge", "primitive", "pull0", "pull1",
		    "pulldown", "pullup", 0};
pChar V_List_R[] = {"rcmos", "release", "repeat",
		    "rnmos", "rpmos", "rtran", "rtranif0", "rtranif1", 0};
pChar V_List_S[] = {"scalared", "signed", "small", "specify", "strength",
		    "strong0", "strong1", 0};
pChar V_List_T[] = {"table", "task", "tran", "tranif0", "tranif1", 0};
pChar V_List_U[] = {"unsigned", 0};
pChar V_List_V[] = {"vectored", 0};
pChar V_List_W[] = {"wait", "weak0", "weak1", "while", 0};
pChar V_List_X[] = {"xnor", "xor", 0};

ppChar V_WordList[] =
  {(ppChar)&V_List_A, (ppChar)&V_List_B, (ppChar)&V_List_C, (ppChar)&V_List_D,
   (ppChar)&V_List_E, (ppChar)&V_List_F, 0,                 (ppChar)&V_List_H,
   (ppChar)&V_List_I, (ppChar)&V_List_J, 0,                 (ppChar)&V_List_L,
   (ppChar)&V_List_M, (ppChar)&V_List_N, (ppChar)&V_List_O, (ppChar)&V_List_P,
   0,                 (ppChar)&V_List_R, (ppChar)&V_List_S, (ppChar)&V_List_T,
   (ppChar)&V_List_U, (ppChar)&V_List_V, (ppChar)&V_List_W, (ppChar)&V_List_X,
   0,               0};

pChar V_List_Directives[] =
  {"reset_all", "timescale", "define", "include", "ifdef", "else", "endif",
   "celldefine", "endcelldefine", "default_nettype", "unconnected_drive",
   "nounconnected_drive", "delay_mode_zero", "delay_mode_unit",
   "delay_mode_path", "delay_mode_distributed", "uselib", 0};

pChar V_List_DataTypes[] = {
   "reg", "integer", "time", "real", "realtime", "wire", "tri", "wor",
   "trior", "wand", "triand", "tri0", "tri1", "supply0", "supply1", "trireg",
   "parameter", "specparam", "event", 0};

pChar V_List_Functions[] =
  {"setup", "hold", "setuphold", "skew", "recovery", "period", "width",
   "monitor", "display", "write", "strobe", "fopen", "fclose", "time",
   "stime", "realtime", "timeformat", "printtimescale", "random", "readmemb",
   "readmemh", "finish", "stop", 0};

// ---------------------------------------------------
// reserved Verilog-A words in alphabetical order
pChar VA_List_A[] = {"abstol", "access", "analog", "ac_stim", "analysis", 0};
pChar VA_List_B[] = {"begin", "branch", "bound_step", 0};
pChar VA_List_C[] = {"case", 0};
pChar VA_List_D[] = {"discipline", "ddt_nature", "ddt", "delay",
		     "discontinuity", "default", 0 };
pChar VA_List_E[] = {"enddiscipline", "else", "end", "endnature", "exclude",
		     "endfunction", "endmodule", "electrical", "endcase", 0};
pChar VA_List_F[] = {"for", "flow", "from", "final_step", "flicker_noise",
		     "function", 0};
pChar VA_List_G[] = {"generate", "ground", 0};
pChar VA_List_I[] = {"if", "idt_nature", "inf", "idt", "initial_step",
		     "input", "inout", 0};
pChar VA_List_L[] = {"laplace_nd", "laplace_np", "laplace_zd", "laplace_zp",
		     "last_crossing", 0};
pChar VA_List_M[] = {"module", 0};
pChar VA_List_N[] = {"nature", "noise_table", 0};
pChar VA_List_P[] = {"potential", "parameter", 0};
pChar VA_List_S[] = {"slew", 0};
pChar VA_List_T[] = {"timer", "transition", 0};
pChar VA_List_U[] = {"units", 0};
pChar VA_List_W[] = {"white_noise", "while", 0};
pChar VA_List_Z[] = {"zi_nd", "zi_np", "zi_zd", "zi_zp", 0};

ppChar VA_WordList[] =
  {(ppChar)&VA_List_A, (ppChar)&VA_List_B, (ppChar)&VA_List_C,
   (ppChar)&VA_List_D, (ppChar)&VA_List_E, (ppChar)&VA_List_F,
   (ppChar)&VA_List_G, 0,                  (ppChar)&VA_List_I,
   0,                  0,                  (ppChar)&VA_List_L,
   (ppChar)&VA_List_M, (ppChar)&VA_List_N, 0,
   (ppChar)&VA_List_P, 0,                  0,
   (ppChar)&VA_List_S, (ppChar)&VA_List_T, (ppChar)&VA_List_U,
   0,                  (ppChar)&VA_List_W, 0,
   0,                  (ppChar)&VA_List_Z };

pChar VA_List_Units[] =
  {"T", "G", "M", "K", "m", "u", "n", "p", "f", "a", 0};

pChar VA_List_Directives[] =
  {"define", "else", "undef", "ifdef", "endif", "include", "resetall", 0};

pChar VA_List_DataTypes[] = {
   "integer", "real", 0};

pChar VA_List_Functions[] =
  {"realtime", "temperature", "vt", "display", "strobe", 0};

// ---------------------------------------------------
// reserved Octave words in alphabetical order
pChar OCT_List_C[] = {"case", "catch", 0};
pChar OCT_List_E[] = {"else", "elseif", "end", "endfor", "endfunction",
		      "endif", "endswitch", "end_try_catch", "endwhile",
		      "end_unwind_protect", 0};
pChar OCT_List_F[] = {"for", "function", 0};
pChar OCT_List_I[] = {"if", 0};
pChar OCT_List_O[] = {"otherwise", 0};
pChar OCT_List_S[] = {"switch", 0};
pChar OCT_List_T[] = {"try", 0};
pChar OCT_List_U[] = {"unwind_protect", "unwind_protect_cleanup", 0};
pChar OCT_List_W[] = {"while", 0};

ppChar OCT_WordList[] =
  {0,                   0,                   (ppChar)&OCT_List_C,
   0,                   (ppChar)&OCT_List_E, (ppChar)&OCT_List_F,
   0,                   0,                   (ppChar)&OCT_List_I,
   0,                   0,                   0,
   0,                   0,                   (ppChar)&OCT_List_O,
   0,                   0,                   0,
   (ppChar)&OCT_List_S, (ppChar)&OCT_List_T, (ppChar)&OCT_List_U,
   0,                   (ppChar)&OCT_List_W, 0,
   0,                   0};

pChar OCT_List_DataTypes[] = {
   "inf", "nan", "pi", 0};

pChar OCT_List_Functions[] =
  {"plot", 0};

// ---------------------------------------------------
void SyntaxHighlighter::markWord(const QString& text, int start, int len)
{
  pChar *List;
  // apply font
  QFont newFont = Doc->TextFont;
  newFont.setPointSize((int)Doc->Scale);

  // get word
  QString Word = text.mid(start, len);

  // switch case sensitivity
  switch (language) {
  case LANG_VHDL:
    Word = Word.lower();
    break;
  default:
    break;
  }

  // get index into wordlist
  int idx = (int)(Word.at(0).latin1() - 'a');
  if(idx >= 0 && idx <= 25) {

    // switch wordlist
    switch (language) {
    case LANG_VHDL:
      List = VHD_WordList[idx];
      break;
    case LANG_VERILOG:
      List = V_WordList[idx];
      break;
    case LANG_OCTAVE:
      List = OCT_WordList[idx];
      break;
    default:
      List = VA_WordList[idx];;
      break;
    }

    // mark reserved words
    if(List)
      for( ; *List != 0; List++)
	if(Word == *List) {
	  newFont.setWeight(QFont::Bold);
	  setFormat(start, len, newFont);
	  return;
	}
  }

  // mark data types
  switch (language) {
  case LANG_VHDL:
    List = VHD_List_DataTypes;
    break;
  case LANG_VERILOG:
    List = V_List_DataTypes;
    break;
  case LANG_OCTAVE:
    List = OCT_List_DataTypes;
    break;
  default:
    List = VA_List_DataTypes;
    break;
  }
  if(List)
    for( ; *List != 0; List++)
      if(Word == *List) {
	setFormat(start, len, QucsSettings.Type);
	return;
      }

  // mark units
  switch (language) {
  case LANG_VHDL:
    List = VHD_List_Units;
    break;
  case LANG_VERILOGA:
    List = VA_List_Units;
    break;
  default:
    List = 0;
    break;
  }
  if(List)
    for( ; *List != 0; List++)
      if(Word == *List) {
	newFont.setWeight(QFont::Bold);
	setFormat(start, len, newFont, QucsSettings.Real);
	return;
      }


  if (Word.at(0) == '`' || Word.at(0) == '$') {
    Word = Word.mid(1);
    // mark directives
    switch (language) {
    case LANG_VERILOG:
      List = V_List_Directives;
      break;
    case LANG_VERILOGA:
      List = VA_List_Directives;
      break;
    default:
      List = 0;
      break;
    }
    if(List)
      for( ; *List != 0; List++)
	if(Word == *List) {
	  newFont.setWeight(QFont::Bold);
	  setFormat(start, len, newFont, QucsSettings.Directive);
	  return;
	}

    // mark special functions
    switch (language) {
    case LANG_VERILOG:
      List = V_List_Functions;
      break;
    case LANG_VERILOGA:
      List = VA_List_Functions;
      break;
    case LANG_OCTAVE:
      List = OCT_List_Functions;
      break;
    default:
      List = 0;
      break;
    }
    if(List)
      for( ; *List != 0; List++)
	if(Word == *List) {
	  newFont.setWeight(QFont::Bold);
	  setFormat(start, len, newFont, QucsSettings.Task);
	  return;
	}
  }
}

// ---------------------------------------------------
// reserved VHDL attributes in alphabetical order
pChar VHD_List_Attrib_A[] = {"active", "ascending", 0};
pChar VHD_List_Attrib_B[] = {"base", 0};
pChar VHD_List_Attrib_D[] = {"delayed", 0};
pChar VHD_List_Attrib_E[] = {"event", 0};
pChar VHD_List_Attrib_H[] = {"high", 0};
pChar VHD_List_Attrib_I[] = {"image", 0};
pChar VHD_List_Attrib_L[] = {"last_active", "last_event", "last_value", "left",
			     "leftof", "length", "low", 0};
pChar VHD_List_Attrib_P[] = {"pos", "pred", 0};
pChar VHD_List_Attrib_Q[] = {"quiet", 0};
pChar VHD_List_Attrib_R[] = {"range", "reverse_range", "right", "rightof", 0};
pChar VHD_List_Attrib_S[] = {"stable", "succ", 0};
pChar VHD_List_Attrib_T[] = {"transaction", 0};
pChar VHD_List_Attrib_V[] = {"val", "value", 0};

ppChar VHD_Attribute_List[] =
  {(ppChar)&VHD_List_Attrib_A, (ppChar)&VHD_List_Attrib_B, 0,
   (ppChar)&VHD_List_Attrib_D, (ppChar)&VHD_List_Attrib_E, 0,
   0, (ppChar)&VHD_List_Attrib_H, (ppChar)&VHD_List_Attrib_I,
   0, 0, (ppChar)&VHD_List_Attrib_L, 0, 0, 0,
   (ppChar)&VHD_List_Attrib_P, (ppChar)&VHD_List_Attrib_Q,
   (ppChar)&VHD_List_Attrib_R, (ppChar)&VHD_List_Attrib_S,
   (ppChar)&VHD_List_Attrib_T, 0, (ppChar)&VHD_List_Attrib_V};

void SyntaxHighlighter::markAttribute(const QString& text, int start, int len)
{
  QString Word = text.mid(start, len).lower();
  int idx = (int)(Word.at(0).latin1() - 'a');
  if(idx < 0 || idx > 22)
    return;
  pChar *List = VHD_Attribute_List[idx];

  if(List)
    for(; *List != 0; List++)
      if(Word == *List) {
        setFormat(start-1, len+1, QucsSettings.Attribute);
        return;
      }
}
