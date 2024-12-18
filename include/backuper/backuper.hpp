#ifndef BACKUPER_HPP
#define BACKUPER_HPP
#include "../AllQtHead.hpp"

#include "../filetools.hpp"




class Backuper : public FileTools{
 public:
    Backuper(){};
    ~Backuper(){};

    
    // 读取备份存储目录的 BackUpInfo 文件 ,并返回以及备份的文件信息列表
    static std::optional<std::vector< std::unique_ptr<BackUpInfo> > >  readBUInfo(std::string path);
    // 备份文件, 备份到默认路径 DefaultBackupPath 
    static bool backupFile(std::string sourceFile  ) ;


    // 兼容性移动文件或目录
    bool ADmoveFileOrDir(std::string source, std::string target);    
    std::vector<std::string> listFilesInDirectory(const std::string& path); // 列出目录下的所有文件

    // 比较两个目录不同之处
    void compareDirectories(const std::string& dir1, const std::string& dir2);     

};





#endif

