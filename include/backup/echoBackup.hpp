#ifndef ECHOBACKUP_HPP
#define ECHOBACKUP_HPP
#include "../AllQtHead.hpp"

#include "../fileutils.hpp"
#include "../pack/echoPack.hpp"
#include "../compress/echoCompress.hpp"
#include "../encrypt/echoEncrypt.hpp"



class EchoBackup : public FileUtils{
 public:
    EchoBackup(){};
    ~EchoBackup(){};

    
    // 读取备份存储目录的 BackUpInfo 文件 ,并返回以及备份的文件信息列表
    static std::optional<std::vector< std::unique_ptr<BackUpInfo> > >  readBUInfo(std::string path);
    // 备份文件, 备份到默认路径 DefaultBackupPath 
    static bool backupFile(std::string sourceFile  ) ;

    // 备份前初始化，第一次备份的话写备份信息，新建备份存储文件夹，否则获取该文件的备份信息
    static std::unique_ptr<BackUpInfo> backupInit(std::string sourceFile);
    static bool advancedBackup(std::string sourceFile, bool ifpack=true , bool ifcompress=false  , bool ifencrypt =false ,std::string password ="123456");

    // 备份文件的还原 ， 函数内区分不同的备份方式，对应不同的还原方法
    static bool restoreFile(std::string sourceDir, std::string filename, std::string targetfile ,std::string password="123456");


};





#endif

