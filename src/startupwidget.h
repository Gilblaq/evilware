#pragma once
#include <QWidget>
#include <QString>
class QLineEdit;
class QLabel;
class QPushButton;
class SplashScreen : public QWidget {
 Q_OBJECT
public:
 explicit SplashScreen(QWidget *parent=nullptr);
signals:
 void finished();
protected:
 void paintEvent(QPaintEvent *) override;
private:
 int tick=0;
};
class AccessScreen : public QWidget {
 Q_OBJECT
public:
 explicit AccessScreen(QWidget *parent=nullptr);
signals:
 void enterRequested(const QString &displayName);
protected:
 void paintEvent(QPaintEvent *) override;
private:
 QLineEdit *name;
 QLabel *heading;
 QPushButton *loginTab;
 QPushButton *signupTab;
 bool signup=false;
 void setMode(bool createAccount);
};
class SkullBackdrop : public QWidget {
 Q_OBJECT
public:
 explicit SkullBackdrop(QWidget *parent=nullptr);
protected:
 void paintEvent(QPaintEvent *) override;
};
