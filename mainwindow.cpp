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
    //connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);
    insertChild("new child");
   //connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);

    updateActions();

    view->setItemDelegateForColumn(0, new ComboBoxDelegate(view));
//bool connection = connect(view->itemDelegate(),SIGNAL(boxupdate(QString)), SLOT(updateComboSlot()));
//qDebug() << connection;


    bool connection = connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), SLOT(updateComboSlot(QModelIndex)));
    qDebug() << "MainWindow Connection: " << connection;


      QVariant a = model->index(0, 0, QModelIndex()).data();
      qDebug() << "stored at a: " << a.toString();

      for (int column = 0; column < model->columnCount(); ++column)
          view->resizeColumnToContents(column);
}


//MUST DELETE OTHER CHILDREN BEFORE ADDING NEW ONES!!
void MainWindow::updateComboSlot(QModelIndex topLeft){

    QStringList parameters;
   //if we're not in a top level node, return straight away. or if we not in column 0
   if (view->model()->parent(topLeft).isValid() || topLeft.column() != 0)
       return;

   QVariant newdata = view->model()->data(topLeft);
   while (view->model()->hasChildren(topLeft)){
       view->model()->removeRow(0, topLeft);
   }


    qDebug() << "Mainwindow update combo slot. New Data" << newdata.toString();

    insertChild("Parameters:");

    if (newdata == "Run" || newdata == "Run with SM") {
        parameters << "Sample #" << "Angle 1" << "uAmps 1" << "Angle 2" << "uAmps 2" << "Angle 3" << "uAmps 3";
        InsertParameters(parameters);
    }
    else if (newdata == "Run Transmissions"){
        parameters << "Subtitle" << "Height Offset" << "s1vg" << "s2vg" << "s3vg" << "s4vg" << "uAmps";
        InsertParameters(parameters);
    }
    else if (newdata == "NIMA"){
        parameters << "Target Pressure" << "Target Area";
        InsertParameters(parameters);
    }
    else if (newdata == "Contrast Change"){
        parameters << "Conc A" << "Conc B" << "Conc C" << "Conc d" << "Flow[mL/min]" << "Volume[mL]";
        InsertParameters(parameters);
    }
    else if (newdata == "Julabo"){
        parameters << "Temperature" << "Run Control Min" << "Run Control Max";
    }
    else if (newdata == "EuroTherm"){
        parameters << "T1" << "T2" << "T3" << "T4" << "T5" << "T6" << "T7" << "T8";
    }
}


void MainWindow::insertChild(QString ChildTitle)
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
        model->setData(child, QVariant(ChildTitle), Qt::EditRole);
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
void MainWindow::insertRow(QString Action)
{
    QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel *model = view->model();

    if (!model->insertRow(index.row()+1, index.parent()))//this is where insertion happens
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        model->setData(child, QVariant(Action), Qt::EditRole);
    }
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
        insertChild("new");

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

void MainWindow::InsertParameters(QStringList parameters){

    QString parameter;

    foreach (parameter, parameters){
        insertRow(parameter);
    }
}


