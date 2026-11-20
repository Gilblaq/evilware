#include "editorwidget.h"
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
EditorWidget::EditorWidget(QWidget *p):QWidget(p){
 auto *v=new QVBoxLayout(this); auto *bar=new QHBoxLayout; auto *o=new QPushButton("Open");auto *s=new QPushButton("Save");auto *sa=new QPushButton("Save as");pathLabel=new QLabel("Untitled");
 bar->addWidget(o);bar->addWidget(s);bar->addWidget(sa);bar->addStretch();bar->addWidget(pathLabel);v->addLayout(bar);
 edit=new QPlainTextEdit;edit->setLineWrapMode(QPlainTextEdit::NoWrap);edit->setStyleSheet("font-family:Consolas,monospace;background:#101722;color:#e3edf8;selection-background-color:#245b91;");
 v->addWidget(edit,1);connect(o,&QPushButton::clicked,this,&EditorWidget::openFile);connect(s,&QPushButton::clicked,this,&EditorWidget::saveFile);connect(sa,&QPushButton::clicked,this,&EditorWidget::saveAs);
}
void EditorWidget::openFile(){QString p=QFileDialog::getOpenFileName(this,"Open source file");if(p.isEmpty())return;QFile f(p);if(!f.open(QIODevice::ReadOnly|QIODevice::Text)){QMessageBox::warning(this,"Open failed",f.errorString());return;}edit->setPlainText(QString::fromUtf8(f.readAll()));currentPath=p;pathLabel->setText(p);}
bool EditorWidget::writeFile(const QString&p){QFile f(p);if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::warning(this,"Save failed",f.errorString());return false;}f.write(edit->toPlainText().toUtf8());currentPath=p;pathLabel->setText(p);return true;}
void EditorWidget::saveFile(){if(currentPath.isEmpty()){saveAs();return;}writeFile(currentPath);}
void EditorWidget::saveAs(){QString p=QFileDialog::getSaveFileName(this,"Save source file");if(!p.isEmpty())writeFile(p);}
