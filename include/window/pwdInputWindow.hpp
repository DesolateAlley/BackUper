#ifndef PWDINPUTWINDOW_HPP
#define PWDINPUTWINDOW_HPP

#include "../AllQtHead.hpp"

 
// 加密时输入密码
class EncPWDDialog : public QDialog {
    Q_OBJECT
 public:
    EncPWDDialog(QWidget *parent = nullptr) ;
    QString getPassword() const ;

 private slots:
    void onOkClicked();

 private:
    QLineEdit *passwordEdit;
    QLineEdit *confirmEdit;
};
 
 
// 解密时输入密码
class DecPWDDialog : public QDialog {
    Q_OBJECT
 public:
    DecPWDDialog(QWidget *parent = nullptr) ;
    QString getPassword() const ;

 private slots:
    void onOkClicked();

 private:
    QLineEdit *passwordEdit;
};




#endif





