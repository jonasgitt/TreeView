#include <QItemDelegate>
#include "combodelegate.h"
#include <QComboBox>
#include <QWidget>
#include <QDebug>
#include <treemodel.h>
#include <QObject>
ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
: QItemDelegate(parent)
{
}

//creates a ComboBox
QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const{

    QComboBox *editor = new QComboBox(parent);


        editor->addItem("Run");
        editor->addItem("Run with SM");
        editor->addItem("Kinetic Run");
        editor->addItem("Run Transmissions");
        editor->addItem("Set Temperature");
        editor->addItem("NIMA");
        editor->addItem("Contrast Change");

    if (!index.parent().isValid())
      return editor;
}

//A new editor is created every time comobobox is clicked
//this function gets the old data out of the model so that the new editor will have same content as before
void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QString value = index.model()->data(index, Qt::EditRole).toString();

  QComboBox *cBox = static_cast<QComboBox*>(editor);
  cBox->setCurrentIndex(cBox->findText(value));

 connect(editor, SIGNAL(currentIndexChanged(QString)), SLOT(test(QString)));
 //emit closeEditor(editor);
}


//when editing is complete the model is updated
void ComboBoxDelegate::setModelData(QWidget *editor, TreeModel *model,
                               const QModelIndex &index)
{qDebug() << "now!!";
  QComboBox *cBox = static_cast<QComboBox*>(editor);
  QString value = cBox->currentText();

  model->setData(index, value, Qt::EditRole);

  emit commitData(editor);
  emit closeEditor(editor); //this deletes the editor entirely
}


//ensures correct dimensions in table
void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}

void ComboBoxDelegate::test(QString Action){
   QString hae = Action;
    qDebug() << Action;
    emit boxupdate(hae);
}



