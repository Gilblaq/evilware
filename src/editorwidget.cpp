#include "editorwidget.h"
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextCursor>
#include <QRegularExpression>
#include <QFont>
#include <QShortcut>
#include <QKeySequence>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSplitter>
#include <QFileInfo>
#include <QDir>

class CodeHighlighter final : public QSyntaxHighlighter {
public:
 explicit CodeHighlighter(QTextDocument *doc):QSyntaxHighlighter(doc){
  keywordFormat.setForeground(QColor("#c792ea"));keywordFormat.setFontWeight(QFont::DemiBold);
  typeFormat.setForeground(QColor("#82aaff"));
  stringFormat.setForeground(QColor("#c3e88d"));
  commentFormat.setForeground(QColor("#697b91"));commentFormat.setFontItalic(true);
  numberFormat.setForeground(QColor("#f78c6c"));
  functionFormat.setForeground(QColor("#ffcb6b"));
 }
 void setLanguage(const QString &language){lang=language;rehighlight();}
protected:
 void highlightBlock(const QString &text) override {
  const bool python=lang=="python";
  const QStringList keywords=python
   ?QStringList{"and","as","assert","async","await","break","class","continue","def","del","elif","else","except","False","finally","for","from","global","if","import","in","is","lambda","None","nonlocal","not","or","pass","raise","return","True","try","while","with","yield"}
   :QStringList{"alignas","auto","bool","break","case","catch","char","class","const","constexpr","continue","default","delete","do","double","else","enum","explicit","extern","false","float","for","friend","if","inline","int","long","namespace","new","noexcept","nullptr","private","protected","public","return","short","signed","sizeof","static","struct","switch","template","this","throw","true","try","typedef","typename","union","unsigned","using","virtual","void","volatile","while","include","define","export","let","var","function","const","async","await","import","from"};
  for(const QString &word:keywords){QRegularExpression re(QString("\\b")+QRegularExpression::escape(word)+"\\b");auto it=re.globalMatch(text);while(it.hasNext()){auto m=it.next();setFormat(m.capturedStart(),m.capturedLength(),keywordFormat);}}
  QRegularExpression types("\\b(int|char|bool|float|double|void|string|QString|QWidget|auto|size_t|std::string|list|dict|tuple|set)\\b");auto ti=types.globalMatch(text);while(ti.hasNext()){auto m=ti.next();setFormat(m.capturedStart(),m.capturedLength(),typeFormat);}
  QRegularExpression strings("\"(?:\\\\.|[^\"\\\\])*\"|'(?:\\\\.|[^'\\\\])*'");auto si=strings.globalMatch(text);while(si.hasNext()){auto m=si.next();setFormat(m.capturedStart(),m.capturedLength(),stringFormat);}
  QRegularExpression numbers("\\b(?:0x[0-9A-Fa-f]+|\\d+(?:\\.\\d+)?)\\b");auto ni=numbers.globalMatch(text);while(ni.hasNext()){auto m=ni.next();setFormat(m.capturedStart(),m.capturedLength(),numberFormat);}
  const int commentStart=python?text.indexOf('#'):text.indexOf("//");
  if(commentStart>=0)setFormat(commentStart,text.size()-commentStart,commentFormat);
  if(!python){const int block=text.indexOf("/*");if(block>=0){const int end=text.indexOf("*/",block+2);setFormat(block,end<0?text.size()-block:end+2-block,commentFormat);setCurrentBlockState(end<0?1:0);}else if(previousBlockState()==1){const int end=text.indexOf("*/");setFormat(0,end<0?text.size():end+2,commentFormat);setCurrentBlockState(end<0?1:0);}}
  QRegularExpression funcs("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*\\()");auto fi=funcs.globalMatch(text);while(fi.hasNext()){auto m=fi.next();setFormat(m.capturedStart(),m.capturedLength(),functionFormat);}
 }
private:
 QString lang="plain";QTextCharFormat keywordFormat,typeFormat,stringFormat,commentFormat,numberFormat,functionFormat;
};

EditorWidget::EditorWidget(QWidget *p):QWidget(p){
 auto *v=new QVBoxLayout(this);auto *bar=new QHBoxLayout;
 auto *n=new QPushButton("New");auto *o=new QPushButton("Open");auto *folder=new QPushButton("Open Folder");auto *s=new QPushButton("Save");auto *sa=new QPushButton("Save as");auto *find=new QPushButton("Find");auto *replace=new QPushButton("Replace all");pathLabel=new QLabel("Untitled");
 bar->addWidget(n);bar->addWidget(o);bar->addWidget(folder);bar->addWidget(s);bar->addWidget(sa);bar->addWidget(find);bar->addWidget(replace);bar->addStretch();bar->addWidget(pathLabel);v->addLayout(bar);
 auto *splitter=new QSplitter(Qt::Horizontal);
 fileModel=new QFileSystemModel(this);fileModel->setRootPath(QDir::homePath());fileTree=new QTreeView;fileTree->setModel(fileModel);fileTree->setRootIndex(fileModel->index(QDir::homePath()));fileTree->setHeaderHidden(true);fileTree->setColumnWidth(0,220);fileTree->setMinimumWidth(170);splitter->addWidget(fileTree);
 edit=new QPlainTextEdit;edit->setLineWrapMode(QPlainTextEdit::NoWrap);edit->setObjectName("codeEditor");
 QFont font("Consolas");font.setStyleHint(QFont::Monospace);font.setPointSize(10);edit->setFont(font);edit->setTabStopDistance(4*edit->fontMetrics().horizontalAdvance(' '));
 splitter->addWidget(edit);splitter->setStretchFactor(0,0);splitter->setStretchFactor(1,1);splitter->setSizes({240,900});v->addWidget(splitter,1);
 statusLabel=new QLabel("Line 1  ·  Column 1  ·  Plain text");statusLabel->setObjectName("mutedText");v->addWidget(statusLabel);
 highlighter=new CodeHighlighter(edit->document());
 connect(n,&QPushButton::clicked,this,&EditorWidget::newFile);connect(o,&QPushButton::clicked,this,&EditorWidget::openFile);
 connect(folder,&QPushButton::clicked,this,[this]{QString path=QFileDialog::getExistingDirectory(this,"Open project folder",currentPath.isEmpty()?QDir::homePath():QFileInfo(currentPath).absolutePath());if(!path.isEmpty())setProjectFolder(path);});
 connect(fileTree,&QTreeView::doubleClicked,this,[this](const QModelIndex &index){QFileInfo info(fileModel->filePath(index));if(info.isFile())openPath(info.absoluteFilePath());});connect(s,&QPushButton::clicked,this,&EditorWidget::saveFile);connect(sa,&QPushButton::clicked,this,&EditorWidget::saveAs);
 connect(find,&QPushButton::clicked,this,[this]{bool ok=false;const QString term=QInputDialog::getText(this,"Find","Text to find:",QLineEdit::Normal,QString(),&ok);if(!ok||term.isEmpty())return;QTextCursor found=edit->document()->find(term,edit->textCursor());if(found.isNull())found=edit->document()->find(term);if(found.isNull())QMessageBox::information(this,"Find","No matching text was found.");else edit->setTextCursor(found);});
 connect(replace,&QPushButton::clicked,this,[this]{bool ok=false;const QString term=QInputDialog::getText(this,"Replace all","Text to find:",QLineEdit::Normal,QString(),&ok);if(!ok||term.isEmpty())return;const QString replacement=QInputDialog::getText(this,"Replace all","Replace with:",QLineEdit::Normal,QString(),&ok);if(!ok)return;QString text=edit->toPlainText();const int count=text.count(term);if(count==0){QMessageBox::information(this,"Replace all","No matching text was found.");return;}text.replace(term,replacement);edit->setPlainText(text);edit->document()->setModified(true);QMessageBox::information(this,"Replace all",QString("Replaced %1 occurrence(s).").arg(count));});

 connect(edit,&QPlainTextEdit::cursorPositionChanged,this,&EditorWidget::updateStatus);connect(edit->document(),&QTextDocument::modificationChanged,this,[this](bool dirty){setWindowModified(dirty);pathLabel->setText((currentPath.isEmpty()?QString("Untitled"):currentPath)+(dirty?"  •":""));});
 auto shortcut=[this](const QKeySequence &key,auto slot){auto *sc=new QShortcut(key,this);connect(sc,&QShortcut::activated,this,slot);};
 shortcut(QKeySequence::New,&EditorWidget::newFile);shortcut(QKeySequence::Open,&EditorWidget::openFile);shortcut(QKeySequence::Save,&EditorWidget::saveFile);shortcut(QKeySequence("Ctrl+Shift+S"),&EditorWidget::saveAs);
 auto *findShortcut=new QShortcut(QKeySequence::Find,this);connect(findShortcut,&QShortcut::activated,find,&QPushButton::click);
 auto *replaceShortcut=new QShortcut(QKeySequence("Ctrl+H"),this);connect(replaceShortcut,&QShortcut::activated,replace,&QPushButton::click);
}
void EditorWidget::setProjectFolder(const QString &path){
 if(path.isEmpty()||!QDir(path).exists())return;
 fileTree->setRootIndex(fileModel->setRootPath(path));
}
void EditorWidget::openPath(const QString &path){
 if(!maybeSave())return;
 QFile file(path);if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){QMessageBox::warning(this,"Open failed",file.errorString());return;}
 edit->setPlainText(QString::fromUtf8(file.readAll()));currentPath=path;edit->document()->setModified(false);pathLabel->setText(path);applyLanguageHighlighting();updateStatus();
}
bool EditorWidget::maybeSave(){
 if(!edit->document()->isModified())return true;
 auto choice=QMessageBox::warning(this,"Unsaved changes","Save changes before continuing?",QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel,QMessageBox::Save);
 if(choice==QMessageBox::Save){saveFile();return !edit->document()->isModified();}
 return choice==QMessageBox::Discard;
}
void EditorWidget::newFile(){if(!maybeSave())return;edit->clear();currentPath.clear();edit->document()->setModified(false);pathLabel->setText("Untitled");applyLanguageHighlighting();updateStatus();}
void EditorWidget::openFile(){
 if(!maybeSave())return;QString p=QFileDialog::getOpenFileName(this,"Open source file",QString(),"Source files (*.cpp *.h *.hpp *.c *.py *.js *.ts *.html *.css *.json *.xml *.md *.txt *.cmake CMakeLists.txt);;All files (*)");
 if(p.isEmpty())return;openPath(p);
}
bool EditorWidget::writeFile(const QString&p){
 QFile f(p);if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::warning(this,"Save failed",f.errorString());return false;}
 const QByteArray data=edit->toPlainText().toUtf8();if(f.write(data)!=data.size()){QMessageBox::warning(this,"Save failed",f.errorString());return false;}
 currentPath=p;edit->document()->setModified(false);pathLabel->setText(p);applyLanguageHighlighting();return true;
}
void EditorWidget::saveFile(){if(currentPath.isEmpty()){saveAs();return;}writeFile(currentPath);}
void EditorWidget::saveAs(){QString p=QFileDialog::getSaveFileName(this,"Save source file",currentPath);if(!p.isEmpty())writeFile(p);}
void EditorWidget::applyLanguageHighlighting(){
 QString ext=currentPath.section('.',-1).toLower();QString lang="plain";
 if(ext=="py")lang="python";else if(ext=="cpp"||ext=="h"||ext=="hpp"||ext=="c"||ext=="js"||ext=="ts"||ext=="java"||ext=="cs"||ext=="html"||ext=="css"||ext=="json"||ext=="xml")lang="code";
 static_cast<CodeHighlighter*>(highlighter)->setLanguage(lang);
}
void EditorWidget::updateStatus(){
 QTextCursor c=edit->textCursor();QString ext=currentPath.section('.',-1).toLower();QString lang=ext.isEmpty()?"Plain text":ext.toUpper();
 statusLabel->setText(QString("Line %1  ·  Column %2  ·  %3").arg(c.blockNumber()+1).arg(c.positionInBlock()+1).arg(lang));
}
