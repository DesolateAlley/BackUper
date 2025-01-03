#include"../../include/encrypt/echoEncrypt.hpp"
 

bool EchoEncrypt::encyptFile(std::string md5ofpwd,std::string md5ofmd5odpwd,std::string sourcefile,std::string targetfile){

    char keyStr[16];
    // 最后将密码的MD5的MD5写入文件末尾，以便检验密码是否正确

    for(int i=0;i<16;i++){
        keyStr[i]=char(strtol(md5ofpwd.substr(2*i,2).c_str(),NULL,16));
    }
    // 用于处理最后一字节
    struct stat srcbuf;
    int count,i,offset;
	
	byte key[16]; 
	charToByte(key, keyStr);
	// 密钥扩展
	word w[4 * (Nr + 1)];
	KeyExpansion(key, w);
	std::bitset<128> data;
	byte plain[16];
    data.reset();
 


	int fin,fout;
	fin=open(sourcefile.c_str(),O_RDONLY);
    stat(sourcefile.c_str(),&srcbuf);
    count=srcbuf.st_size/16;
    offset=srcbuf.st_size%16;
	fout=open(targetfile.c_str(),O_RDWR|O_CREAT|O_TRUNC|O_APPEND,0644);
	for(i=0; i<count; i++){
        read(fin,(char*)&data, sizeof(data));
        divideToByte(plain, data);
		encrypt(plain, w);
		data = mergeByte(plain);
		write(fout,(char*)&data, sizeof(data));
		data.reset();  // 置0
    }

    // 最后一块加密时补充8个字节16
    if(offset==0){
        memset((char*)&data,16,16);
        divideToByte(plain, data);
		encrypt(plain, w);
		data = mergeByte(plain);
		write(fout,(char*)&data, sizeof(data));
    }else{   //否则按照offset填充
        read(fin,(char*)&data,offset);
        memset((char*)&data+offset,16-offset,16-offset);
        divideToByte(plain, data);
		encrypt(plain, w);
		data = mergeByte(plain);
		write(fout,(char*)&data, sizeof(data));
    }
    char buffer[33];
    strcpy(buffer,md5ofmd5odpwd.c_str());
    // std::cout<<"写入的文件:" <<targetfile <<std::endl;
    // std::cout<<"写入的md5md5:" <<buffer <<std::endl;
    
    write(fout,buffer,32);
    fsync(fout);  // 确保数据写入磁盘

	close(fin);
	close(fout);
	std::cout << "加密成功！"<<std::endl;
    return true;
	
}

// 集成打包加密使用，这个时候 targetfile 所在的目录肯定存在，所以无需检查目录存在与否
bool EchoEncrypt::encyptAESFile(std::string sourcefile, std::string targetfile, std::string password){
    std::string md5, md5md5;
    md5 = this->encode(password);
    md5md5 = this->encode(md5);
    return this->encyptFile(md5, md5md5, sourcefile, targetfile);
}


bool EchoEncrypt::decryptFile(std::string md5ofpwd,std::string md5md5ofpwd, std::string sourcefile,std::string targetfile){
      
    int count,i;	
    int fin,fout;
    struct stat srcbuf;
    //首先验证密码正确性
    std::string filetail;
    char buffer[33];
    fin=open(sourcefile.c_str(),O_RDONLY);
    lseek(fin,-32,SEEK_END);
    read(fin,buffer,32);
    buffer[32]='\0';
    close(fin);
    close(fout);
    filetail=buffer;
    // std::cout<<sourcefile<<std::endl ;
    // std::cout<<buffer<<std::endl ;
    if(md5md5ofpwd!=filetail){
        std::cout<<md5md5ofpwd<<"!="<<filetail<<"密码错误，退出解密！\n";
        return false;
    }
    //密码正确则将密码的MD5值转化为密钥
    char keyStr[16];
    for(int i=0;i<16;i++){
        keyStr[i]=char(strtol(md5ofpwd.substr(2*i,2).c_str(),NULL,16));
    }
    byte key[16]; 
	charToByte(key, keyStr);
	// 密钥扩展
	word w[4*(Nr+1)];
	KeyExpansion(key, w);
	std::bitset<128> data;
    std::bitset<8> off;
	byte plain[16];

    //16字节一组，开始解密
	fin=open(sourcefile.c_str(),O_RDONLY);
	fout=open(targetfile.c_str(),O_RDWR|O_CREAT|O_TRUNC|O_APPEND,0644);
    stat(sourcefile.c_str(),&srcbuf);
    count=srcbuf.st_size/16-2;
    for(i=0;i<count-1;i++){
        read(fin,(char*)&data, sizeof(data));
        divideToByte(plain, data );
		decrypt(plain, w);
		data = mergeByte(plain);
		write(fout,(char*)&data, sizeof(data));
		data.reset();  // 置0
    }
    //处理最后16字节
    read(fin,(char*)&data, sizeof(data));
    divideToByte(plain, data );
	decrypt(plain, w);
	data = mergeByte(plain);
	write(fout,(char*)&data,16-plain[0].to_ulong());
	data.reset();  // 置0
    
    close(fin);
    close(fout);
    std::cout<<"文件解密完成，请查看\n";
    return true;

}

bool EchoEncrypt::decryptAESFile( std::string sourcefile, std::string targetfile  , std::string password ){
    std::string suffix = sourcefile.substr(sourcefile.find_last_of(".") ) ;
    if (suffix!= BUKindsSuffix["Enc"] && suffix!= BUKindsSuffix["CmpEnc"] ){
        std::cout << "源文件格式不正确\n";
        return false; 
    }
    std::string md5, md5md5;
    md5 = this->encode(password);
    md5md5 = this->encode(md5);

    // std::cout<<"pwd:"<<password<< std::endl <<"md5   :"<< md5<<std::endl <<"md5md5:"<< md5md5 <<std::endl ;


    return this->decryptFile(md5, md5md5, sourcefile, targetfile);
}



