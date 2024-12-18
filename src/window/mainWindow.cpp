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
    this->setWindowTitle("Backup");
    this->resize(800, 600); // 设置窗口大小
    this->move((screenWidth -  this->width()) / 2, (screenHeight - this->height()) / 2); // 设置窗口初始位置 // 计算窗口位置以居中显示 设置窗口初始位置

    // window 的主要层(垂直)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建 FileExplorer 实例并添加到布局
    this->fileExplorer = new FileExplorer(viewDirPath, this);
    mainLayout->addWidget(this->fileExplorer);
    mainLayout->setAlignment(this->fileExplorer, Qt::AlignTop);

    // 设置固定高度的 spacer item
    QSpacerItem *spacer1 = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(spacer1);

    // 添加其他控件
    // 创建水平层，增加功能按钮
    QHBoxLayout *backupLayout = new QHBoxLayout();
    QPushButton *backupButton = new QPushButton("备份", this);
    backupButton->setFixedSize(120,30);
    backupLayout->addWidget(backupButton );
    QPushButton *findBackUpFileButton = new QPushButton("查看已备份的文件", this);
    findBackUpFileButton->setFixedSize(120,30);
    backupLayout->addWidget(findBackUpFileButton);
    //将水平层加入主要层mainLayout
    mainLayout->addLayout(backupLayout );

    // 添加固定高度的 spacer item
    QSpacerItem *spacer2 = new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(spacer2);

    QVBoxLayout *extendFuncLayout = new QVBoxLayout();
    // 添加伸缩空间，推控件到底部
    extendFuncLayout->addStretch(); 
    QLabel *extendFuncLabel = new QLabel("扩展功能区" , this);  
    extendFuncLayout->addWidget(extendFuncLabel, 0, Qt::AlignHCenter );
    // extendFuncLayout->setAlignment(extendFuncLabel, Qt::AlignBottom);
    // 创建水平层，其他扩展功能按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *packButton = new QPushButton("打包", this);
    buttonLayout->addWidget(packButton);
    buttonLayout->setAlignment(packButton, Qt::AlignTop);
    QPushButton *unpackButton = new QPushButton("解包", this);
    buttonLayout->addWidget(unpackButton);
    buttonLayout->setAlignment(unpackButton, Qt::AlignTop);
    QPushButton *compressButton = new QPushButton("压缩", this);
    buttonLayout->addWidget(compressButton);
    buttonLayout->setAlignment(compressButton, Qt::AlignTop);
    QPushButton *uncompressButton = new QPushButton("解压", this);
    buttonLayout->addWidget(uncompressButton);
    buttonLayout->setAlignment(uncompressButton, Qt::AlignTop);
    QPushButton *encryptButton = new QPushButton("加密", this);
    buttonLayout->addWidget(encryptButton);
    buttonLayout->setAlignment(encryptButton, Qt::AlignTop);
    QPushButton *decryptButton = new QPushButton("解密", this);
    buttonLayout->addWidget(decryptButton);
    buttonLayout->setAlignment(decryptButton, Qt::AlignTop);
    QPushButton *deleteButton = new QPushButton("删除", this);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->setAlignment(deleteButton, Qt::AlignTop);
    //将垂直层加入主要层mainLayout
    extendFuncLayout->addLayout(buttonLayout );
    
    mainLayout->addLayout(extendFuncLayout );


    setLayout(mainLayout);

    // 连接信号与槽
    connect(backupButton, &QPushButton::clicked, this, &MainWindow::backupButtonClicked);   //备份按钮
    connect(findBackUpFileButton, &QPushButton::clicked, this, &MainWindow::findBackupFileButtonClicked);   //查看备份文件按钮
    connect(packButton, &QPushButton::clicked, this, &MainWindow::packButtonClicked);   //打包按钮
    connect(unpackButton, &QPushButton::clicked, this, &MainWindow::unpackButtonClicked);   //解包按钮
    connect(compressButton, &QPushButton::clicked, this, &MainWindow::compressButtonClicked);   //压缩按钮
    connect(uncompressButton, &QPushButton::clicked, this, &MainWindow::uncompressButtonClicked);   //解压按钮
    connect(encryptButton, &QPushButton::clicked, this, &MainWindow::encryptButtonClicked);   //加密按钮
    connect(decryptButton, &QPushButton::clicked, this, &MainWindow::decryptButtonClicked);   //解密按钮
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteButtonClicked);   //解密按钮

     
}


void MainWindow::backupButtonClicked() {
    // QMessageBox::warning(nullptr, "Error", "备份"); 
    // 检查有没有选中文件
    std::string SelectedFile = this->fileExplorer -> curSelectedFile .toStdString();;
    if( SelectedFile == ""){
        // 未选中文件,报错
        QMessageBox::warning(nullptr, "Notice", "No file selected.\n未选中文件."); 
    }else{
        // 得到了需要备份的文件 string SelectedFile
        if(Backuper::backupFile(SelectedFile))QMessageBox::information(nullptr,"Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" Backup successfully.\n备份成功."); 
        else QMessageBox::warning(nullptr, "Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" Backup failed.\n备份失败."); 
          
    }
    // 调用函数 ,输入选中文件的路径
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

// 打包按钮点击
void MainWindow::packButtonClicked(){
    QString SelectedFile = this->fileExplorer-> curSelectedFile;
    if( SelectedFile == ""){
        // 未选中文件,报错
        QMessageBox::warning(nullptr, "Notice", "No file selected.\n未选中文件."); 
    }else{
        QFileInfo fileInfo(SelectedFile);
        // 检查路径是否是一个目录
        if (!fileInfo.isDir()) {
            QMessageBox::warning(nullptr, "Invalid Dirname", "This is not a directory file");
            return ;
        } 
        //  SelectedFile 是选中的文件
        QString parentDir = QFileInfo(SelectedFile).absolutePath(); //选中待打包文件的所在目录
        QDir dir(parentDir);
        QString fileName = QFileInfo(SelectedFile).fileName();  // 获取待打包文件的文件名
        QString newFileName="" ;
        // std::cout<<parentDir.toStdString() +" , " +  fileName.toStdString() <<std::endl; 
        // 下面获取打包后文件名
        do{
            // 弹出对话框让用户重命名
            bool ok;
            newFileName = QInputDialog::getText(nullptr, "Package file name",  "请输入文件名 , 否则采用默认名", 
                                                     QLineEdit::Normal, fileName, &ok);
            if (ok && !newFileName.isEmpty()) {
                // 检查新的文件名是否合法
                if (MainWindow::isValidFileName(newFileName)) {
                    newFileName = newFileName  ;
                } else {
                    QMessageBox::warning(nullptr, "Invalid Filename", "The filename contains invalid characters like '/' or other forbidden characters.");
                    continue;  // 继续让用户重新输入
                }
            } else if(ok && newFileName.isEmpty() ) continue;
            else return;
        }while ( !MainWindow::isValidFileName(newFileName) || dir.exists(newFileName+ QString::fromStdString(packSuffix))) ;
        
        // 打包
        if(Packer::packDir(SelectedFile.toStdString() , parentDir.toStdString() +"/"+ newFileName.toStdString() )){
            // 成功
            QMessageBox::information(nullptr, "Notice","pack successfully.\n打包成功."); 
        }else QMessageBox::warning(nullptr, "Notice","pack failed.\n打包失败."); 
    }
    // 调用函数 ,输入选中文件的路径
}
// 解包按钮点击
void MainWindow::unpackButtonClicked(){
    // QMessageBox::information(nullptr, "Error", "解包"); 
    QString SelectedFile = this->fileExplorer-> curSelectedFile;
    if( SelectedFile == ""){
        // 未选中文件,报错
        QMessageBox::warning(nullptr, "Notice", "No file selected.\n未选中文件."); 
    }else{
        // 需要待解包文件 sourcePackFile 以及 解包后的包存放路径
        //  SelectedFile 是选中的文件
        QFileInfo fileInfo(SelectedFile);
        QString parentDir = fileInfo.absolutePath(); //选中待打包文件的所在目录
        QString fileName = fileInfo.fileName();  // 获取待打包文件的文件名
        QString fileSuffix = fileInfo.suffix();  // 获取文件的后缀
        QString baseName = fileInfo.baseName(); // 获取不带后缀的文件名
        QDir dir(parentDir);
        QString newFileName="" ;
        if( ("."+fileSuffix)!= QString::fromStdString(packSuffix)  ){
            QMessageBox::warning(nullptr, "Notice", "当前解包文件格式不符."); 
            return ;
        }
        
        QString targetDir;
        // 下面获取解包后文件名
        // 弹出对话框让用户重命名
        do{ // 获取用户的重命名，检查文件名格式和是否空，暂不检查存在与否
            bool ok;
            newFileName = QInputDialog::getText(nullptr, "Package file name",  "请输入文件名 , 否则采用默认名", 
                                                     QLineEdit::Normal, baseName, &ok);
            if (ok && !newFileName.isEmpty()) {
                // 检查新的文件名是否合法
                if (MainWindow::isValidFileName(newFileName)) {
                    newFileName = newFileName  ;
                } else {
                    QMessageBox::warning(nullptr, "Invalid Filename", "The filename contains invalid characters like '/' or other forbidden characters.");
                    continue;  // 继续让用户重新输入
                }
            } else if(ok && newFileName.isEmpty() ) continue;
            else return;
        }while ( !MainWindow::isValidFileName(newFileName) || newFileName.isEmpty()   ) ;
        // 如果文件名已经存在了 ，就需要重新输入了
        while ( !MainWindow::isValidFileName(newFileName) || dir.exists( newFileName ) ) {
            bool ok;
            newFileName = QInputDialog::getText(nullptr, "Package file name",  "文件名有重复，请重新输入：", 
                                                     QLineEdit::Normal, newFileName, &ok);
            if (ok && !newFileName.isEmpty()) {
                // 检查新的文件名是否合法
                if (MainWindow::isValidFileName(newFileName)) {
                    newFileName = newFileName  ;
                } else {
                    QMessageBox::warning(nullptr, "Invalid Filename", "The filename contains invalid characters like '/' or other forbidden characters.");
                    continue;  // 继续让用户重新输入
                }
            } else if(ok && newFileName.isEmpty() ) continue;
            else return;                                            
        }
        
        targetDir = parentDir +"/" + newFileName ;
        // QMessageBox::warning(nullptr, "Notice",SelectedFile +" , " + targetDir); 
        
        // 解包
        if(Packer::unpackBag(SelectedFile.toStdString() , targetDir.toStdString())){
            // 成功
            QMessageBox::information(nullptr, "Notice","Unpack successfully.\n解包成功."); 
        }else QMessageBox::warning(nullptr, "Notice","Unpack failed.\n解包失败."); 
    }
}





// 压缩按钮点击
void MainWindow::compressButtonClicked(){
    QMessageBox::information(nullptr, "Error", "压缩"); 

}
// 解压按钮点击
void MainWindow::uncompressButtonClicked(){
    QMessageBox::information(nullptr, "Error", "解压"); 

}
// 加密按钮点击
void MainWindow::encryptButtonClicked(){
    QMessageBox::information(nullptr, "Error", "加密"); 

}
// 解密按钮点击    
void MainWindow::decryptButtonClicked(){
    QMessageBox::information(nullptr, "Error", "解密"); 
    

}
// 删除文件按钮点击
void MainWindow::deleteButtonClicked(){
    // QMessageBox::information(nullptr, "Error", "删除"); 
    std::string  SelectedFile = this->fileExplorer-> curSelectedFile.toStdString();
    if( SelectedFile == ""){
        // 未选中文件,报错
        QMessageBox::warning(nullptr, "Notice", "No file selected.\n未选中文件."); 
    }else{
        if(FileTools::rmDirOrFile( SelectedFile , false))
            QMessageBox::information(nullptr,"Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" delete successfully.\n删除成功."); 
        else QMessageBox::warning(nullptr, "Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" delete failed.\n删除失败."); 
    }
}