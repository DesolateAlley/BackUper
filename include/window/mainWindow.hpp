#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include "../AllQtHead.hpp" //所有的Qt头文件

#include "./fileExplor.hpp"
#include "./restoreFileWindow.hpp"
#include "./pwdInputWindow.hpp"

#include "../backup/echoBackup.hpp"
#include "../pack/echoPack.hpp"




class MainWindow : public QWidget {
 public:
    FileExplorer *fileExplorer ;
    QCheckBox *packCheckBox     ; // 备份选项
    QCheckBox *compressCheckBox ; // 备份选项
    QCheckBox *encryptCheckBox  ; // 备份选项

    QLabel *cur_selectedFile_label; // 控件:展示当前选择的文件

    MainWindow(QWidget *parent = nullptr);

    // 备份按钮点击
    void backupButtonClicked() ;
    // 备份按钮点击，高级备份
    void advancedBUButtonClicked();
    // 查看备份文件按钮点击
    void findBackupFileButtonClicked();
    
    // 删除文件按钮点击
    void deleteButtonClicked();
    // 检查文件名是否合法
    bool isValidFileName(const QString &fileName);

    // 选中文件 的值的变化检测
    void onCurSelectedFileChanged(bool status); 

    
};
  


#endif