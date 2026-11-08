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
#include "itemcondition.h"

#include "conditioncontentdialog.h"

ItemCondition::ItemCondition(QWidget * parent)
  : QWidget(parent)
  , listMode_(false)
{
  comboBox1_ = new QComboBox(this);
  comboBox2_ = new QComboBox(this);
  comboBox3_ = new QComboBox(this);
  lineEdit_ = new LineEdit(this);

  QStringList itemList;
  itemList << tr("Title")  << tr("Description")
           << tr("Author") << tr("Category") << tr("State")
           << tr("Link") << tr("News")
              /*<< tr("Published") << tr("Received")*/;
  comboBox1_->addItems(itemList);

  itemList.clear();
  itemList << tr("New") << tr("Read") << tr("Starred");
  comboBox3_->addItems(itemList);

  listSummary_ = new QLabel(this);
  listSummary_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  listSummary_->setVisible(false);

  editButton_ = new ToolButton(this);
  editButton_->setText("...");
  editButton_->setToolTip(tr("Edit content in a large editor"));
  editButton_->setAutoRaise(true);

  addButton_ = new ToolButton(this);
  addButton_->setIcon(QIcon(":/images/addT"));
  addButton_->setToolTip(tr("Add Condition"));
  addButton_->setAutoRaise(true);
  deleteButton_ = new ToolButton(this);
  deleteButton_->setIcon(QIcon(":/images/deleteT"));
  deleteButton_->setToolTip(tr("Delete Condition"));
  deleteButton_->setAutoRaise(true);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  buttonsLayout->setAlignment(Qt::AlignCenter);
  buttonsLayout->setMargin(0);
  buttonsLayout->setSpacing(5);
  buttonsLayout->addWidget(comboBox1_);
  buttonsLayout->addWidget(comboBox2_);
  buttonsLayout->addWidget(comboBox3_, 1);
  buttonsLayout->addWidget(lineEdit_, 1);
  buttonsLayout->addWidget(listSummary_, 1);
  buttonsLayout->addWidget(editButton_);
  buttonsLayout->addWidget(addButton_);
  buttonsLayout->addWidget(deleteButton_);

  setLayout(buttonsLayout);

  currentIndexChanged(tr("Title"));

  connect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteFilterRules()));
  connect(editButton_, SIGNAL(clicked()), this, SLOT(editContent()));
  connect(comboBox1_, SIGNAL(currentIndexChanged(QString)),
          this, SLOT(currentIndexChanged(QString)));
}

void ItemCondition::deleteFilterRules()
{
  emit signalDeleteCondition(this);
}

void ItemCondition::currentIndexChanged(const QString &str)
{
  QStringList itemList;

  comboBox2_->clear();
  comboBox3_->setVisible(false);
  if (str == tr("State")) {
    itemList << tr("is") << tr("isn't");
    comboBox2_->addItems(itemList);
    comboBox3_->setVisible(true);
    lineEdit_->setVisible(false);
    listSummary_->setVisible(false);
    editButton_->setVisible(false);
    return;
  }

  if (str == tr("Title")) {
    itemList << tr("contains") << tr("doesn't contain")
             << tr("is") << tr("isn't")
             << tr("begins with") << tr("ends with")
             << tr("Regular expressions");
    comboBox2_->addItems(itemList);
  } else if (str == tr("Description")) {
    itemList << tr("contains") << tr("doesn't contain")
             << tr("Regular expressions");
    comboBox2_->addItems(itemList);
  } else if (str == tr("Author")) {
    itemList << tr("contains") << tr("doesn't contain")
             << tr("is") << tr("isn't")
             << tr("Regular expressions");
    comboBox2_->addItems(itemList);
  } else if (str == tr("Category")) {
    itemList << tr("contains") << tr("doesn't contain")
             << tr("is") << tr("isn't")
             << tr("begins with") << tr("ends with")
             << tr("Regular expressions");
    comboBox2_->addItems(itemList);
  } else if (str == tr("Link")) {
    itemList << tr("contains") << tr("doesn't contain")
             << tr("is") << tr("isn't")
             << tr("begins with") << tr("ends with")
             << tr("Regular expressions");
    comboBox2_->addItems(itemList);
  } else if (str == tr("News")) {
    itemList << tr("contains") << tr("doesn't contain")
             << tr("Regular expressions");
    comboBox2_->addItems(itemList);
  }

  if (listMode_) {
    lineEdit_->setVisible(false);
    listSummary_->setVisible(true);
  } else {
    lineEdit_->setVisible(true);
    listSummary_->setVisible(false);
  }
  editButton_->setVisible(true);
}

int ItemCondition::itemCount() const
{
  return listContent_.split('\n', QString::SkipEmptyParts).count();
}

QString ItemCondition::content() const
{
  if (listMode_) return listContent_;
  return lineEdit_->text();
}

void ItemCondition::setContent(const QString &content, bool list)
{
  listMode_ = list;
  if (list) {
    QStringList items;
    QStringList lines = content.split('\n', QString::SkipEmptyParts);
    foreach (const QString &line, lines) {
      QString item = line.trimmed();
      if (item.isEmpty() || items.contains(item)) continue;
      items.append(item);
    }
    listContent_ = items.join("\n");
  } else {
    listContent_.clear();
    lineEdit_->setText(content);
  }
  if (comboBox1_->currentIndex() == 4) {
    lineEdit_->setVisible(false);
    listSummary_->setVisible(false);
    editButton_->setVisible(false);
  } else if (listMode_) {
    lineEdit_->setVisible(false);
    listSummary_->setVisible(true);
  } else {
    lineEdit_->setVisible(true);
    listSummary_->setVisible(false);
  }
  updateListSummary();
}

void ItemCondition::updateListSummary()
{
  int n = itemCount();
  listSummary_->setText(tr("%1 items").arg(n));
  if (n > 0) {
    QStringList items = listContent_.split('\n', QString::SkipEmptyParts);
    QStringList previewItems = items.mid(0, 3);
    QString preview = previewItems.join("; ");
    if (items.count() > 3) preview.append("...");
    listSummary_->setToolTip(preview);
  } else {
    listSummary_->setToolTip(tr("Empty list"));
  }
}

void ItemCondition::editContent()
{
  ConditionContentDialog dlg(this, comboBox1_->currentText(),
                             comboBox2_->currentText(),
                             comboBox1_->currentIndex(),
                             comboBox2_->currentIndex(),
                             listMode_, content(),
                             tr("https://example.com/news/123"));
  if (dlg.exec() == QDialog::Accepted) {
    setContent(dlg.content(), dlg.isListMode());
    emit signalConditionChanged();
  }
}