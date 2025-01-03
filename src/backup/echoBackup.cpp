#include "../../include/backup/echoBackup.hpp"

// 读取备份存储目录的 BackUpInfo 文件 ,并返回以及备份的文件信息列表
std::optional<std::vector< std::unique_ptr<BackUpInfo> > > EchoBackup:: readBUInfo(std::string path){
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file: " << path << std::endl;
        // QMessageBox::information(nullptr, "Error", "Unable to open file:"+QString::fromStdString(path)); 
        return std::nullopt; // 表示失败
    }

    std::vector< std::unique_ptr<BackUpInfo> > buInfos ;
    std::string line;
    while (std::getline(inputFile, line)) {
        // 处理每一行
        // std::cout << line << std::endl;
        std::unique_ptr<BackUpInfo> buInfo =std::make_unique<BackUpInfo>();
        std::vector<std::string> parts = buInfo->split(line , "////");
        if(parts.size()!=5){
            std::cout<<"BackUpInfo文件读取错误!"<<std::endl;
            return std::nullopt; // 表示失败
        }
        buInfo->filename = parts[0];
        buInfo->inode = parts[1];
        buInfo->devno = parts[2];
        buInfo->path = parts[3];
        buInfo->BURename = parts[4]; 
        buInfos.push_back(std::move(buInfo));
    }

    inputFile.close(); // 关闭文件

    return buInfos;

}

// 备份文件, 备份到默认路径 DefaultBackupPath 
bool EchoBackup::backupFile(std::string sourceFile  ) {
    // std::cout<<"sb"+sourceFile+DefaultBackupPath<<std::endl; 
    std::string BUInfoFilePath = DefaultBackupPath + "/" +DefaultBUInfoName ;
    //// 判断该文件之前是否备份过,若备份过,在原备份记录上新增备份点
    // 首先获取 sourceFile 的文件信息 , 构建 BackUpInfo 类
    struct stat srcbuf;
    stat(sourceFile.c_str(), &srcbuf);
    std::unique_ptr<BackUpInfo> sourceFileInfo =std::make_unique<BackUpInfo>(sourceFile.substr(sourceFile.find_last_of("/\\") + 1) , 
                                                        std::to_string(srcbuf.st_ino), std::to_string(srcbuf.st_dev) , sourceFile);
    
    // 读取 BUInfoFilePath : /home/cl/Desktop/Backup_King/BackupDir/ + BackUpInfo 内容
    auto resultOpt = EchoBackup::readBUInfo(BUInfoFilePath) ;
    if(!resultOpt)return false ; 
    std::vector< std::unique_ptr<BackUpInfo> > BUInfos = std::move(*resultOpt) ;
    // 获取 所有已备份文件的备份文件夹重命名
    std::vector<std::string> AllRename ; 
    for (const auto& ptr:BUInfos ) AllRename.push_back(ptr->BURename) ;

    // 查找 sourceFileInfo 是否在 BUInfos 里，即是否备份过
    std::unique_ptr<BackUpInfo> findRecord  = sourceFileInfo->findSameInVec(std::move(BUInfos)) ;
    
    std::string BUFilePath ; // 待备份文件自己的备份存储文件夹
    if (findRecord == nullptr) {  // 没找到，说明该文件是第一次备份
        // BackUpInfo行内容:   文件名////文件inode(st_ino)////文件设备号////源路径////备份重命名
        // 首先在 BackUpInfo 里面写入新的记录，但是此时还有一个属性未定义(UPRename)，在备份目录 DefaultBackupPath 里新开一个自己的备份文件夹 ，这个文件夹名要避免重名
        // 查看本文件的filename在AllRename中有无重名，若有，则需要更改，更改的规则是加在filename文件名后加一个6位的字符串，其内容为字母或数字随机排列
        sourceFileInfo->BURename = sourceFileInfo->filename; //没有命名冲突的话不会进入循环
        while(std::find(AllRename.begin() , AllRename.end() , sourceFileInfo->BURename )!=AllRename.end()){ 
            sourceFileInfo->BURename = sourceFileInfo->filename + FileUtils::getRandomString(); // 有命名冲突，就一直循环
        }
        if(FileUtils::addToFile(BUInfoFilePath , sourceFileInfo->to_string())==false){ // 写入失败
            std::cout<<"备份文件信息写入BackUpInfo失败"<<std::endl;
            return false ;
        }
        // BackUpInfo已经写入记录 ， 下面在/home/cl/Desktop/Backup_King/BackupDir/BackUpFiles 里新建其重命名的BURename文件夹
        BUFilePath = DefaultBackupPath + "/"+DefaultBUFilesName + "/" + sourceFileInfo->BURename ;
        FileUtils filetool ;
        filetool.mkDir(BUFilePath); 
        // 在BUFilePath新建一个备份记录文件 BackUpRecord.hwcq307,用于记录该文件的每次备份
        // 打开文件并使用 trunc 模式清空内容
        std::ofstream outFile(BUFilePath+ "/"+DefaultBackupRecord , std::ios::trunc);
        if (!outFile) {
            std::cerr << "Error creating file: " << BUFilePath + DefaultBackupRecord << std::endl;
            return false;
        }
        outFile.close(); // 关闭文件
        
    } else { // 找到了记录，说明该文件不是第一次备份了 ,在原备份记录上新增备份点
        // std::cout<<"end"<<std::endl;
        BUFilePath = DefaultBackupPath + "/"+DefaultBUFilesName + "/"  + findRecord->BURename ;
        sourceFileInfo ->BURename = findRecord->BURename;
    }

    // 不管找不找到，到这的都是对 sourceFileInfo 进行备份，备份的路径是 BUFilePath , 但是还要知道当前是第几备份点，要查看 备份记录文件 BackUpRecord.hwcq307
    int recordNum= FileUtils::getFileLine(BUFilePath +"/" + DefaultBackupRecord) ;
    if(recordNum<0){ // 读取文件失败
        std::cerr << "Unable to open file: " << BUFilePath + DefaultBackupRecord << std::endl;
        return false; 
    }
    std::string BUVersion = sourceFileInfo ->BURename + "V" + std::to_string(recordNum) ;
    // 先写备份记录文件 其内容为每行是一条备份记录： 备份时的文件名////当前备份的备份文件名（UPRenameVi）////备份时间////备份时的路径
    std::string wdata = sourceFileInfo->filename +"////" + BUVersion+"////" + FileUtils::getCurrentTimeString() + "////" +sourceFile  ;
    
    // 下面进行备份
    if(FileUtils::copyAllFileKinds(sourceFile  , BUFilePath + "/" +BUVersion )){
        FileUtils::addToFile(BUFilePath +"/" + DefaultBackupRecord ,  wdata) ;
        return true;    
    }   
    return false;

}


// 备份前初始化，第一次备份的话写备份信息，新建备份存储文件夹，否则获取该文件的备份信息
std::unique_ptr<BackUpInfo> EchoBackup::backupInit(std::string sourceFile){
    std::string BUInfoFilePath = DefaultBackupPath + "/" + DefaultBUInfoName ;
    //// 判断该文件之前是否备份过,若备份过,在原备份记录上新增备份点
    // 首先获取 sourceFile 的文件信息 , 构建 BackUpInfo 类
    struct stat srcbuf;
    lstat(sourceFile.c_str(), &srcbuf);
    std::unique_ptr<BackUpInfo> sourceFileInfo =std::make_unique<BackUpInfo>(sourceFile.substr(sourceFile.find_last_of("/\\") + 1) , 
                                                        std::to_string(srcbuf.st_ino), std::to_string(srcbuf.st_dev) , sourceFile);
    
    // 读取 BUInfoFilePath : /home/cl/Desktop/Backup_King/BackupDir/ + BackUpInfo 内容
    auto resultOpt = EchoBackup::readBUInfo(BUInfoFilePath) ;
    if(!resultOpt)return nullptr ; 
    std::vector< std::unique_ptr<BackUpInfo> > BUInfos = std::move(*resultOpt) ;
    // 获取 所有已备份文件的备份文件夹重命名
    std::vector<std::string> AllRename ; 
    for (const auto& ptr:BUInfos ) AllRename.push_back(ptr->BURename) ;

    // 查找 sourceFileInfo 是否在 BUInfos 里，即是否备份过
    std::unique_ptr<BackUpInfo> findRecord  = sourceFileInfo->findSameInVec(std::move(BUInfos)) ;
     
    if (findRecord == nullptr) {  // 没找到，说明该文件是第一次备份
        // BackUpInfo行内容:   文件名////文件inode(st_ino)////文件设备号////源路径////备份重命名
        // 首先在 BackUpInfo 里面写入新的记录，但是此时还有一个属性未定义(UPRename)，在备份目录 DefaultBackupPath 里新开一个自己的备份文件夹 ，这个文件夹名要避免重名
        // 查看本文件的filename在AllRename中有无重名，若有，则需要更改，更改的规则是加在filename文件名后加一个6位的字符串，其内容为字母或数字随机排列
        sourceFileInfo->BURename = sourceFileInfo->filename; //没有命名冲突的话不会进入循环
        while(std::find(AllRename.begin() , AllRename.end() , sourceFileInfo->BURename )!=AllRename.end()){ 
            sourceFileInfo->BURename = sourceFileInfo->filename + FileUtils::getRandomString(); // 有命名冲突，就一直循环
        }
        if(FileUtils::addToFile(BUInfoFilePath , sourceFileInfo->to_string())==false){ // 写入失败
            std::cout<<"备份文件信息写入BackUpInfo失败"<<std::endl;
            return nullptr ;
        }
        // BackUpInfo已经写入记录 ， 下面在/home/cl/Desktop/Backup_King/BackupDir/BackUpFiles 里新建其重命名的BURename文件夹
        std::string BUFilePath = DefaultBackupPath +"/"+DefaultBUFilesName + "/" +  sourceFileInfo->BURename ;
        FileUtils filetool ;
        filetool.mkDir(BUFilePath); 
        // 在BUFilePath新建一个备份记录文件 BackUpRecord.hwcq307,用于记录该文件的每次备份
        // 打开文件并使用 trunc 模式清空内容
        std::ofstream outFile(BUFilePath+ "/"+DefaultBackupRecord , std::ios::trunc);
        if (!outFile) {
            std::cerr << "Error creating file: " << BUFilePath +  "/"+ DefaultBackupRecord << std::endl;
            return nullptr;
        }
        outFile.close(); // 关闭文件   
    } else { // 找到了记录，说明该文件不是第一次备份了 ,在原备份记录上新增备份点
        sourceFileInfo ->BURename = findRecord->BURename;
    }
    return  std::move(sourceFileInfo);

}

// 高级备份
bool EchoBackup::advancedBackup(std::string sourceFile, bool ifpack , bool ifcompress , bool ifencrypt ,std::string password){  
    // 备份初始化 
    std::unique_ptr<BackUpInfo> sourceFileInfo = EchoBackup::backupInit(sourceFile);
    if(sourceFileInfo==nullptr)return false ;
    //该文件的全部备份存储目录
    std::string BUFilePath =  DefaultBackupPath + "/"+DefaultBUFilesName + "/" +  sourceFileInfo->BURename ;
    // 但是还要知道当前是第几备份点，要查看 备份记录文件 DefaultBackupRecord(BackUpRecord.hwcq307)
    int recordNum= FileUtils::getFileLine(BUFilePath +"/" + DefaultBackupRecord) ;
    if(recordNum<0){ // 读取文件失败
        std::cerr << "Unable to open file: " << BUFilePath + DefaultBackupRecord << std::endl;
        return false; 
    }
    
    // 具体存储文件的名 ， 以及写入备份记录的 wdata 
    std::string BUVersion , wdata , BUVersionFinal ; 
    BUVersion = sourceFileInfo ->BURename + "V" + std::to_string(recordNum) ;

    // 区分五种情况，不打包、打包、打包压缩、打包加密、打包压缩加密
    if(!ifpack){ // 普通备份
        BUVersion += BUKindsSuffix["UnPack"] ;
        // 先写备份记录文件 其内容为每行是一条备份记录： 备份时的文件名////当前备份的备份文件名（UPRenameVi）////备份时间////备份时的路径
        wdata = sourceFileInfo->filename +"////" + BUVersion+"////" + FileUtils::getCurrentTimeString() + "////" +sourceFile +"////No" ;
        // 下面进行备份
        if(!FileUtils::copyAllFileKinds(sourceFile  , BUFilePath + "/" +BUVersion )){
            std::cout<<"文件复制失败"<<std::endl;
            return false ;
        }   

        // 写备份记录文件 其内容为每行是一条备份记录： 备份时的文件名////当前备份的备份文件名（UPRenameVi）////备份时间////备份时的路径////是否加密了
        if(!FileUtils::addToFile(BUFilePath +"/" + DefaultBackupRecord ,  wdata) ){
            std::cout<<"备份记录写入失败"<<std::endl;
            FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersionFinal  , false); //备份失败了，把之前的记录删掉
            return false ;
        }
    }else{ // 需要打包的
    
        if(!EchoPack::packFile( sourceFile , BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"] )){

            std::cout<<"文件打包失败"<<std::endl;
            FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion  , false); // 打包失败了，把复制的文件删掉
            return false ;
        }
        FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion  , false); // 打包成功了，把复制的文件删掉
        
        
        if(!ifcompress && !ifencrypt){ //只打包
            // 复制&打包成功，写入备份记录
            BUVersionFinal = BUVersion+BUKindsSuffix["Pack"] ;   
        }else if(  ifcompress && !ifencrypt){ //打包且压缩
            // 打包成功了 下面对打包后的文件压缩
            EchoCompress compresser;
            if(! compresser.compress(BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"] , BUFilePath + "/" +BUVersion+BUKindsSuffix["Cmp"] )){
                std::cout<<"文件压缩失败"<<std::endl;
                FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"]  , false); // 压缩失败了，把打包的文件删掉
                return false ;
            }
            FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"]  , false); // 压缩成功了，把打包的文件删掉
            // 复制&打包&压缩成功，写入备份记录
            BUVersionFinal = BUVersion+BUKindsSuffix["Cmp"] ;
            
        }else if( !ifcompress &&  ifencrypt){   //打包且加密
            // 打包成功了 下面对打包后的文件加密
            EchoEncrypt encrypter;
            if(! encrypter.encyptAESFile(BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"] , BUFilePath + "/" +BUVersion+BUKindsSuffix["Enc"] , password  )){
                std::cout<<"文件加密失败"<<std::endl;
                FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"]  , false); // 加密失败了，把打包的文件删掉
                return false ;
            }
            FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"]  , false); // 加密成功了，把打包的文件删掉
            // 复制&打包&加密成功，写入备份记录
            BUVersionFinal = BUVersion+BUKindsSuffix["Enc"] ;

        }else if(  ifcompress &&  ifencrypt){ //打包压缩加加密
            // 打包成功了 下面对打包后的文件压缩
            EchoCompress compresser;
            if(! compresser.compress(BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"] , BUFilePath + "/" +BUVersion+BUKindsSuffix["Cmp"] )){
                std::cout<<"文件压缩失败"<<std::endl;
                FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"]  , false); // 压缩失败了，把打包的文件删掉
                return false ;
            }
            FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"]  , false); // 压缩成功了，把打包的文件删掉
            // 压缩成功了，下面进行加密
            EchoEncrypt encrypter;
            if(! encrypter.encyptAESFile(BUFilePath + "/" +BUVersion+BUKindsSuffix["Cmp"] , BUFilePath + "/" +BUVersion+BUKindsSuffix["CmpEnc"] , password)){
                std::cout<<"文件加密失败"<<std::endl;
                FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Cmp"]  , false); // 加密失败了，把压缩的文件删掉
                return false ;
            }
            FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Cmp"]  , false); // 加密成功了，把压缩的文件删掉
            // 复制&打包&压缩&加密成功，写入备份记录
            BUVersionFinal = BUVersion+BUKindsSuffix["CmpEnc"] ;
        }else {
            std::cout<<"备份选项传参错误"<<std::endl;
            FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"]  , false); // 打包成功了，把复制的文件删掉
            return false;
        }


        // 写备份记录文件 其内容为每行是一条备份记录： 备份时的文件名////当前备份的备份文件名（UPRenameVi）////备份时间////备份时的路径////是否加密了
        wdata = sourceFileInfo->filename +"////" + BUVersionFinal +"////" + FileUtils::getCurrentTimeString() + "////" +sourceFile + "////"  ;
        if(ifencrypt)wdata += "Yes" ;
        else wdata += "No" ;
        if(!FileUtils::addToFile(BUFilePath +"/" + DefaultBackupRecord ,  wdata) ){
            std::cout<<"备份记录写入失败"<<std::endl;
            FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersionFinal  , false); //备份失败了，把之前的记录删掉
            return false ;
        }
    }
    std::cout<<"备份成功！"<<std::endl;
    return true;  
}



// sourceDir 是待还原文件所在目录路径 ， filename是待还原文件的文件名  ， targetfile 是还原到的路径
bool EchoBackup::restoreFile(std::string sourceDir, std::string filename, std::string targetfile  ,std::string password){
    std::vector<std::string> BUSuffixs;
    for (const auto& pair : BUKindsSuffix)if(pair.first!="UnPack")BUSuffixs.push_back(pair.second);
    // 区分五种情况，不打包、打包、打包压缩、打包加密、打包压缩加密
    
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos ||   // 普通备份
        (pos != std::string::npos &&  std::find(BUSuffixs.begin(),BUSuffixs.end(),filename.substr(pos))== BUSuffixs.end() ) ){
        // 普通备份
        return FileUtils::copyAllFileKinds( sourceDir+"/"+filename , targetfile) ;
    }else{
        std::string fileSuffix = filename.substr( filename.find_last_of('.') );
        std::string baseName =  filename.substr( 0 ,  filename.find_last_of('.') );
        // std::cout<<fileSuffix <<std::endl<< baseName <<std::endl ;


        //区分四种备份
        if( fileSuffix == BUKindsSuffix["CmpEnc"] ){    // 打包&压缩&加密
            
            EchoEncrypt encrypter;
            if(! encrypter.decryptAESFile( sourceDir+"/"+filename  , sourceDir+"/"+ baseName +BUKindsSuffix["Cmp"] , password  )){
                std::cout<<"文件解密失败"<<std::endl;
                return false ;
            }
            // 解压
            EchoCompress compresser;
            if(!compresser.decompress(sourceDir+"/"+ baseName +BUKindsSuffix["Cmp"] , sourceDir+"/"+ baseName + BUKindsSuffix["Pack"] )){
                std::cout<<"文件解压失败"<<std::endl;
                FileUtils::rmDirOrFile( sourceDir+"/"+ baseName  +BUKindsSuffix["Cmp"]   , false); // 解压失败了，把解密的文件删掉
                return false ;
            }
            FileUtils::rmDirOrFile( sourceDir+"/"+ baseName +  BUKindsSuffix["Cmp"]   , false); // 解压成功了，把解密的文件删掉


        }else if(fileSuffix == BUKindsSuffix["Enc"]){   // 打包&加密
            // 解密
            EchoEncrypt encrypter;
            if(! encrypter.decryptAESFile( sourceDir+"/"+filename  , sourceDir+"/"+ baseName +BUKindsSuffix["Pack"] , password  )){
                std::cout<<"文件解密失败"<<std::endl;
                return false ;
            }
        }else if(fileSuffix == BUKindsSuffix["Cmp"]){   // 打包&压缩
            // 解压缩
            EchoCompress compresser;
            if(! compresser.decompress( sourceDir+"/"+filename  , sourceDir+"/"+ baseName  +BUKindsSuffix["Pack"] )){
                std::cout<<"文件解压失败"<<std::endl;
                return false ;
            }
        }else if(fileSuffix == BUKindsSuffix["Pack"]){   // 打包
            
        }else{
            std::cout<<"备份选项传参错误"<<std::endl;
            // FileUtils::rmDirOrFile( BUFilePath + "/" +BUVersion+BUKindsSuffix["Pack"]  , false); // 打包成功了，把复制的文件删掉
            return false;
        }

        // // 解包
        // if(!EchoPack::unpackBag(  sourceDir+"/"+ baseName +  BUKindsSuffix["Pack"]  ,  sourceDir+"/"+ baseName  )){
        //     std::cout<<"文件解包失败"<<std::endl;
        //     if(fileSuffix != BUKindsSuffix["Pack"])FileUtils::rmDirOrFile( sourceDir+"/"+ baseName + BUKindsSuffix["Pack"]  , false); // 打包失败了，把复制的文件删掉
        //     return false ;
        // }
        // if(fileSuffix != BUKindsSuffix["Pack"])
        //     FileUtils::rmDirOrFile( sourceDir+"/"+ baseName +  BUKindsSuffix["Pack"]   , false); // 解包成功了，把之前的包文件删掉
        // // 复制到指定路径 targetfile
        // std::cout<<sourceDir+"/"+ baseName<<std::endl<< targetfile<<std::endl;
        // if(!FileUtils::copyAllFileKinds( sourceDir+"/"+ baseName , targetfile)){ 
        //     std::cout<<"解包后的文件复制失败"<<std::endl;
        //     FileUtils::rmDirOrFile( sourceDir+"/"+ baseName  , false); // 打包失败了，把复制的文件删掉
        //     return false ;
        // }
        // FileUtils::rmDirOrFile( sourceDir+"/"+ baseName  , false); // 打包失败了，把复制的文件删掉


        // 解包
        if(!EchoPack::unpackBag(  sourceDir+"/"+ baseName +  BUKindsSuffix["Pack"]  ,  targetfile )){
            std::cout<<"文件解包失败"<<std::endl;
            if(fileSuffix != BUKindsSuffix["Pack"])FileUtils::rmDirOrFile( sourceDir+"/"+ baseName + BUKindsSuffix["Pack"]  , false); // 打包失败了，把复制的文件删掉
            return false ;
        }
        if(fileSuffix != BUKindsSuffix["Pack"])
            FileUtils::rmDirOrFile( sourceDir+"/"+ baseName +  BUKindsSuffix["Pack"]   , false); // 解包成功了，把之前的包文件删掉
    }
    std::cout<<"还原成功！"<<std::endl;
    return true;
}




