#include "../../include/window/mainWindow.hpp"


MainWindow::MainWindow(QWidget *parent ) : QWidget(parent) {

    // 获取屏幕信息
    QScreen *screen = QGuiApplication::primaryScreen(); // 获取主屏幕
    if (!screen){
        QMessageBox::warning(nullptr, "Error", "无法获取屏幕信息"); 
        exit(0);
    }    

    int screenWidth = screen->geometry().width(); // 屏幕宽度
    int screenHeight = screen->geometry().height(); // 屏幕高度
    this->setWindowTitle("EchoBack");
    this->resize(800, 600); // 设置窗口大小
    this->move((screenWidth -  this->width()) / 2, (screenHeight - this->height()) / 2); // 设置窗口初始位置 // 计算窗口位置以居中显示 设置窗口初始位置
    this->setStyleSheet("background-color: rgba(242, 255, 248, 1)"); //应用背景颜色

    // window 的主要层(垂直)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // 设置固定高度的 spacer item
    QSpacerItem *spacer1 = new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(spacer1);
    // 创建 FileExplorer 实例
    this->fileExplorer = new FileExplorer(viewDirPath, this);
    


    // 显示当前选中的文件
    QHBoxLayout *selectfileLayout = new QHBoxLayout();
        // 创建容器小部件
    QFrame  *SFcontainer = new QFrame(this);  // 用来设置背景颜色
        // 展示当前选中的文件
    this->cur_selectedFile_label = new QLabel("当前选中的文件为："+this->fileExplorer->curSelectedFile , this);
    this->cur_selectedFile_label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);// 设置文本可以被选中、复制
    selectfileLayout->addWidget( this->cur_selectedFile_label  , Qt::AlignLeft );
    selectfileLayout->setAlignment(this->cur_selectedFile_label, Qt::AlignLeft );
        // 添加删除文件按钮至 selectfileLayout
    QPushButton *deleteButton = new QPushButton("删除", this);
    deleteButton->setFixedSize(80,30);
    selectfileLayout->addWidget(deleteButton);
    selectfileLayout->setAlignment(deleteButton, Qt::AlignRight);
        // 将布局设置到容器小部件上
    SFcontainer->setLayout(selectfileLayout);
    SFcontainer->setFrameShape(QFrame::StyledPanel);  // 设置框架形状
    SFcontainer->setFrameShadow(QFrame::Raised);      // 设置框架阴影
    SFcontainer->setStyleSheet("background-color:rgba(253, 250, 239, 1);" );   // 设置容器的背景颜色
    mainLayout->addWidget(SFcontainer ); //将容器加入垂直层
    // mainLayout->addLayout(selectfileLayout );

    // 设置固定高度的 spacer item
    QSpacerItem *spacer2 = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(spacer2);

    // 创建垂直层，增加功能按钮
    QVBoxLayout *backupLayout = new QVBoxLayout();
    // 添加固定高度的 spacer item
    QSpacerItem *spacer4 = new QSpacerItem(0, 3, QSizePolicy::Minimum, QSizePolicy::Fixed);
    // backupLayout->addSpacerItem(spacer4);
    //备份选项层
    QHBoxLayout *optionLayout = new QHBoxLayout();
    QLabel *optLabel = new QLabel("备份选项:" , this);  
    optLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);// 设置文本可以被选中、复制
    optionLayout->addWidget(optLabel);
    optionLayout->setAlignment(optLabel ,Qt::AlignCenter);
    // 创建单选框和按钮
    packCheckBox     = new QCheckBox("打包", this);
    compressCheckBox = new QCheckBox("压缩", this);
    encryptCheckBox  = new QCheckBox("加密", this);
    optionLayout->addWidget(packCheckBox     );
    optionLayout->addWidget(compressCheckBox );
    optionLayout->addWidget(encryptCheckBox  );
    optionLayout->setAlignment(packCheckBox     ,Qt::AlignCenter);
    optionLayout->setAlignment(compressCheckBox ,Qt::AlignCenter);
    optionLayout->setAlignment(encryptCheckBox  ,Qt::AlignCenter);
    // 添加固定宽度的 spacer item
    QSpacerItem *spacer3 = new QSpacerItem(370, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
    optionLayout->addSpacerItem(spacer3);
    // optionLayout->setSpacing(0); // 控件间距
    // 设置初始状态
    packCheckBox    ->setChecked(true);
    compressCheckBox->setChecked(false);
    encryptCheckBox ->setChecked(false);
    // 连接第一个复选框的状态变化信号，packRadio为false时，其他两个无法选中
    connect(packCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        // if(!checked){
        //     packCheckBox->setChecked(true);
        //     checked = true ;
        //     QMessageBox::information(this, "重大通知", "想取消勾选吗？嘿嘿，取消不了!\n必须给我打包备份！(～￣▽￣)～");
        // } 
        compressCheckBox->setEnabled(checked);
        encryptCheckBox ->setEnabled(checked);
        // 当取消选中时，强制选项 2 和选项 3 为未选中
        if (!checked) {
            compressCheckBox->setChecked(false);
            encryptCheckBox ->setChecked(false);
        }
    });
    backupLayout->addLayout(optionLayout ); //将备份选项层(水平)加入垂直层

    // 添加固定高度的 spacer item
    QSpacerItem *spacer5 = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);
    backupLayout->addSpacerItem(spacer5);

    //备份按钮水平层
    QHBoxLayout *backButtonLayout = new QHBoxLayout();
        //备份按钮
    QPushButton *backupButton = new QPushButton("备份", this);
    backupButton->setFixedSize(100,30);
    backButtonLayout->addWidget(backupButton );
    backButtonLayout->setAlignment(backupButton, Qt::AlignCenter);
        // 查看备份文件按钮
    QPushButton *findBackUpFileButton = new QPushButton("已备份文件", this);
    findBackUpFileButton->setFixedSize(100,30);
    backButtonLayout->addWidget(findBackUpFileButton);
    backButtonLayout->setAlignment(findBackUpFileButton, Qt::AlignCenter);

    backupLayout->addLayout(backButtonLayout ); //将备份按钮层(水平)加入垂直层

    //将层加入主要层mainLayout
    mainLayout->addLayout(backupLayout );

    
    // 设置固定高度的 spacer item
    QSpacerItem *spacer_1 = new QSpacerItem(0, 30, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(spacer_1);


    // 添加 FileExplorer 到布局
    mainLayout->addWidget(this->fileExplorer);
    mainLayout->setAlignment(this->fileExplorer, Qt::AlignTop);


    setLayout(mainLayout);

    // 连接信号与槽
    connect(backupButton, &QPushButton::clicked, this, &MainWindow::advancedBUButtonClicked);   //备份按钮
    connect(findBackUpFileButton, &QPushButton::clicked, this, &MainWindow::findBackupFileButtonClicked);   //查看备份文件按钮
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteButtonClicked);   //删除按钮
    
    // 连接子控件 fileExplorer 的 curSelectedFileChanged 信号到父控件的槽
    connect( fileExplorer, &FileExplorer::curSelectedFileChanged, this, &MainWindow::onCurSelectedFileChanged);

}

// 文件展示控件的当前所选文件值的变化信号 所对应的槽函数
void MainWindow::onCurSelectedFileChanged(bool status){
    if(status){
        this->cur_selectedFile_label->setText("当前选中的文件为："+this->fileExplorer->curSelectedFile  ); // 更新目录的时候重置选中的文件
    }
}


//备份
void MainWindow::advancedBUButtonClicked(){
    // QMessageBox::warning(nullptr, "Error", "高级备份"); 
    bool ifpack     = packCheckBox->isChecked();
    bool ifcompress = compressCheckBox ->isChecked();
    bool ifencrypt  = encryptCheckBox  ->isChecked() ;
    
    std::string SelectedFile = this->fileExplorer -> curSelectedFile .toStdString();;
    if( SelectedFile == ""){
        // 未选中文件,报错
        QMessageBox::warning(nullptr, "Notice", "No file selected.\n未选中文件."); 
        return ;
    }
    // 判断 SelectedFile 是不是 DefaultBackupPath(软件备份的工作路径) 下的，
    if(FileUtils::isSubPath(SelectedFile , DefaultBackupPath)){
        QMessageBox::warning(nullptr, "Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+
                            " Backup failed.\n备份失败，所选文件为软件备份工作目录内！不允许备份"); 
        return ;
    }
    //// 下面进行备份


    // 得到了需要备份的文件 string SelectedFile
    if(!ifpack){ // 不打包，使用普通的备份
        if(EchoBackup::advancedBackup(SelectedFile , ifpack=false, ifcompress=false , ifencrypt=false  ))
            QMessageBox::information(nullptr,"Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" Backup successfully.\n备份成功."); 
        else QMessageBox::warning(nullptr, "Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" Backup failed.\n备份失败."); 
     }else{
        // 打包，考虑是否压缩、加密
        QString password="" ;
        if(ifencrypt){ // 需要加密的话，输入密码
            EncPWDDialog dialog(this);
            if (dialog.exec() == QDialog::Accepted)password = dialog.getPassword();
            else return;
        }
        // 备份
        if(EchoBackup::advancedBackup(SelectedFile , ifpack, ifcompress , ifencrypt , password.toStdString() )){
            QMessageBox::information(nullptr,"Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" Backup successfully.\n备份成功."); 
        }else QMessageBox::warning(nullptr, "Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" Backup failed.\n备份失败."); 
    }
}

// 查看备份文件按钮点击
void MainWindow::findBackupFileButtonClicked(){
    // 实例化新窗口，并显示
    RestoreWindow *restoreWindow = new RestoreWindow();
    restoreWindow->show();

}

// 检查文件名是否合法
bool  MainWindow::isValidFileName(const QString &fileName) {
    // 文件名不能包含"/"等非法字符
    QRegularExpression regex("^[^/]*$");
    return regex.match(fileName).hasMatch();
}

// 删除文件按钮点击
void MainWindow::deleteButtonClicked(){
    // QMessageBox::information(nullptr, "Error", "删除"); 
    std::string  SelectedFile = this->fileExplorer-> curSelectedFile.toStdString();
    if( SelectedFile == ""){
        // 未选中文件,报错
        QMessageBox::warning(nullptr, "Notice", "No file selected.\n未选中文件."); 
    }else{
        if(FileUtils::rmDirOrFile( SelectedFile , false)){
            this->fileExplorer-> curSelectedFile = "";
            this->cur_selectedFile_label->setText("当前选中的文件为："+ this->fileExplorer->curSelectedFile ); // 更新目录的时候重置选中的文件
            QMessageBox::information(nullptr,"Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" delete successfully.\n删除成功."); 
            
        }else QMessageBox::warning(nullptr, "Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" delete failed.\n删除失败."); 
    }
}