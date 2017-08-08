/*#ifndef NODE_H
#define NODE_H


class Node {
public:
    Node (const QString& aText = "NO Data", Node *aParent= 0)
    ~Node();
    QVariant data() const;
public:
    QString text;
    Node *parent;
    QList <Node*> children;

};

#endif // NODE_H

Drag and Drop!

class DndModel : public InsertRemoveModel {

public:
    Qt::ItemFlags flags(index) const;
    Qt::DropActions supportedDragActions() const;
    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(indexes) const;

    bool dropMimeData(data, dropAction, row, column, parent);
    bool removeRows(row, count, parent);
    bool insertRows(row, count, parent);

    //https://www.youtube.com/watch?v=d2OFjACLgOg&t=333s
    //implementation at 13:09
}
*/
