#include <QDebug>
#include "mainwindow.h"
#include "treemodel.h"
#include "combodelegate.h"
#include <QFile>
#include <QComboBox>
#include <QTreeView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    QStringList headers;
    headers << tr("Action") << tr("Summary of Command");

    QFile file(":/default.txt");
    file.open(QIODevice::ReadOnly);
    TreeModel *model = new TreeModel(headers, file.readAll());
    file.close();

    view->setModel(model);


    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,this, &MainWindow::updateActions);
    connect(actionsMenu, &QMenu::aboutToShow, this, &MainWindow::updateActions);
    connect(insertColumnAction, &QAction::triggered, this, &MainWindow::insertColumn);
    connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    connect(removeColumnAction, &QAction::triggered, this, &MainWindow::removeColumn);
    connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);
    connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);

    updateActions();

    //view->setItemDelegateForColumn(0, new ComboBoxDelegate(view));
//bool connection = connect(view->itemDelegate(),SIGNAL(boxupdate(QString)), SLOT(updateComboSlot()));
//qDebug() << connection;




      QVariant a = model->index(0, 0, QModelIndex()).data();
      qDebug() << "stored at a: " << a.toString();

      for (int column = 0; column < model->columnCount(); ++column)
          view->resizeColumnToContents(column);
}

void MainWindow::updateComboSlot(){
qDebug() << "Mainwindow update combo slot";
}

void MainWindow::insertChild()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant("CellContent"), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);
    }

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool MainWindow::insertColumn()
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);

    updateActions();

    return changed;
}


//PROBLEM: this finds the index of what ever is selected (which depends on the user!!) ... is this even a problem?
void MainWindow::insertRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(index.row()+1, index.parent()))//this is where insertion happens
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        model->setData(child, QVariant("CellContent"), Qt::EditRole);
    }

    QComboBox *combo = new QComboBox();
    QStringList Options;
    Options << "Run" << "Run with SM" << "Set Temperature" << "NIMA" << "Run Transmissions";
    combo->addItems(Options);
     view->setIndexWidget(index, combo);

     connect(combo, SIGNAL(currentIndexChanged(int)), SLOT(showParams(int)));
}

void MainWindow::showParams(int Action){

    //update model data for the combobox
   QModelIndex boxIndex =  view->model()->index(0,0, QModelIndex());


   switch(Action){
        case 0: {//run
        view->model()->setData(boxIndex,QVariant("Run"), Qt::DisplayRole);
        QVariant data = view->model()->index(0,0,QModelIndex()).data();
        qDebug() << "new data" << data.toString();
        //insert childern and set their data
        insertChild();
        insertRow();
        insertRow();
   }
    case 1: //Run with SM
break;
    case 2:// Set Temp
break;
    case 3: //NIMA
        break;
    case 4: //Run Transmissions
break;
    }
}

bool MainWindow::removeColumn()
{
    QAbstractItemModel *model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column);

    if (changed)
        updateActions();

    return changed;
}

void MainWindow::removeRow()
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

//menu bar "Actions"
void MainWindow::updateActions()
{
    bool hasSelection = !view->selectionModel()->selection().isEmpty(); //if nothing is selected, add row/col is disabled since we dont know where to put it
    removeRowAction->setEnabled(hasSelection);
    removeColumnAction->setEnabled(hasSelection);

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        int row = view->selectionModel()->currentIndex().row();
        int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid())//is valid since it has a parent
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}

void MainWindow::InsertRunPar(){
/*
    insertChild("Sample Number");
    QStringList parameters;
    parameters << "Angle 1" << "uAmps 1" << "Angle 2" << "uAmps 2" << "Angle 3" << "uAmps 3";

    QString parameter;

    foreach (parameter, parameters){
        insertRow(parameter);
    }
*/
}


