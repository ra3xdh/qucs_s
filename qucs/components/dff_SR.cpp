/***************************************************************************
                              dff_SR
                             --------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * dff_SR.cpp - device implementations for dff_SR module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 */
#include "dff_SR.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"

dff_SR::dff_SR()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("D flip flop with set and reset verilog device");

  Props.emplace_back ( Property ("TR_H", "6", false,
    QObject::tr ("cross coupled gate transfer function high scaling factor")));
  Props.emplace_back ( Property ("TR_L", "5", false,
    QObject::tr ("cross coupled gate transfer function low scaling factor")));
  Props.emplace_back ( Property ("Delay", "1 ns", false,
    QObject::tr ("cross coupled gate delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 4;
  ty = y2 + 4;
  Model = "dff_SR";
  Name  = "Y";
  SpiceModel = "A";
}

Component * dff_SR::newOne()
{
  dff_SR * p = new dff_SR();
  p->Props.front().Value = Props.front().Value;
  p->recreate(0);
  return p;
}

Element * dff_SR::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("D-FlipFlop w/ SR");
  BitmapFile = (char *) "dff_SR";

  if(getNewOne) return new dff_SR();
  return 0;
}

void dff_SR::createSymbol()
{
  // put in here symbol drawing code and terminal definitions
  Rects.emplace_back( qucs::Rect(-30, -40, 60, 80, QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));

  Lines.emplace_back( qucs::Line(-50,-20,-30,-20,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(-50, 20,-30, 20,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line( 30, 20, 50, 20,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line( 30,-20, 50,-20,QPen(Qt::darkBlue,2)));

  Polylines.emplace_back( qucs::Polyline(
    std::vector<QPointF>{{-30, 10}, {-20, 20}, {-30, 30}}, QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin)
  ));

  Lines.emplace_back( qucs::Line(  0, -50,  0, -60,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(  0,  50,  0,  60,QPen(Qt::darkBlue,2)));

  Ellipses.emplace_back( qucs::Ellips( -5,-50, 10, 10, QPen(Qt::darkBlue,2)));
  Ellipses.emplace_back( qucs::Ellips( -5, 40, 10, 10, QPen(Qt::darkBlue,2)));

  Texts.emplace_back( Text(-25,-28,  "D", Qt::darkBlue, 12.0));
  Texts.emplace_back( Text( 15.5,-28,  "Q", Qt::darkBlue, 12.0));
  Texts.emplace_back( Text( -4,-39,  "S", Qt::darkBlue, 12.0));
  Texts.emplace_back( Text( 15.5,  12,  "Q", Qt::darkBlue, 12.0));
  Texts.back().over=true;
  Texts.emplace_back( Text( -4, 24,  "R", Qt::darkBlue, 12.0));

  Ports.emplace_back( Port(0,  -60));  // S
  Ports.emplace_back( Port(-50,-20));  // D
  Ports.emplace_back( Port(-50, 20));  // CLK
  Ports.emplace_back( Port(  0, 60));  // R
  Ports.emplace_back( Port( 50, 20));  // QB
  Ports.emplace_back( Port( 50,-20));  // Q

  x1 = -50; y1 = -60;
  x2 =  50; y2 =  60;
}

QString dff_SR::vhdlCode( int )
{
  QString s="";
  QString td = prop(2).Value;
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString S     = port(0).getConnection()->Name;
  QString D     = port(1).getConnection()->Name;
  QString CLK   = port(2).getConnection()->Name;
  QString R     = port(3).getConnection()->Name;
  QString QB    = port(4).getConnection()->Name;
  QString Q     = port(5).getConnection()->Name;

  s = "\n  "+Name+":process ("+S+", "+CLK+", "+R+") is\n"+
      "  variable state : std_logic;\n"+
      "  begin\n" +
      "    if ("+S+" = '0') then\n"+
      "      state := '1';\n"+
      "    elsif ("+R+" = '0') then\n"+
      "      state := '0';\n"+
      "    elsif ("+CLK+" = '1' and "+CLK+"'event) then\n"+
      "      state := "+D+";\n"+
      "    end if;\n"+
      "    "+Q+" <= state"+td+
      "    "+QB+" <= not state"+td+
      "  end process;\n";
  return s;
}

QString dff_SR::verilogCode( int )
{

  QString td = prop(2).Value;
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString l = "";

  QString const S     = port(0).getConnection()->Name;
  QString D     = port(1).getConnection()->Name;
  QString CLK   = port(2).getConnection()->Name;
  QString R     = port(3).getConnection()->Name;
  QString QB    = port(4).getConnection()->Name;
  QString Q     = port(5).getConnection()->Name;

  QString QR   = "Q_reg"  + Name + Q;
  QString QBR  = "QB_reg"  + Name + QB;
  QString ST   = "Q_state" + Name;

  l = "\n  // "+Name+" d flip-flop with set and reset\n"+
      "  assign  "+Q+" = "+QR+";\n"+
      "  reg     "+QR+" = 0;\n"+
      "  assign  "+QB+" = "+QBR+";\n"+
      "  reg     "+QBR+" = 1;\n"+
      "  reg     "+ST+" = 0;\n"+
      "  always @ (posedge "+CLK+")\n"+
      "  begin\n"+
      "    if ("+R+" == 1 && "+S+" == 1)\n"+
      "    begin\n"+
      "      "+ST+" = "+D+";\n"+
      "      "+QR+" <="+td+" "+ST+";\n"+
      "      "+QBR+" <="+td+" ~"+ST+";\n"+
      "    end\n"+
      "  end\n"+
      "  always @ ("+R+")\n"+
      "  begin\n"+
      "    if ("+R+" == 0) "+ST+" = 0;\n"+
      "    "+QR+" <="+td+" "+ST+";\n"+
      "    "+QBR+" <="+td+" ~"+ST+";\n"+
      "  end\n"+
      "  always @ ("+S+")\n"+
      "  begin if ("+S+" == 0) "+ST+" = 1;\n"+
      "    "+QR+" <="+td+" "+ST+";\n"+
      "    "+QBR+" <="+td+" ~"+ST+";\n"+
      "  end\n";
  return l;
}

QString dff_SR::spice_netlist(bool isXyce)
{
    if (isXyce) return QString("");

    QString s = SpiceModel + Name;
    QString tmp_model = "model_" + Name;
    QString td = spicecompat::normalize_value(getProperty("Delay")->Value);

    QString SET   = spicecompat::normalize_node_name(port(0).getConnection()->Name);
    QString D     = spicecompat::normalize_node_name(port(1).getConnection()->Name);
    QString CLK   = spicecompat::normalize_node_name(port(2).getConnection()->Name);
    QString RESET = spicecompat::normalize_node_name(port(3).getConnection()->Name);
    QString QB    = spicecompat::normalize_node_name(port(4).getConnection()->Name);
    QString Q     = spicecompat::normalize_node_name(port(5).getConnection()->Name);

    s += " " + D + " " + CLK + " " + SET + " " + RESET + " " + Q + " " + QB;

    s += " " + tmp_model + "\n";
    s += QString(".model %1 d_dff(clk_delay=%2 set_delay=%2 reset_delay=%2 rise_delay=%2 fall_delay=%2)\n")
            .arg(tmp_model).arg(td);
    return s;
}
