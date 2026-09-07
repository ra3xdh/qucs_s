/*
 * test_substratelib.cpp - frameworkless test for the substrate library reader
 *
 * Copyright (C) 2026 Qucs-S team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 */

#include "substratelib.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

#include <cmath>
#include <cstdio>

static int failures = 0;

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);     \
      ++failures;                                                              \
    }                                                                          \
  } while (0)

static bool nearly(double a, double b) {
  return std::fabs(a - b) <= 1e-9 * std::fmax(1.0, std::fabs(b));
}

static QString writeFile(const QDir& dir, const QString& name,
                         const QString& content) {
  const QString path = dir.filePath(name);
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return QString();
  }
  QTextStream ts(&f);
  ts << content;
  f.close();
  return path;
}

int main() {
  QTemporaryDir tmp;
  if (!tmp.isValid()) {
    std::fprintf(stderr, "cannot create temporary directory\n");
    return 2;
  }
  const QDir dir(tmp.path());

  const QString lib =
      "<Qucs Library 0.0.10 \"Substrates\">\n\n"
      "<Component RO4003>\n"
      "  <Description>\n"
      "    Hydrocarbon Ceramic Woven Glass\n"
      "  </Description>\n"
      "  <Model>\n"
      "    <SUBST RO4003_ 1 0 0 -30 24 0 0 \"3.38\" 1 \"0.8 mm\" 1 \"35 um\" 1 "
      "\"0.0027\" 1 \"0.022e-6\" 1 \"1.4e-6\" 1>\n"
      "  </Model>\n"
      "</Component>\n\n"
      "<Component RT/duroid 5880>\n"
      "  <Model>\n"
      "    <SUBST RT5880_ 1 0 0 -30 24 0 0 \"2.20\" 1 \"1.5 mm\" 1 \"17 um\" 1 "
      "\"0.0009\" 1 \"0\" 1 \"0.5e-6\" 1>\n"
      "  </Model>\n"
      "</Component>\n\n"
      "<Component NoSubstHere>\n"
      "  <Model>\n"
      "  </Model>\n"
      "</Component>\n\n"
      "<Component BadUnit>\n"
      "  <Model>\n"
      "    <SUBST BAD_ 1 0 0 -30 24 0 0 \"4.0\" 1 \"1.0 nm\" 1 \"35 um\" 1 "
      "\"0.02\" 1 \"1e-7\" 1 \"1e-6\" 1>\n"
      "  </Model>\n"
      "</Component>\n\n"
      "<Component BadNumber>\n"
      "  <Model>\n"
      "    <SUBST BAD2_ 1 0 0 -30 24 0 0 \"abc\" 1 \"0.8 mm\" 1 \"35 um\" 1 "
      "\"0.02\" 1 \"1e-7\" 1 \"1e-6\" 1>\n"
      "  </Model>\n"
      "</Component>\n";

  const QString path = writeFile(dir, "Substrates.lib", lib);
  CHECK(!path.isEmpty());

  const QList<SubstrateMaterial> mats = readSubstrateLibrary(path);
  // Only RO4003 and "RT/duroid 5880" are usable.
  CHECK(mats.count() == 2);

  if (mats.count() >= 1) {
    const SubstrateMaterial& m = mats.at(0);
    CHECK(m.name == "RO4003");
    CHECK(nearly(m.er, 3.38));
    CHECK(nearly(m.h, 0.8));
    CHECK(m.hUnit == "mm");
    CHECK(nearly(m.t, 35.0));
    CHECK(m.tUnit == "um");
    CHECK(nearly(m.tand, 0.0027));
    CHECK(nearly(m.rho, 0.022e-6));
    CHECK(nearly(m.D, 1.4e-6));
  }

  if (mats.count() >= 2) {
    const SubstrateMaterial& m = mats.at(1);
    CHECK(m.name == "RT/duroid 5880"); // spaces and '/' preserved
    CHECK(nearly(m.rho, 0.0));         // rho == 0 parses fine, no division
    CHECK(m.hUnit == "mm");
    CHECK(nearly(m.D, 0.5e-6));
  }

  // A file that cannot be opened yields an empty list.
  CHECK(readSubstrateLibrary(dir.filePath("missing.lib")).isEmpty());

  // Direct checks of the value/unit splitter.
  double v = 0.0;
  QString u;
  CHECK(parseLengthWithUnit("0.8 mm", v, u) && nearly(v, 0.8) && u == "mm");
  CHECK(parseLengthWithUnit("35 um", v, u) && nearly(v, 35.0) && u == "um");
  CHECK(parseLengthWithUnit("1.5e-3 m", v, u) && nearly(v, 1.5e-3) && u == "m");
  CHECK(!parseLengthWithUnit("1.0 nm", v, u)); // unsupported unit
  CHECK(!parseLengthWithUnit("abc mm", v, u)); // not numeric
  CHECK(!parseLengthWithUnit("5", v, u));      // no unit token

  if (failures) {
    std::fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  std::printf("all substratelib checks passed\n");
  return 0;
}
