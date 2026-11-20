#pragma once
#include <QWidget>
class QPlainTextEdit; class QLabel;
class EditorWidget:public QWidget{
 Q_OBJECT
public: explicit EditorWidget(QWidget *parent=nullptr);
private slots: void openFile(); void saveFile(); void saveAs();
private: QPlainTextEdit *edit; QLabel *pathLabel; QString currentPath; bool writeFile(const QString&);
};
