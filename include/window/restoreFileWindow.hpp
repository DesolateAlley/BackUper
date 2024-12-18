#ifndef RESTOREFILEWINDOW_HPP
#define RESTOREFILEWINDOW_HPP
#include "../AllQtHead.hpp" //所有的Qt头文件


#include "./backupedView.hpp"


class RestoreWindow : public QWidget {
    Q_OBJECT
 public:
    BackUpedViewer * backupedViewer;

    RestoreWindow(QWidget *parent = nullptr);
    void viewButtonClicked();
   
};








#endif
