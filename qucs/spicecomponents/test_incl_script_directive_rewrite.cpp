#include "incl_script_directive_rewrite.h"

#include <QFileInfo>
#include <QTemporaryFile>
#include <cstdio>
#include <functional>

namespace {

int failures = 0;

void expectEqual(const QString& actual, const QString& expected,
                 const char* testName) {
  if (actual != expected) {
    std::fprintf(stderr, "FAIL: %s\n  expected: %s\n  actual:   %s\n", testName,
                 expected.toUtf8().constData(), actual.toUtf8().constData());
    ++failures;
  }
}

void expectTrue(bool cond, const char* testName) {
  if (!cond) {
    std::fprintf(stderr, "FAIL: %s\n", testName);
    ++failures;
  }
}

} // namespace

int main() {
  using qucs_s::rewriteIncludeLines;

  // 1. Quoted relative path, resolver returns a different (resolved) string.
  {
    auto resolver = [](const QString& f) {
      return QStringLiteral("/abs/dir/") + f;
    };
    const QString in  = QStringLiteral(".INCLUDE \"test file.inc\"\n");
    const QString out = rewriteIncludeLines(in, resolver, true);
    expectEqual(out, QStringLiteral(".INCLUDE \"/abs/dir/test file.inc\"\n"),
                "quoted-relative-resolved");
  }

  // 2. Unquoted relative path, resolver returns a different string.
  {
    auto resolver = [](const QString& f) {
      return QStringLiteral("/abs/dir/") + f;
    };
    const QString in  = QStringLiteral(".include test.inc\n");
    const QString out = rewriteIncludeLines(in, resolver, true);
    expectEqual(out, QStringLiteral(".include \"/abs/dir/test.inc\"\n"),
                "unquoted-relative-resolved");
  }

  // 3. Resolver echoes the input back (not found) -> line unchanged.
  {
    auto echo         = [](const QString& f) { return f; };
    const QString in  = QStringLiteral(".INCLUDE missing.inc\n");
    const QString out = rewriteIncludeLines(in, echo, true);
    expectEqual(out, in, "unresolved-left-untouched");
  }

  // 3b. Environment-variable-style token, resolver echoes back -> untouched.
  {
    auto echo         = [](const QString& f) { return f; };
    const QString in  = QStringLiteral(".INCLUDE \"$HOME/lib.inc\"\n");
    const QString out = rewriteIncludeLines(in, echo, true);
    expectEqual(out, in, "env-var-left-untouched");
  }

  // 4. Token already an absolute, existing path; resolver echoes it back ->
  // still requoted.
  {
    QTemporaryFile tmp;
    expectTrue(tmp.open(), "temp-file-open");
    const QString absPath  = QFileInfo(tmp).absoluteFilePath();
    auto echo              = [](const QString& f) { return f; };
    const QString in       = QStringLiteral(".include %1\n").arg(absPath);
    const QString out      = rewriteIncludeLines(in, echo, true);
    const QString expected = QStringLiteral(".include \"%1\"\n").arg(absPath);
    expectEqual(out, expected, "absolute-existing-requoted");
  }

  // 5. quoteResolved = false -> no quotes even though resolved.
  {
    auto resolver = [](const QString& f) {
      return QStringLiteral("/abs/dir/") + f;
    };
    const QString in  = QStringLiteral(".INCLUDE \"test.inc\"\n");
    const QString out = rewriteIncludeLines(in, resolver, false);
    expectEqual(out, QStringLiteral(".INCLUDE /abs/dir/test.inc\n"),
                "quote-resolved-false");
  }

  // 6. Case-insensitivity, original keyword case preserved.
  {
    auto resolver = [](const QString& f) {
      return QStringLiteral("/abs/dir/") + f;
    };
    const QString in  = QStringLiteral(".Include test.inc\n");
    const QString out = rewriteIncludeLines(in, resolver, true);
    expectEqual(out, QStringLiteral(".Include \"/abs/dir/test.inc\"\n"),
                "case-insensitive-preserved-case");
  }

  // 7. Full-line comment left untouched; resolver never invoked.
  {
    int calls = 0;
    std::function<QString(const QString&)> counting =
        [&calls](const QString& f) {
          ++calls;
          return f;
        };
    const QString in  = QStringLiteral("* .INCLUDE foo.inc\n");
    const QString out = rewriteIncludeLines(in, counting, true);
    expectEqual(out, in, "comment-line-untouched");
    expectTrue(calls == 0, "comment-line-resolver-not-called");
  }

  // 8. Mixed block: only .INCLUDE lines touched, others preserved exactly,
  // order kept.
  {
    auto resolver = [](const QString& f) {
      return QStringLiteral("/abs/") + f;
    };
    const QString in       = QStringLiteral(".PARAM rp = 1k\n"
                                                  ".INCLUDE a.inc\n"
                                                  ".FUNC prod(x,y) = {x*y}\n"
                                                  "\n"
                                                  ".INCLUDE b.inc\n");
    const QString out      = rewriteIncludeLines(in, resolver, true);
    const QString expected = QStringLiteral(".PARAM rp = 1k\n"
                                            ".INCLUDE \"/abs/a.inc\"\n"
                                            ".FUNC prod(x,y) = {x*y}\n"
                                            "\n"
                                            ".INCLUDE \"/abs/b.inc\"\n");
    expectEqual(out, expected, "mixed-block-only-includes-touched");
  }

  // 9. Quoted filename with an embedded space: resolver receives the whole
  // token.
  {
    QString capturedArg;
    std::function<QString(const QString&)> capture =
        [&capturedArg](const QString& f) {
          capturedArg = f;
          return QStringLiteral("/abs/") + f;
        };
    const QString in  = QStringLiteral(".INCLUDE \"lib file.inc\"\n");
    const QString out = rewriteIncludeLines(in, capture, true);
    expectEqual(capturedArg, QStringLiteral("lib file.inc"),
                "space-token-captured-whole");
    expectEqual(out, QStringLiteral(".INCLUDE \"/abs/lib file.inc\"\n"),
                "space-token-requoted");
  }

  // 10. Unterminated quote -> left unchanged, resolver never invoked.
  {
    int calls = 0;
    std::function<QString(const QString&)> counting =
        [&calls](const QString& f) {
          ++calls;
          return f;
        };
    const QString in  = QStringLiteral(".INCLUDE \"broken.inc\n");
    const QString out = rewriteIncludeLines(in, counting, true);
    expectEqual(out, in, "unterminated-quote-untouched");
    expectTrue(calls == 0, "unterminated-quote-resolver-not-called");
  }

  // 11. No trailing newline in input -> none in output either.
  {
    auto resolver = [](const QString& f) {
      return QStringLiteral("/abs/") + f;
    };
    const QString in  = QStringLiteral(".INCLUDE a.inc");
    const QString out = rewriteIncludeLines(in, resolver, true);
    expectEqual(out, QStringLiteral(".INCLUDE \"/abs/a.inc\""),
                "no-trailing-newline-preserved");
  }

  // 12. Indentation, directive case, inner spacing, trailing comment, and
  // trailing newline are all preserved around a rewritten quoted path.
  {
    QString capturedArg;
    std::function<QString(const QString&)> capture =
        [&capturedArg](const QString& f) {
          capturedArg = f;
          return QStringLiteral("/abs/a.inc");
        };
    const QString in  = QStringLiteral("\t.Include   \"a.inc\" ; keep this\n");
    const QString out = rewriteIncludeLines(in, capture, true);
    expectEqual(capturedArg, QStringLiteral("a.inc"),
                "surroundings-preserved-resolver-arg");
    expectEqual(out,
                QStringLiteral("\t.Include   \"/abs/a.inc\" ; keep this\n"),
                "surroundings-preserved");
  }

  if (failures == 0) {
    std::printf("All InclScriptDirectiveRewrite tests passed.\n");
    return 0;
  }
  std::fprintf(stderr, "%d test(s) failed.\n", failures);
  return 1;
}
