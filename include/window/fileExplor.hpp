#ifndef FILEEXPLOR_HPP  // fileExplor.hpp
#define FILEEXPLOR_HPP
#include "../AllQtHead.hpp" //所有的Qt头文件
#include "../fileutils.hpp"

class FileExplorer : public QWidget {
    Q_OBJECT
 private:
    
    QLineEdit *pathInput;
    QFileSystemModel *model;
    QTreeView *treeView;
    QLabel *cur_path_label;

 public:
    QString  curViewPath;
    QString  curSelectedFile ;
    QLabel *cur_selectedFile_label;
    
    void setcurSelectedFile(QString Qstr); // 设置 curSelectedFile 的值

    FileExplorer(const QString &directoryPath , QWidget *parent = nullptr) ;  //  : QWidget(parent)
    ~FileExplorer(){};
 signals:
    void curSelectedFileChanged(bool status); // 自定义信号，通知 curSelectedFile 值的变化
 private slots:
    void refresh(); // Enter按钮的点击函数, 确认输入的路径
    void upDirPathView(); // Up按钮的点击函数 , 显示当前目录路径的上级目录

    // treeview查看文件的功能函数
    void onFileClicked(const QModelIndex &index) ; 
    void onFileDoubleClicked(const QModelIndex &index);
    void FileExpanded(const QModelIndex &index);

};

#endif

