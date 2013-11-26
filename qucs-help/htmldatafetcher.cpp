/***************************************************************************
 * Copyright (C) 2006 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#include "htmldatafetcher.h"
#include <QMimeData>
#include <q3dragobject.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <Q3MimeSourceFactory>

HtmlDataFetcher::HtmlDataFetcher()
{
  initMap();
}

void HtmlDataFetcher::initMap()
{
  entityMap.insert(QString("AElig"),QChar(0x00c6));
  entityMap.insert(QString("Aacute"), QChar(0x00c1));
  entityMap.insert(QString("Acirc"), QChar(0x00c2));
  entityMap.insert(QString("Agrave"), QChar(0x00c0));
  entityMap.insert(QString("Alpha"), QChar(0x0391));
  entityMap.insert(QString("AMP"), QChar(38));
  entityMap.insert(QString("Aring"), QChar(0x00c5));
  entityMap.insert(QString("Atilde"), QChar(0x00c3));
  entityMap.insert(QString("Auml"), QChar(0x00c4));
  entityMap.insert(QString("Beta"), QChar(0x0392));
  entityMap.insert(QString("Ccedil"), QChar(0x00c7));
  entityMap.insert(QString("Chi"), QChar(0x03a7));
  entityMap.insert(QString("Dagger"), QChar(0x2021));
  entityMap.insert(QString("Delta"), QChar(0x0394));
  entityMap.insert(QString("ETH"), QChar(0x00d0));
  entityMap.insert(QString("Eacute"), QChar(0x00c9));
  entityMap.insert(QString("Ecirc"), QChar(0x00ca));
  entityMap.insert(QString("Egrave"), QChar(0x00c8));
  entityMap.insert(QString("Epsilon"), QChar(0x0395));
  entityMap.insert(QString("Eta"), QChar(0x0397));
  entityMap.insert(QString("Euml"), QChar(0x00cb));
  entityMap.insert(QString("Gamma"), QChar(0x0393));
  entityMap.insert(QString("GT"), QChar(62));
  entityMap.insert(QString("Iacute"), QChar(0x00cd));
  entityMap.insert(QString("Icirc"), QChar(0x00ce));
  entityMap.insert(QString("Igrave"), QChar(0x00cc));
  entityMap.insert(QString("Iota"), QChar(0x0399));
  entityMap.insert(QString("Iuml"), QChar(0x00cf));
  entityMap.insert(QString("Kappa"), QChar(0x039a));
  entityMap.insert(QString("Lambda"), QChar(0x039b));
  entityMap.insert(QString("LT"), QChar(60));
  entityMap.insert(QString("Mu"), QChar(0x039c));
  entityMap.insert(QString("Ntilde"), QChar(0x00d1));
  entityMap.insert(QString("Nu"), QChar(0x039d));
  entityMap.insert(QString("OElig"), QChar(0x0152));
  entityMap.insert(QString("Oacute"), QChar(0x00d3));
  entityMap.insert(QString("Ocirc"), QChar(0x00d4));
  entityMap.insert(QString("Ograve"), QChar(0x00d2));
  entityMap.insert(QString("Omega"), QChar(0x03a9));
  entityMap.insert(QString("Omicron"), QChar(0x039f));
  entityMap.insert(QString("Oslash"), QChar(0x00d8));
  entityMap.insert(QString("Otilde"), QChar(0x00d5));
  entityMap.insert(QString("Ouml"), QChar(0x00d6));
  entityMap.insert(QString("Phi"), QChar(0x03a6));
  entityMap.insert(QString("Pi"), QChar(0x03a0));
  entityMap.insert(QString("Prime"), QChar(0x2033));
  entityMap.insert(QString("Psi"), QChar(0x03a8));
  entityMap.insert(QString("QUOT"), QChar(34));
  entityMap.insert(QString("Rho"), QChar(0x03a1));
  entityMap.insert(QString("Scaron"), QChar(0x0160));
  entityMap.insert(QString("Sigma"), QChar(0x03a3));
  entityMap.insert(QString("THORN"), QChar(0x00de));
  entityMap.insert(QString("Tau"), QChar(0x03a4));
  entityMap.insert(QString("Theta"), QChar(0x0398));
  entityMap.insert(QString("Uacute"), QChar(0x00da));
  entityMap.insert(QString("Ucirc"), QChar(0x00db));
  entityMap.insert(QString("Ugrave"), QChar(0x00d9));
  entityMap.insert(QString("Upsilon"), QChar(0x03a5));
  entityMap.insert(QString("Uuml"), QChar(0x00dc));
  entityMap.insert(QString("Xi"), QChar(0x039e));
  entityMap.insert(QString("Yacute"), QChar(0x00dd));
  entityMap.insert(QString("Yuml"), QChar(0x0178));
  entityMap.insert(QString("Zeta"), QChar(0x0396));
  entityMap.insert(QString("aacute"), QChar(0x00e1));
  entityMap.insert(QString("acirc"), QChar(0x00e2));
  entityMap.insert(QString("acute"), QChar(0x00b4));
  entityMap.insert(QString("aelig"), QChar(0x00e6));
  entityMap.insert(QString("agrave"), QChar(0x00e0));
  entityMap.insert(QString("alefsym"), QChar(0x2135));
  entityMap.insert(QString("alpha"), QChar(0x03b1));
  entityMap.insert(QString("amp"), QChar(38));
  entityMap.insert(QString("and"), QChar(0x22a5));
  entityMap.insert(QString("ang"), QChar(0x2220));
  entityMap.insert(QString("apos"), QChar(0x0027));
  entityMap.insert(QString("aring"), QChar(0x00e5));
  entityMap.insert(QString("asymp"), QChar(0x2248));
  entityMap.insert(QString("atilde"), QChar(0x00e3));
  entityMap.insert(QString("auml"), QChar(0x00e4));
  entityMap.insert(QString("bdquo"), QChar(0x201e));
  entityMap.insert(QString("beta"), QChar(0x03b2));
  entityMap.insert(QString("brvbar"), QChar(0x00a6));
  entityMap.insert(QString("bull"), QChar(0x2022));
  entityMap.insert(QString("cap"), QChar(0x2229));
  entityMap.insert(QString("ccedil"), QChar(0x00e7));
  entityMap.insert(QString("cedil"), QChar(0x00b8));
  entityMap.insert(QString("cent"), QChar(0x00a2));
  entityMap.insert(QString("chi"), QChar(0x03c7));
  entityMap.insert(QString("circ"), QChar(0x02c6));
  entityMap.insert(QString("clubs"), QChar(0x2663));
  entityMap.insert(QString("cong"), QChar(0x2245));
  entityMap.insert(QString("copy"), QChar(0x00a9));
  entityMap.insert(QString("crarr"), QChar(0x21b5));
  entityMap.insert(QString("cup"), QChar(0x222a));
  entityMap.insert(QString("curren"), QChar(0x00a4));
  entityMap.insert(QString("dArr"), QChar(0x21d3));
  entityMap.insert(QString("dagger"), QChar(0x2020));
  entityMap.insert(QString("darr"), QChar(0x2193));
  entityMap.insert(QString("deg"), QChar(0x00b0));
  entityMap.insert(QString("delta"), QChar(0x03b4));
  entityMap.insert(QString("diams"), QChar(0x2666));
  entityMap.insert(QString("divide"), QChar(0x00f7));
  entityMap.insert(QString("eacute"), QChar(0x00e9));
  entityMap.insert(QString("ecirc"), QChar(0x00ea));
  entityMap.insert(QString("egrave"), QChar(0x00e8));
  entityMap.insert(QString("empty"), QChar(0x2205));
  entityMap.insert(QString("emsp"), QChar(0x2003));
  entityMap.insert(QString("ensp"), QChar(0x2002));
  entityMap.insert(QString("epsilon"), QChar(0x03b5));
  entityMap.insert(QString("equiv"), QChar(0x2261));
  entityMap.insert(QString("eta"), QChar(0x03b7));
  entityMap.insert(QString("eth"), QChar(0x00f0));
  entityMap.insert(QString("euml"), QChar(0x00eb));
  entityMap.insert(QString("euro"), QChar(0x20ac));
  entityMap.insert(QString("exist"), QChar(0x2203));
  entityMap.insert(QString("fnof"), QChar(0x0192));
  entityMap.insert(QString("forall"), QChar(0x2200));
  entityMap.insert(QString("frac12"), QChar(0x00bd));
  entityMap.insert(QString("frac14"), QChar(0x00bc));
  entityMap.insert(QString("frac34"), QChar(0x00be));
  entityMap.insert(QString("frasl"), QChar(0x2044));
  entityMap.insert(QString("gamma"), QChar(0x03b3));
  entityMap.insert(QString("ge"), QChar(0x2265));
  entityMap.insert(QString("gt"), QChar(62));
  entityMap.insert(QString("hArr"), QChar(0x21d4));
  entityMap.insert(QString("harr"), QChar(0x2194));
  entityMap.insert(QString("hearts"), QChar(0x2665));
  entityMap.insert(QString("hellip"), QChar(0x2026));
  entityMap.insert(QString("iacute"), QChar(0x00ed));
  entityMap.insert(QString("icirc"), QChar(0x00ee));
  entityMap.insert(QString("iexcl"), QChar(0x00a1));
  entityMap.insert(QString("igrave"), QChar(0x00ec));
  entityMap.insert(QString("image"), QChar(0x2111));
  entityMap.insert(QString("infin"), QChar(0x221e));
  entityMap.insert(QString("int"), QChar(0x222b));
  entityMap.insert(QString("iota"), QChar(0x03b9));
  entityMap.insert(QString("iquest"), QChar(0x00bf));
  entityMap.insert(QString("isin"), QChar(0x2208));
  entityMap.insert(QString("iuml"), QChar(0x00ef));
  entityMap.insert(QString("kappa"), QChar(0x03ba));
  entityMap.insert(QString("lArr"), QChar(0x21d0));
  entityMap.insert(QString("lambda"), QChar(0x03bb));
  entityMap.insert(QString("lang"), QChar(0x2329));
  entityMap.insert(QString("laquo"), QChar(0x00ab));
  entityMap.insert(QString("larr"), QChar(0x2190));
  entityMap.insert(QString("lceil"), QChar(0x2308));
  entityMap.insert(QString("ldquo"), QChar(0x201c));
  entityMap.insert(QString("le"), QChar(0x2264));
  entityMap.insert(QString("lfloor"), QChar(0x230a));
  entityMap.insert(QString("lowast"), QChar(0x2217));
  entityMap.insert(QString("loz"), QChar(0x25ca));
  entityMap.insert(QString("lrm"), QChar(0x200e));
  entityMap.insert(QString("lsaquo"), QChar(0x2039));
  entityMap.insert(QString("lsquo"), QChar(0x2018));
  entityMap.insert(QString("lt"), QChar(60));
  entityMap.insert(QString("macr"), QChar(0x00af));
  entityMap.insert(QString("mdash"), QChar(0x2014));
  entityMap.insert(QString("micro"), QChar(0x00b5));
  entityMap.insert(QString("middot"), QChar(0x00b7));
  entityMap.insert(QString("minus"), QChar(0x2212));
  entityMap.insert(QString("mu"), QChar(0x03bc));
  entityMap.insert(QString("nabla"), QChar(0x2207));
  entityMap.insert(QString("nbsp"), QChar(0x00a0));
  entityMap.insert(QString("ndash"), QChar(0x2013));
  entityMap.insert(QString("ne"), QChar(0x2260));
  entityMap.insert(QString("ni"), QChar(0x220b));
  entityMap.insert(QString("not"), QChar(0x00ac));
  entityMap.insert(QString("notin"), QChar(0x2209));
  entityMap.insert(QString("nsub"), QChar(0x2284));
  entityMap.insert(QString("ntilde"), QChar(0x00f1));
  entityMap.insert(QString("nu"), QChar(0x03bd));
  entityMap.insert(QString("oacute"), QChar(0x00f3));
  entityMap.insert(QString("ocirc"), QChar(0x00f4));
  entityMap.insert(QString("oelig"), QChar(0x0153));
  entityMap.insert(QString("ograve"), QChar(0x00f2));
  entityMap.insert(QString("oline"), QChar(0x203e));
  entityMap.insert(QString("omega"), QChar(0x03c9));
  entityMap.insert(QString("omicron"), QChar(0x03bf));
  entityMap.insert(QString("oplus"), QChar(0x2295));
  entityMap.insert(QString("or"), QChar(0x22a6));
  entityMap.insert(QString("ordf"), QChar(0x00aa));
  entityMap.insert(QString("ordm"), QChar(0x00ba));
  entityMap.insert(QString("oslash"), QChar(0x00f8));
  entityMap.insert(QString("otilde"), QChar(0x00f5));
  entityMap.insert(QString("otimes"), QChar(0x2297));
  entityMap.insert(QString("ouml"), QChar(0x00f6));
  entityMap.insert(QString("para"), QChar(0x00b6));
  entityMap.insert(QString("part"), QChar(0x2202));
  entityMap.insert(QString("percnt"), QChar(0x0025));
  entityMap.insert(QString("permil"), QChar(0x2030));
  entityMap.insert(QString("perp"), QChar(0x22a5));
  entityMap.insert(QString("phi"), QChar(0x03c6));
  entityMap.insert(QString("pi"), QChar(0x03c0));
  entityMap.insert(QString("piv"), QChar(0x03d6));
  entityMap.insert(QString("plusmn"), QChar(0x00b1));
  entityMap.insert(QString("pound"), QChar(0x00a3));
  entityMap.insert(QString("prime"), QChar(0x2032));
  entityMap.insert(QString("prod"), QChar(0x220f));
  entityMap.insert(QString("prop"), QChar(0x221d));
  entityMap.insert(QString("psi"), QChar(0x03c8));
  entityMap.insert(QString("quot"), QChar(34));
  entityMap.insert(QString("rArr"), QChar(0x21d2));
  entityMap.insert(QString("radic"), QChar(0x221a));
  entityMap.insert(QString("rang"), QChar(0x232a));
  entityMap.insert(QString("raquo"), QChar(0x00bb));
  entityMap.insert(QString("rarr"), QChar(0x2192));
  entityMap.insert(QString("rceil"), QChar(0x2309));
  entityMap.insert(QString("rdquo"), QChar(0x201d));
  entityMap.insert(QString("real"), QChar(0x211c));
  entityMap.insert(QString("reg"), QChar(0x00ae));
  entityMap.insert(QString("rfloor"), QChar(0x230b));
  entityMap.insert(QString("rho"), QChar(0x03c1));
  entityMap.insert(QString("rlm"), QChar(0x200f));
  entityMap.insert(QString("rsaquo"), QChar(0x203a));
  entityMap.insert(QString("rsquo"), QChar(0x2019));
  entityMap.insert(QString("sbquo"), QChar(0x201a));
  entityMap.insert(QString("scaron"), QChar(0x0161));
  entityMap.insert(QString("sdot"), QChar(0x22c5));
  entityMap.insert(QString("sect"), QChar(0x00a7));
  entityMap.insert(QString("shy"), QChar(0x00ad));
  entityMap.insert(QString("sigma"), QChar(0x03c3));
  entityMap.insert(QString("sigmaf"), QChar(0x03c2));
  entityMap.insert(QString("sim"), QChar(0x223c));
  entityMap.insert(QString("spades"), QChar(0x2660));
  entityMap.insert(QString("sub"), QChar(0x2282));
  entityMap.insert(QString("sube"), QChar(0x2286));
  entityMap.insert(QString("sum"), QChar(0x2211));
  entityMap.insert(QString("sup1"), QChar(0x00b9));
  entityMap.insert(QString("sup2"), QChar(0x00b2));
  entityMap.insert(QString("sup3"), QChar(0x00b3));
  entityMap.insert(QString("sup"), QChar(0x2283));
  entityMap.insert(QString("supe"), QChar(0x2287));
  entityMap.insert(QString("szlig"), QChar(0x00df));
  entityMap.insert(QString("tau"), QChar(0x03c4));
  entityMap.insert(QString("there4"), QChar(0x2234));
  entityMap.insert(QString("theta"), QChar(0x03b8));
  entityMap.insert(QString("thetasym"), QChar(0x03d1));
  entityMap.insert(QString("thinsp"), QChar(0x2009));
  entityMap.insert(QString("thorn"), QChar(0x00fe));
  entityMap.insert(QString("tilde"), QChar(0x02dc));
  entityMap.insert(QString("times"), QChar(0x00d7));
  entityMap.insert(QString("trade"), QChar(0x2122));
  entityMap.insert(QString("uArr"), QChar(0x21d1));
  entityMap.insert(QString("uacute"), QChar(0x00fa));
  entityMap.insert(QString("uarr"), QChar(0x2191));
  entityMap.insert(QString("ucirc"), QChar(0x00fb));
  entityMap.insert(QString("ugrave"), QChar(0x00f9));
  entityMap.insert(QString("uml"), QChar(0x00a8));
  entityMap.insert(QString("upsih"), QChar(0x03d2));
  entityMap.insert(QString("upsilon"), QChar(0x03c5));
  entityMap.insert(QString("uuml"), QChar(0x00fc));
  entityMap.insert(QString("weierp"), QChar(0x2118));
  entityMap.insert(QString("xi"), QChar(0x03be));
  entityMap.insert(QString("yacute"), QChar(0x00fd));
  entityMap.insert(QString("yen"), QChar(0x00a5));
  entityMap.insert(QString("yuml"), QChar(0x00ff));
  entityMap.insert(QString("zeta"), QChar(0x03b6));
  entityMap.insert(QString("zwj"), QChar(0x200d));
  entityMap.insert(QString("zwnj"), QChar(0x200c));
}

//Eg &#1055;
QChar HtmlDataFetcher::unicodeFor(QString placeHolder)
{
  placeHolder.remove(' ');
  placeHolder.remove('\t');
  Q_ASSERT(placeHolder[0] == '&');
  Q_ASSERT(placeHolder[(int)placeHolder.length() - 1] == ';');
  placeHolder.remove(placeHolder.length() - 1,1);//remove ';'
  placeHolder.remove(0,1);// remove '&'
  if(placeHolder[0] == '#')
  {
    placeHolder.remove(0,1);//remove '#'
    return QChar(placeHolder.toInt());
  }
  QMap<QString, QChar>::Iterator it = entityMap.find(placeHolder);
  if ( it != entityMap.end() )
    return *it;
  qWarning("HtmlDataFetcher::unicodeFor() :  Unknown html entity or html special character");
  return '*';
}

QStringList HtmlDataFetcher::fetchChapterTexts(const QString &indexFile)
{
  QStringList retVal;

  QFile file(indexFile);
  if(!file.open(QIODevice::ReadOnly))
  {
    qWarning("HtmlDataFetcher::fetchChapterTexts() : Can't open file %s",indexFile.latin1());
    return retVal;
  }

  const QMimeSource *source = Q3MimeSourceFactory::defaultFactory()->data(indexFile);
  Q_ASSERT(source);
  QString sourceText;
  bool status = Q3TextDrag::decode(source,sourceText);
  Q_ASSERT(status);

  Q3TextStream str(&sourceText,QIODevice::ReadOnly);
  QString txt;
  bool inText = false;//spans multiple lines
  while ( !str.atEnd() )
  {
    QString line = str.readLine();
    if(inText == false)
    {
      txt = "";
      int index = line.find("href=\"");
      if(index == -1  || line.contains("http") || line.contains("mailto"))
        continue;
      index += 6;
      index = line.find('>',index);
      if(index == -1)
      {
        qWarning("HtmlDataFetcher::fetchChapterTexts() : Parse error");
        return retVal;
      }
      ++index;
      int end = line.find("</a>",index);
      if(end != -1)
      {
        txt = line.mid(index,end-index);
        formatAndReplace(txt);
        retVal << txt;
      }
      else
      {
        txt += line.mid(index);
		txt += ' ';
        inText = true;
      }
    }
    else
    {
      int end = line.find("</a>");
      if(end == -1)
        txt += line + ' ';
      else
      {
        txt += line.left(end);
        inText = false;
        formatAndReplace(txt);
        retVal << txt;
      }
    }
  }
  return retVal;
}

QStringList HtmlDataFetcher::fetchLinksToFiles(const QString &indexFile)
{
  QStringList retVal;
  retVal << QString("index.html");
  QFile file(indexFile);
  if(!file.open(QIODevice::ReadOnly))
  {
    qWarning("HtmlDataFetcher::fetchLinksToFiles() : File open error");
    return QStringList();
  }
  Q3TextStream str(&file);
  QString line,link;
  int index = -1;
  int end = -1;
  while ( !str.atEnd() )
  {
    line = str.readLine();
    if(line.contains("http") || line.contains("mailto"))
      continue;
    index = line.find("href=\"");//find link to other file
    if(index != -1)
    {
      index += 6;
      end = line.find('"',index);
      if(end == -1)
      {
        qWarning("HtmlDataFetcher::fetchLinksToFiles() : Can't find end quote. May be HTML error");
        return QStringList();
      }
      link = line.mid(index,end-index);
      retVal << link;
    }
  }
  return retVal;
}

void HtmlDataFetcher::formatAndReplace(QString &txt)
{
  if(!txt.contains('&'))
    return;
  int st,end;
  
  while(1)
  {
    st = txt.find('&');
    if(st == -1)
      return;
    end = txt.find(';',st);
    if(end == -1)
    {
      qWarning("HtmlDataFetcher::formatAndReplace() : Can't find ';'");
      return;
    }
    QChar ch = unicodeFor(txt.mid(st,end-st+1));
    txt.replace(st,end-st+1,ch);
  }
}
