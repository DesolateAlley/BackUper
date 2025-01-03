#include "./include/AllQtHead.hpp" //所有的Qt头文件
#include "./include/fileutils.hpp"
#include "./include/window/mainWindow.hpp"


// 路径
QString viewDirPath = "/home/cl/Desktop/EchoBack_test" ;
// 默认的备份文件存储工作总路径
std::string DefaultBackupPath = "/home/cl/AAA_EchoBack/EchoBack_BackupDir" ;   //暂不支持自定义
// 所有的文件备份总存储
std::string DefaultBUFilesName = "BackUpFiles" ;
// 记录所有文件备份信息的, 用于区分每次备份的是不是已经备份过，以及区分不同文件的备份
std::string DefaultBUInfoName  = "BackUpInfo.info" ;

// 每个文件备份文件夹里的记录，用于记录每次备份的信息
std::string DefaultBackupRecord = "BackUpRecord.EchoBack" ;
//打包时加的默认后缀
std::string  packSuffix = ".EchoBackPack";
// 不同的备份种类的后缀添加
std::unordered_map<std::string, std::string> BUKindsSuffix = {{"UnPack" , ""},  {"Pack" , ".echoP"},  {"Cmp" , ".echoPC"}, 
                                                            {"Enc" , ".echoPE"},  {"CmpEnc" , ".echoPCE"} };

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 检查备份工作路径
    if (access(DefaultBackupPath.c_str(), F_OK))FileUtils::mkDir(DefaultBackupPath);
    if (access((DefaultBackupPath+"/"+DefaultBUFilesName).c_str(), F_OK))FileUtils::mkDir(DefaultBackupPath+"/"+DefaultBUFilesName);
    if (access((DefaultBackupPath+"/"+DefaultBUInfoName).c_str(), F_OK)){
        std::ofstream outFile(DefaultBackupPath+"/"+DefaultBUInfoName , std::ios::trunc);
        if (!outFile) {
            std::cerr << "Error creating file: " << DefaultBackupPath+"/"+DefaultBUInfoName << std::endl;
            return -1;
        }
        outFile.close(); // 关闭文件
    }


    // 创建程序窗口
    MainWindow window;
    window.show();

    qDebug() << "Qt version:" << qVersion();  // 获取运行时版本
    qDebug() << "Qt compiled version:" << QT_VERSION_STR;  // 获取编译时版本

    return app.exec();
}

// #include "main.moc" // 如果你使用的是 CMake，可能需要配置 CMakeLists.txt 以启用 MOC
