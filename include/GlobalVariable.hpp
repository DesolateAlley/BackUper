#ifndef GLOBALVARIABLE_HPP  
#define GLOBALVARIABLE_HPP
/**
 * 这个头文件用于 extern 所有的全局变量
 */
#include <QString>
#include <string>
#include <unordered_map>


extern QString viewDirPath;
// 默认的备份文件存储路径
extern std::string DefaultBackupPath ;
// 所有的文件备份总存储
extern std::string DefaultBUFilesName;
// 记录所有文件备份信息的, 用于区分每次备份的是不是已经备份过，以及区分不同文件的备份
extern std::string DefaultBUInfoName ; 



// 每个文件备份文件夹里的记录，用于记录每次备份的信息
extern std::string DefaultBackupRecord ;


//打包时加的默认后缀
extern std::string  packSuffix ;
// 不同的备份种类的后缀添加
extern std::unordered_map<std::string, std::string> BUKindsSuffix;





#endif