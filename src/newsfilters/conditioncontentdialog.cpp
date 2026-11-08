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

namespace {

enum MatchKind {
  MatchContains,
  MatchNotContains,
  MatchIs,
  MatchIsNot,
  MatchBeginsWith,
  MatchEndsWith,
  MatchRegExp,
  MatchNone
};

MatchKind matchKind(int fieldIndex, int conditionIndex)
{
  switch (fieldIndex) {
  case 0: // Title
  case 3: // Category
  case 5: // Link
    switch (conditionIndex) {
    case 0: return MatchContains;
    case 1: return MatchNotContains;
    case 2: return MatchIs;
    case 3: return MatchIsNot;
    case 4: return MatchBeginsWith;
    case 5: return MatchEndsWith;
    case 6: return MatchRegExp;
    default: return MatchNone;
    }
  case 2: // Author
    switch (conditionIndex) {
    case 0: return MatchContains;
    case 1: return MatchNotContains;
    case 2: return MatchIs;
    case 3: return MatchIsNot;
    case 4: return MatchRegExp;
    default: return MatchNone;
    }
  case 1: // Description
  case 6: // News
    switch (conditionIndex) {
    case 0: return MatchContains;
    case 1: return MatchNotContains;
    case 2: return MatchRegExp;
    default: return MatchNone;
    }
  default:
    return MatchNone;
  }
}

} // namespace

ConditionContentDialog::ConditionContentDialog(QWidget *parent,
                                               const QString &fieldText,
                                               const QString &conditionText,
                                               int fieldIndex,
                                               int conditionIndex,
                                               bool listMode,
                                               const QString &content,
                                               const QString &urlLiteral)
  : Dialog(parent, Qt::WindowMinMaxButtonsHint)
  , listMode_(listMode)
  , content_(content)
  , fieldText_(fieldText)
  , conditionText_(conditionText)
  , fieldIndex_(fieldIndex)
  , conditionIndex_(conditionIndex)
{
  setWindowTitle(tr("Edit Condition Content"));
  setMinimumSize(520, 340);
  resize(660, 500);

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

  testToggleButton_ = new QToolButton(this);
  testToggleButton_->setText(tr("Test..."));
  testToggleButton_->setToolTip(tr("Check a sample text against these conditions"));
  testToggleButton_->setCheckable(true);
  testToggleButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  testToggleButton_->setArrowType(Qt::RightArrow);

  testPanel_ = new QWidget(this);
  QLabel *sampleLabel = new QLabel(tr("Sample text:"), testPanel_);
  sampleEdit_ = new QPlainTextEdit(testPanel_);
  sampleEdit_->setMinimumHeight(70);
  sampleEdit_->setMaximumHeight(130);
  sampleEdit_->setTabChangesFocus(true);
#ifdef HAVE_QT5
  if (fieldText == tr("Link") && !urlLiteral.isEmpty())
    sampleEdit_->setPlaceholderText(tr("Paste a link, for example: %1").arg(urlLiteral));
  else
    sampleEdit_->setPlaceholderText(tr("Paste the text that a news item should contain"));
#endif
  testResultLabel_ = new QLabel(testPanel_);
  testResultLabel_->setWordWrap(true);
  testResultLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);

  QVBoxLayout *testLayout = new QVBoxLayout();
  testLayout->setMargin(0);
  testLayout->setSpacing(4);
  testLayout->addWidget(sampleLabel);
  testLayout->addWidget(sampleEdit_);
  testLayout->addWidget(testResultLabel_);
  testPanel_->setLayout(testLayout);
  testPanel_->setVisible(false);

  pageLayout->addWidget(infoLabel);
  pageLayout->addWidget(listModeCheckBox_);
  pageLayout->addWidget(textEdit_, 1);
  pageLayout->addWidget(countLabel_);

  QHBoxLayout *testButtonLayout = new QHBoxLayout();
  testButtonLayout->setMargin(0);
  testButtonLayout->addWidget(testToggleButton_);
  testButtonLayout->addStretch();
  pageLayout->addLayout(testButtonLayout);
  pageLayout->addWidget(testPanel_);

  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptDialog()));
  connect(listModeCheckBox_, SIGNAL(toggled(bool)), this, SLOT(updateCount()));
  connect(textEdit_, SIGNAL(textChanged()), this, SLOT(updateCount()));
  connect(testToggleButton_, SIGNAL(toggled(bool)), this, SLOT(toggleTestPanel(bool)));
  connect(sampleEdit_, SIGNAL(textChanged()), this, SLOT(runTest()));

  Settings settings;
  restoreGeometry(settings.value("conditionContentDlg/geometry").toByteArray());
  if (settings.value("conditionContentDlg/testPanelVisible", false).toBool())
    testToggleButton_->setChecked(true);

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

void ConditionContentDialog::toggleTestPanel(bool visible)
{
  testPanel_->setVisible(visible);
  testToggleButton_->setArrowType(visible ? Qt::DownArrow : Qt::RightArrow);
  if (visible) {
    runTest();
    sampleEdit_->setFocus();
  }
}

void ConditionContentDialog::showTestResult(bool matched, const QString &detail)
{
  const char *color = matched ? "#1b7d1b" : "#a33";
  const QString note = detail.isEmpty() ? QString() :
                       QString(" (%1)").arg(detail.toHtmlEscaped());
  testResultLabel_->setText(
        QString("<b style=\"color:%1\">%2</b>%3").arg(
          color, matched ? tr("MATCHED") : tr("NOT MATCHED"), note));
}

void ConditionContentDialog::runTest()
{
  const QString sample = sampleEdit_->toPlainText().trimmed();
  if (sample.isEmpty()) {
    testResultLabel_->setText(tr("Enter a sample text to test this condition."));
    return;
  }

  QStringList values;
  if (listModeCheckBox_->isChecked())
    values = cleanedItems();
  else
    values = QStringList(textEdit_->toPlainText().trimmed());

  values.removeAll(QString());
  if (values.isEmpty()) {
    testResultLabel_->setText(tr("No value to test."));
    return;
  }

  QString errorText;
  QStringList matchedFragments;
  foreach (const QString &value, values) {
    if (testMatch(fieldIndex_, conditionIndex_, value, sample, &errorText))
      matchedFragments.append(QString("\"%1\"").arg(value));
    if (!errorText.isEmpty())
      break;
  }

  if (!errorText.isEmpty()) {
    testResultLabel_->setText(
          QString("<b style=\"color:#a33\">%1</b> %2").arg(
            tr("Invalid condition:"), errorText.toHtmlEscaped()));
    return;
  }

  if (matchedFragments.isEmpty()) {
    if (values.count() == 1)
      showTestResult(false, tr("The sample does not meet this condition."));
    else
      showTestResult(false, tr("None of the %1 values match.").arg(values.count()));
    return;
  }

  if (values.count() == 1) {
    showTestResult(true, tr("The sample meets this condition."));
  } else {
    showTestResult(true, tr("%1 of %2 values match: %3").
                   arg(matchedFragments.count()).
                   arg(values.count()).
                   arg(matchedFragments.join(" ")));
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
  settings.setValue("conditionContentDlg/testPanelVisible",
                    testToggleButton_->isChecked());
}

bool ConditionContentDialog::testMatch(int fieldIndex, int conditionIndex,
                                       const QString &value,
                                       const QString &sample,
                                       QString *errorText)
{
  if (errorText) errorText->clear();

  const MatchKind kind = matchKind(fieldIndex, conditionIndex);
  if (kind == MatchNone) {
    if (errorText) *errorText = tr("Unsupported condition for this field.");
    return false;
  }

  if (kind == MatchRegExp) {
#ifdef HAVE_QT5
    QRegularExpression re(value);
    if (!re.isValid()) {
      if (errorText) *errorText = tr("Regular expression error at %1: %2").
          arg(re.patternErrorOffset()).arg(re.errorString());
      return false;
    }
    return re.match(sample).hasMatch();
#else
    QRegExp re(value);
    if (!re.isValid()) {
      if (errorText) *errorText = tr("Regular expression error: %1").arg(re.errorString());
      return false;
    }
    return re.indexIn(sample) >= 0;
#endif
  }

  // The engine compares Title/Description/Author/Category/News with UPPER(),
  // while Link is matched case-sensitively.
  const bool ignoreCase = (fieldIndex != 5);
  const QString haystack = ignoreCase ? sample.toUpper() : sample;
  const QString needle = ignoreCase ? value.toUpper() : value;

  switch (kind) {
  case MatchContains:
    return haystack.contains(needle);
  case MatchNotContains:
    return !haystack.contains(needle);
  case MatchIs:
    return haystack == needle;
  case MatchIsNot:
    return haystack != needle;
  case MatchBeginsWith:
    return haystack.startsWith(needle);
  case MatchEndsWith:
    return haystack.endsWith(needle);
  default:
    break;
  }
  return false;
}