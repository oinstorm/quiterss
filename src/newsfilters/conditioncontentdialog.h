/* ============================================================
* QuiteRSS is a open-source cross-platform RSS/Atom news feeds reader
* Copyright (C) 2011-2021 QuiteRSS Team <quiterssteam@gmail.com>
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
* ============================================================ */
#ifndef CONDITIONCONTENTDIALOG_H
#define CONDITIONCONTENTDIALOG_H

#ifdef HAVE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "dialog.h"

class ConditionContentDialog : public Dialog
{
  Q_OBJECT
public:
  explicit ConditionContentDialog(QWidget *parent,
                                  const QString &fieldText,
                                  const QString &conditionText,
                                  bool listMode,
                                  const QString &content);

  QString content() const;
  bool isListMode() const;

private slots:
  void updateCount();
  void acceptDialog();
  void closeDialog();

private:
  QStringList cleanedItems() const;

  bool listMode_;
  QString content_;

  QPlainTextEdit *textEdit_;
  QCheckBox *listModeCheckBox_;
  QLabel *countLabel_;
};

#endif // CONDITIONCONTENTDIALOG_H
