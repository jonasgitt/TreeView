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
    view->setAnimated(true);

    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,this, &MainWindow::updateActions);
    connect(actionsMenu, &QMenu::aboutToShow, this, &MainWindow::updateActions);
    connect(insertColumnAction, &QAction::triggered, this, &MainWindow::insertColumn);
    connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    connect(removeColumnAction, &QAction::triggered, this, &MainWindow::removeColumn);
    //connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);
   insertChild("Choose a Command...");
   //connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
//on_newCommand_clicked();
    updateActions();

    view->setItemDelegateForColumn(0, new ComboBoxDelegate(view));

    bool connection = connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), SLOT(updateComboSlot(QModelIndex)));
    qDebug() << "MainWindow Connection: " << connection;

    for (int column = 0; column < model->columnCount(); ++column)
      view->resizeColumnToContents(column);
}


void MainWindow::updateComboSlot(QModelIndex topLeft){

    QVariant newdata = view->model()->data(topLeft);
    if (newdata == "Choose a Command...") return;

    if (view->model()->parent(topLeft).isValid() && topLeft.column() == 1)

   while (view->model()->hasChildren(topLeft)){
       view->model()->removeRow(0, topLeft);
   }

   if (!view->model()->parent(topLeft).isValid() && topLeft.column() == 0){
       InsertParameters(parameterList(newdata));
   }

}

QStringList MainWindow::parameterList(QVariant runOption){

    QStringList parameters;

    if (runOption == "Run" || runOption == "Run with SM") {
        parameters << "Sample#" << "uAmps 3" << "Angle 3" << "uAmps 2" << "Angle 2" << "uAmps 1" << "Angle 1";
     }
    else if (runOption == "Run Transmissions"){
        parameters << "Subtitle" << "uAmps" << "s4vg" << "s3vg" << "s2vg" << "s1vg" << "Height Offset";
    }
    else if (runOption == "NIMA"){
        parameters << "Target Pressure" << "Target Area";
    }
    else if (runOption == "Contrast Change"){
        parameters << "Conc A" << "Volume[mL]" << "Flow[mL/min]" << "Conc D" << "Conc C" << "Conc B";;
    }
    else if (runOption == "Julabo"){
        parameters << "Temperature" << "Run Control Min" << "Run Control Max";
    }
    else if (runOption == "Eurotherm"){
        parameters << "T1" << "T8" << "T7" << "T6" << "T5" << "T4" << "T3" << "T2";
    }
    return parameters;
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

    //for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, 0, index);
        model->setData(child, QVariant(ChildTitle), Qt::EditRole);
        //if (!model->headerData(column, Qt::Horizontal).isValid())
          //  model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);
   //}

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}


void MainWindow::on_newCommand_clicked()
{
   QAbstractItemModel *model = view->model();

   if (!model->index(0,0).isValid()){ //if table is empty
       insertChild("Choose a Command...");
       return;
   }

   int insertionRow;
  QModelIndex selectedIndex = view->selectionModel()->currentIndex();

   if (!view->model()->parent(selectedIndex).isValid()){
           insertionRow = selectedIndex.row()+1;
            model->insertRow(insertionRow);
            qDebug() << "I'm in parent";
   }
   else{
       insertionRow = model->rowCount();
       model->insertRow(insertionRow);
       qDebug() << "I's in child";
   }

   QModelIndex child = model->index(insertionRow, 0);
   model->setData(child, QVariant("Choose a Command..."));
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

    //take the first parameter and add it as a child
    QString parameter = parameters.takeFirst();
    insertChild(parameter);

    foreach (parameter, parameters){
        insertRow(parameter);
    }
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

void MainWindow::on_RemoveCommand_clicked()
{
    QAbstractItemModel *model = view->model();
    if (!model->index(0,0).isValid()) return;

    QModelIndex index = view->selectionModel()->currentIndex();
    if (!view->model()->parent(index).isValid()){
        if (model->removeRow(index.row(), index.parent()))
            updateActions();
    }
}

void MainWindow::removeRow()
{
    on_RemoveCommand_clicked();
}

//conect to dataChanged()
//easiest way is probably just to parse everything if data is changed
//data is stored permanently in the model
//could have a button "Generate Script" so that it isnt parsing for every tiniest changed
//this generate script button would reside in mainwindow
//i must work in mainwindow because that is where the instance of the model resides?
//i can call a function on that instance (the function is declared here <<-- do that
//this is only used for scriptwriting, i can call the scriptLines functions straight from here
// --> then return the scriptLines to MainWindow for printing?
void MainWindow::parseModel(){

    QVector<QVariant> params;
    runstruct runvars;
    QAbstractItemModel *model = view->model();

    QModelIndex childIndex = model->index(0,0, QModelIndex());
    QModelIndex grandChild = model->index(0,1, childIndex);
    qDebug() << "Grandchild Data: " << model->data(grandChild).toString();
    qDebug() << "RootItems Child Count: " << model->rowCount(QModelIndex());

    for (int row = 0; row < model->rowCount(QModelIndex()); row++){

         QModelIndex comboIndex = model->index(row,0, QModelIndex());
         QString comboSelected = model->data(comboIndex).toString();
         qDebug() << "Combo Selected: " << comboSelected;

         for (int par = 0; par < model->rowCount(comboIndex); par++){

             QModelIndex parIndex = model->index(par, 1, comboIndex);// 0 for testing
             QString parameter = model->data(parIndex, Qt::EditRole).toString();
             qDebug() << "Data: " << parameter;

             params.append(model->data(parIndex, Qt::EditRole));
         }

         if (comboSelected == "Run")
            parseRun(params);
         else if (comboSelected == "Contrast Change")
            parseContrast(params);
         else if (comboSelected == "NIMA")
             parseNIMA(params);
         else if (comboSelected == "Eurotherm")
             parseEurotherm(params);
         else if (comboSelected == "Julabo")
             parseJulabo(params);
         else if (comboSelected == "Run Transmissions")
             parseTransm(params);
    }
}


//on collapse and DnD, parse all, print all

void MainWindow::on_parseButton_clicked()
{
    parseModel();
}
