#pragma once
#include <QWidget>
#include <QString>
class QPlainTextEdit; class QLabel; class QSyntaxHighlighter;
class EditorWidget:public QWidget{
 Q_OBJECT
public: explicit EditorWidget(QWidget *parent=nullptr); void setProjectFolder(const QString &path);
private slots: void openPath(const QString &path); void newFile(); void openFile(); void saveFile(); void saveAs();
private:
 QPlainTextEdit *edit; QLabel *pathLabel; QLabel *statusLabel; class QFileSystemModel *fileModel=nullptr; class QTreeView *fileTree=nullptr; QString currentPath; bool writeFile(const QString&); bool maybeSave(); void updateStatus(); void applyLanguageHighlighting();
 QSyntaxHighlighter *highlighter=nullptr;
};
