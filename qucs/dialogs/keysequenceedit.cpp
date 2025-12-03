/*
 * Copyright (C) 2014  You‑Tang Lee (YodaLee) <lc85301@gmail.com>
 *
 * Ported and modified in 2025: Andrés Martínez Mera <andresmmera@protonmail.com> – this
 * version contains only the changes required for the port.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>

#include "keysequenceedit.h"

using namespace std;

KeySequenceEdit::KeySequenceEdit(QWidget *parent) : QLineEdit(parent) {}

KeySequenceEdit::KeySequenceEdit(const QKeySequence &keySequence,
                                 QWidget *parent)
    : QLineEdit(parent) {
  setKeySequence(keySequence);
}

KeySequenceEdit::~KeySequenceEdit() {}

void KeySequenceEdit::setKeySequence(const QKeySequence &keySequence)

{
  setText(keySequence.toString(QKeySequence::NativeText));
}

void KeySequenceEdit::keyPressEvent(QKeyEvent *event) {
  keyInt = event->key();
  Qt::Key key = static_cast<Qt::Key>(keyInt);
  if (key == Qt::Key_unknown) {
    qDebug() << "Unknown key from a macro probably";
    return;
  }
  // the user have clicked just and only the special keys Ctrl, Shift, Alt,
  // Meta.
  if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt ||
      key == Qt::Key_Meta) {
    return;
  }

  // check for a combination of user clicks
  // if the keyText is empty than it's a special key like F1, F5, ...
  Qt::KeyboardModifiers modifiers = event->modifiers();
  if (modifiers & Qt::ShiftModifier)
    keyInt += Qt::SHIFT;
  if (modifiers & Qt::ControlModifier)
    keyInt += Qt::CTRL;
  if (modifiers & Qt::AltModifier)
    keyInt += Qt::ALT;
  if (modifiers & Qt::MetaModifier)
    keyInt += Qt::META;
}

void KeySequenceEdit::keyReleaseEvent(QKeyEvent *) {
  qDebug() << "Key Release: "
           << QKeySequence(keyInt).toString(QKeySequence::NativeText);
  setText(QKeySequence(keyInt).toString(QKeySequence::NativeText));
  clearFocus();
}
