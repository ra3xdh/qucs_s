#ifndef INCL_SCRIPT_DIRECTIVE_REWRITE_H
#define INCL_SCRIPT_DIRECTIVE_REWRITE_H

#include <QFileInfo>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <functional>

namespace qucs_s {

inline QString
rewriteIncludeLine(const QString& line,
                   const std::function<QString(const QString&)>& resolveFile,
                   bool quoteResolved) {
  static const QRegularExpression prefixRe(
      QStringLiteral("^(\\s*\\.include)(\\s+)"),
      QRegularExpression::CaseInsensitiveOption);

  const QRegularExpressionMatch m = prefixRe.match(line);
  if (!m.hasMatch()) {
    return line;
  }

  const int afterPrefix = m.capturedEnd(0);
  const QString prefix  = line.left(afterPrefix);

  QString token;
  QString suffix;

  if (afterPrefix < line.size() && line.at(afterPrefix) == QLatin1Char('"')) {
    const int closeIdx = line.indexOf(QLatin1Char('"'), afterPrefix + 1);
    if (closeIdx < 0) {
      return line; // unterminated quote: malformed, leave unchanged
    }
    token  = line.mid(afterPrefix + 1, closeIdx - afterPrefix - 1);
    suffix = line.mid(closeIdx + 1);
  } else {
    int idx = afterPrefix;
    while (idx < line.size() && !line.at(idx).isSpace()) {
      ++idx;
    }
    token  = line.mid(afterPrefix, idx - afterPrefix);
    suffix = line.mid(idx);
  }

  if (token.isEmpty()) {
    return line;
  }

  const QString resolved = resolveFile(token);
  const bool changed     = (resolved != token);
  const bool originalIsExistingAbsolute =
      !changed && QFileInfo(token).isAbsolute() && QFileInfo(token).exists();

  if (!changed && !originalIsExistingAbsolute) {
    return line; // unresolved: missing file, typo, or env-var placeholder
  }

  QString rewritten = prefix;
  if (quoteResolved) {
    rewritten += QLatin1Char('"');
    rewritten += resolved;
    rewritten += QLatin1Char('"');
  } else {
    rewritten += resolved;
  }
  rewritten += suffix;
  return rewritten;
}

inline QString
rewriteIncludeLines(const QString& text,
                    const std::function<QString(const QString&)>& resolveFile,
                    bool quoteResolved) {
  const QStringList lines = text.split(QLatin1Char('\n'));
  QStringList result;
  result.reserve(lines.size());
  for (const QString& line : lines) {
    result.append(rewriteIncludeLine(line, resolveFile, quoteResolved));
  }
  return result.join(QLatin1Char('\n'));
}

} // namespace qucs_s

#endif
