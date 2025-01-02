#include "../../include/window/restoreFileWindow.hpp"


RestoreWindow::RestoreWindow(QWidget *parent ) : QWidget(parent) {

    // 获取屏幕信息
    QScreen *screen = QGuiApplication::primaryScreen(); // 获取主屏幕
    if (!screen){
        QMessageBox::warning(nullptr, "Error", "无法获取屏幕信息"); 
        exit(0);
    }    
    int screenWidth = screen->geometry().width(); // 屏幕宽度
    int screenHeight = screen->geometry().height(); // 屏幕高度
    this->setWindowTitle("Restore");
    this->setStyleSheet("background-color: rgba(242, 255, 248, 1)"); //应用背景颜色
    this->resize(700, 400); // 设置窗口大小
    this->move((screenWidth -  this->width()) / 2, (screenHeight - this->height()) / 2); // 设置窗口初始位置 // 计算窗口位置以居中显示 设置窗口初始位置


    // window 的主要层(垂直)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);


    // 创建 BackUpedViewer 实例并添加到布局
    this->backupedViewer = new BackUpedViewer( QString::fromStdString(DefaultBackupPath+"/"+DefaultBUInfoName) , this);
    mainLayout->addWidget(this->backupedViewer);
    mainLayout->setAlignment(this->backupedViewer , Qt::AlignTop);



    // 创建查看按钮和取消按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *viewButton = new QPushButton("查看备份点", this);
    viewButton->setFixedSize(120,30);
    buttonLayout->addWidget(viewButton );
    QPushButton *cancelButton = new QPushButton("取消", this);
    cancelButton->setFixedSize(120,30);
    buttonLayout->addWidget(cancelButton);
    //将水平层加入主要层mainLayout
    mainLayout->addLayout(buttonLayout );


    setLayout(mainLayout);

    // 连接信号与槽
    connect(viewButton, &QPushButton::clicked, this, &RestoreWindow::viewButtonClicked);
    // 连接按钮点击信号到窗口的 close 槽
    connect(cancelButton, &QPushButton::clicked, this, &QWidget::close);
}

void RestoreWindow::viewButtonClicked(){
    // 选中的文件名 this->backupedViewer->selectRenameDir ;
    // QMessageBox::information(this, "Selected Row Data", "You selected: " + this->backupedViewer->selectRenameDir);
    if( this->backupedViewer->selectDirName == ""){
        // 未选中文件,报错
        QMessageBox::warning(nullptr, "Notice", "No  selected.\n未选中备份点."); 
        return ;
    }
    QString  viewDirPath = QString::fromStdString(DefaultBackupPath + "/" + DefaultBUFilesName ) +"/"+ this->backupedViewer->selectRenameDir ;

    // 实例化新窗口，并显示
    BackUpVersionViewer *BUVersionViewer = new BackUpVersionViewer( viewDirPath  , this->backupedViewer->selectDirName );
    BUVersionViewer->show();



}
