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

#ifndef KEYSEQUENCEEDIT_H_
#define KEYSEQUENCEEDIT_H_ value

#include <QLineEdit>

class KeySequenceEdit : public QLineEdit {
  Q_OBJECT
public:
  KeySequenceEdit(QWidget *parent = 0);
  KeySequenceEdit(const QKeySequence &keySequence, QWidget *parent = 0);
  virtual ~KeySequenceEdit();

protected:
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);
private slots:
  void setKeySequence(const QKeySequence &);

private:
  int keyInt;
};

#endif /* end of include guard: KEYSEQUENCEEDIT_H_ */
