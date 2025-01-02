#include "../../include/window/pwdInputWindow.hpp"
 
EncPWDDialog::EncPWDDialog(QWidget *parent ) : QDialog(parent) {
    setWindowTitle("压缩密码确认");
    // 创建控件
    QLabel *passwordLabel = new QLabel("请输入密码:");
    QLabel *confirmLabel = new QLabel("确认密码:");
    passwordEdit = new QLineEdit;
    confirmEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);  // 设置密码框
    confirmEdit->setEchoMode(QLineEdit::Password);   // 设置密码框
    QPushButton *okButton = new QPushButton("确认");
    QPushButton *cancelButton = new QPushButton("取消");
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(confirmLabel);
    mainLayout->addWidget(confirmEdit);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    // 连接信号和槽
    connect(okButton, &QPushButton::clicked, this, &EncPWDDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &EncPWDDialog::reject);
}

QString EncPWDDialog::getPassword() const {
    return passwordEdit->text();
}

void EncPWDDialog:: onOkClicked() {
    if(passwordEdit->text() ==""){
        QMessageBox::warning(this, "错误", "输入密码为空！");
    }else if (passwordEdit->text() != confirmEdit->text()) {
        QMessageBox::warning(this, "错误", "两次输入的密码不一致，请重新输入！");
    } else {
        accept();  // 密码一致，接受对话框
        // done(QDialog::Accepted);  // 使用 done() 关闭对话框，结束并传递状态码
    }
}

  
 
DecPWDDialog::DecPWDDialog(QWidget *parent ) : QDialog(parent) {
    setWindowTitle("解压密码确认");
    // 创建控件
    QLabel *passwordLabel = new QLabel("请输入密码:");
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);  // 设置密码框
    QPushButton *okButton = new QPushButton("确认");
    QPushButton *cancelButton = new QPushButton("取消");
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    // 连接信号和槽
    connect(okButton, &QPushButton::clicked, this, &DecPWDDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &DecPWDDialog::reject);
}

QString DecPWDDialog::getPassword() const {
    return passwordEdit->text();
}

void DecPWDDialog:: onOkClicked() {
    if (passwordEdit->text() == "") {
        QMessageBox::warning(this, "错误", "输入密码为空！");
    } else {
        accept();  // 密码一致，接受对话框
        // done(QDialog::Accepted);  // 使用 done() 关闭对话框，结束并传递状态码
    }
}

  
