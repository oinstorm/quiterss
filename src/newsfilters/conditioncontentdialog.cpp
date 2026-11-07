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
#include "conditioncontentdialog.h"

#include "settings.h"

ConditionContentDialog::ConditionContentDialog(QWidget *parent,
                                               const QString &fieldText,
                                               const QString &conditionText,
                                               bool listMode,
                                               const QString &content)
  : Dialog(parent, Qt::WindowMinMaxButtonsHint)
  , listMode_(listMode)
  , content_(content)
{
  setWindowTitle(tr("Edit Condition Content"));
  setMinimumSize(480, 320);
  resize(620, 460);

  QLabel *infoLabel = new QLabel(
        tr("Field: %1    Condition: %2").arg(fieldText).arg(conditionText), this);
  QFont infoFont = infoLabel->font();
  infoFont.setBold(true);
  infoLabel->setFont(infoFont);

  listModeCheckBox_ = new QCheckBox(tr("List mode (one value per line)"), this);
  listModeCheckBox_->setChecked(listMode_);

  textEdit_ = new QPlainTextEdit(this);
  textEdit_->setPlainText(content_);
  textEdit_->setTabChangesFocus(true);
  textEdit_->setLineWrapMode(QPlainTextEdit::NoWrap);
#ifdef HAVE_QT5
  textEdit_->setPlaceholderText(
        listMode_ ? tr("One value per line, for example:\nkeyword1\nkeyword2")
                  : tr("Enter a value"));
#endif

  countLabel_ = new QLabel(this);

  pageLayout->addWidget(infoLabel);
  pageLayout->addWidget(listModeCheckBox_);
  pageLayout->addWidget(textEdit_, 1);
  pageLayout->addWidget(countLabel_);

  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptDialog()));
  connect(listModeCheckBox_, SIGNAL(toggled(bool)), this, SLOT(updateCount()));
  connect(textEdit_, SIGNAL(textChanged()), this, SLOT(updateCount()));

  Settings settings;
  restoreGeometry(settings.value("conditionContentDlg/geometry").toByteArray());

  connect(this, SIGNAL(finished(int)), this, SLOT(closeDialog()));

  updateCount();
}

QString ConditionContentDialog::content() const
{
  return content_;
}

bool ConditionContentDialog::isListMode() const
{
  return listMode_;
}

QStringList ConditionContentDialog::cleanedItems() const
{
  QStringList items;
  QStringList lines = textEdit_->toPlainText().split('\n');
  foreach (const QString &line, lines) {
    QString item = line.trimmed();
    if (item.isEmpty()) continue;
    if (!items.contains(item)) items.append(item);
  }
  return items;
}

void ConditionContentDialog::updateCount()
{
  if (listModeCheckBox_->isChecked()) {
    countLabel_->setText(
          tr("Valid items: %1 (blank lines and duplicates are removed)").
          arg(cleanedItems().count()));
  } else {
    countLabel_->setText(tr("Single value; line breaks are converted to spaces."));
  }
}

void ConditionContentDialog::acceptDialog()
{
  listMode_ = listModeCheckBox_->isChecked();
  if (listMode_) {
    QStringList items = cleanedItems();
    if (items.isEmpty()) {
      QMessageBox::warning(this, tr("Edit Condition Content"),
                           tr("Please enter at least one value."));
      textEdit_->setFocus();
      return;
    }
    content_ = items.join("\n");
  } else {
    QString value = textEdit_->toPlainText().trimmed();
    if (value.isEmpty()) {
      QMessageBox::warning(this, tr("Edit Condition Content"),
                           tr("Please enter a value."));
      textEdit_->setFocus();
      return;
    }
    value.replace('\r', ' ');
    value.replace('\n', ' ');
    content_ = value;
  }
  accept();
}

void ConditionContentDialog::closeDialog()
{
  Settings settings;
  settings.setValue("conditionContentDlg/geometry", saveGeometry());
}
