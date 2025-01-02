#include "../../include/pack/echoPack.hpp"

std::string EchoPack::packDirName = "";  // 初始化为合适的默认值

// 获取目录 dirname 的大小
long long int EchoPack::getDirSize(std::string dirname){
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	std::string subdir;
	long long int totalSize = 0;
	if ((dp = opendir(dirname.c_str())) == NULL){
		fprintf(stderr, "Cannot open dir: %s\n", dirname.c_str());
		return -1; // 可能是个文件，或者目录不存在
	}
	// 先加上自身目录的大小
	lstat(dirname.c_str(), &statbuf);
	totalSize += statbuf.st_size;
	while ((entry = readdir(dp)) != NULL){
		if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			continue;
		subdir = dirname + '/' + entry->d_name;
		lstat(subdir.c_str(), &statbuf);
		if (S_ISDIR(statbuf.st_mode)){
			long long int subDirSize = getDirSize(subdir);
			totalSize += subDirSize;
		}
		else{
			totalSize += statbuf.st_size;
		}
	}
	closedir(dp);
	return totalSize;
}

// 对目标目录或文件生成对应头结点
headblock *EchoPack::genHeader(std::string filename){
	int i;
	struct stat srcbuf;
	lstat(filename.c_str(), &srcbuf);
	headblock *head = new headblock();
	 
	long mtimesec, mtimensec;
	std::string mtimesecstring, mtimensecstring;
	char sylinkname[100];
	// 存储文件名
	std::string storeFilename = filename.substr(filename.find(packDirName)+packDirName.length()) ;
	// std::cout<<storeFilename<<std::endl;
	strcpy(head->name, storeFilename.c_str());
	// 存储文件st_mode,事实上已经包含用户权限和文件类型
	strcpy(head->mode, (std::to_string(srcbuf.st_mode)).c_str());
	// 存储用户ID
	strcpy(head->uid, (std::to_string(srcbuf.st_uid)).c_str());
	// 存储groupID
	strcpy(head->gid, (std::to_string(srcbuf.st_gid)).c_str());
	// 存储目录或大小
	if (S_ISDIR(srcbuf.st_mode))
		strcpy(head->size, (std::to_string(getDirSize(filename))).c_str());
	else
		strcpy(head->size, (std::to_string(srcbuf.st_size)).c_str());
	// 存储最近修改时间
	sprintf(head->mtime_sec, "%ld", srcbuf.st_mtim.tv_sec);
	sprintf(head->mtime_nsec, "%ld", srcbuf.st_mtim.tv_nsec);

	//// 存储文件类型
	if (S_ISREG(srcbuf.st_mode)) head->typeflag = '0';  // 普通文件
	if (S_ISLNK(srcbuf.st_mode)){ // 符号链接文件
		head->typeflag = '1';
		if (readlink(filename.c_str(), sylinkname, srcbuf.st_size) != -1) ; //std::cout << "读取链接成功"<<std::endl;
		else  perror("link");
		// 软链接，存放链接路径
		strcpy(head->linkname, sylinkname);
		head->linkname[srcbuf.st_size] = '\0';
	}
	if (S_ISFIFO(srcbuf.st_mode)) head->typeflag = '2'; // 管道文件
	if(S_ISCHR(srcbuf.st_mode) || S_ISBLK(srcbuf.st_mode)){ // 设备文件
		head->typeflag = '3'; 
		strcpy(head->rdev, (std::to_string(srcbuf.st_rdev)).c_str());
	} 
	if(S_ISSOCK(srcbuf.st_mode)) head->typeflag = '4'; // 套接字文件
	if (S_ISDIR(srcbuf.st_mode))  head->typeflag = '5'; // 文件夹

	
	// 头结点块标记
	head->fileflag = '1';
	// 头结点末尾填零
	char padd[170] = {'\0'};
	stpcpy(head->padding, padd);
	return head ; 
}



// 递归扫描目录时将文件写入 bagfile 中,dirname就是当前要打包的目录
bool EchoPack::addFileToBag(std::string dirname, int bagfile){
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	headblock *head;
	// std::cout << "当前打包" << dirname << std::endl;

	// 首先写入当前目录的头结点
	head = genHeader(dirname);
	write(bagfile, head, BLOCKSIZE);
	int fin, fout = bagfile; // 文件描述符
	std::string filePath;
	char buffer[BLOCKSIZE];
	char flag[BLOCKSIZE] = {'\0'};
	// 打开目录并返回存储目录信息的DIR结构体
	if ((dp = opendir(dirname.c_str())) == NULL){
		fprintf(stderr, "Can`t open directory %s\n", dirname.c_str());
		return false;
	}
	// 通过先前的DIR目录结构体读取当前目录的目录文件
	// 返回dirent结构体（entry）保存目录文件的信息
	while ((entry = readdir(dp)) != NULL){
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		filePath = dirname + '/' + entry->d_name;
		lstat(filePath.c_str(), &statbuf);
		
		// 打包
		if (S_ISDIR(statbuf.st_mode)){		// 如果是子目录，递归打包
			addFileToBag(filePath, fout);
		}else if (S_ISREG(statbuf.st_mode)){	// 如果是普通文件，创建头结点并写入文件内容
			head = genHeader(filePath);
			write(fout, head, BLOCKSIZE);
			fin = open(filePath.c_str(), O_RDONLY);
			// 文件比一个块大，才多次读入，否则一次读写即可
			if (statbuf.st_size > BLOCKSIZE){
				for (int i = 0; i < (statbuf.st_size / BLOCKSIZE) - 1; i++){
					read(fin, buffer, BLOCKSIZE);
					write(fout, buffer, BLOCKSIZE);
				}
				// 最后一次读写补零
				read(fin, buffer, statbuf.st_size % BLOCKSIZE);
				write(fout, buffer, statbuf.st_size % BLOCKSIZE);
				write(fout, flag, BLOCKSIZE - statbuf.st_size % BLOCKSIZE); // 末尾补零
			}else{
				read(fin, buffer, statbuf.st_size);
				write(fout, buffer, statbuf.st_size);
				write(fout, flag, BLOCKSIZE - statbuf.st_size);
			}
			close(fin);
		}else if (S_ISLNK(statbuf.st_mode) || S_ISFIFO(statbuf.st_mode) 
					|| S_ISCHR(statbuf.st_mode)|| S_ISBLK(statbuf.st_mode) || S_ISSOCK(statbuf.st_mode)  ){  // 如果是链接文件和管道文件，只保留头结点即可
			head = genHeader(filePath);
			write(fout, head, BLOCKSIZE);
		}else	std::cout << "抱歉,文件" << filePath << "类型暂不支持！"<<std::endl;
	
	}
	closedir(dp);
	return true;
}

// 打包目录文件，调用 addFileToBag 打包源目录，sourcedir为打包源目录，targetbag为打包输出文件，后缀名为bo
bool EchoPack::packDir(std::string sourcedir, std::string targetbag){
	DIR *dp;
	std::string dirPath;
	int fout;
	headblock *head;
	char flag[BLOCKSIZE] = {'\0'};
	// 使用 open 函数创建并打开目标文件 targetbag。打开模式为 O_RDWR可读可写; O_CREAT文件不存在时创建文件; O_TRUNC清空文件内容; O_APPEND追加写入文件; 文件权限设置为 0644，表示文件拥有者可读写，其他用户只读。
	if ((fout = open(targetbag.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0644)) < 0){
		return false;
	}
	// 打开目录并返回存储目录信息的DIR结构体
	if ((dp = opendir(sourcedir.c_str())) == NULL){
		fprintf(stderr, "Can`t open directory %s\n", sourcedir.c_str());
		return false;
	}
	packDirName = sourcedir.substr(sourcedir.find_last_of("/")+1) ;
	// 打包文件
	if (addFileToBag(sourcedir, fout)){
	// if (addFileToBag("/."+ sourcedir.substr(sourcedir.find_last_of("/")) , fout)){
		write(fout, flag, BLOCKSIZE); //使用 write 函数向目标文件写入 flag 填充块，以满足文件对齐或结尾标记。
		close(fout);
		std::cout << "打包成功！"<<std::endl;
		packDirName = "";
		return true;
	}
	
	close(fout);
	std::cout << "打包失败!"<<std::endl;
	packDirName = "";
	return false;
}


// 打包文件  //sourcefile是待打包的文件 ， targetbag是包的路径加文件名
bool EchoPack::packFile(std::string sourcefile, std::string targetbag){
	// 如果传入的是个目录，那么调用打包目录的
	if(std::filesystem::is_directory(sourcefile))return packDir(sourcefile , targetbag );
	//下面是打包非目录文件的
	int fin,fout;
	struct dirent *entry;
	struct stat statbuf;
	headblock *head;
	// std::cout << "当前打包" << dirname << std::endl;
	packDirName = sourcefile.substr(sourcefile.find_last_of("/")+1) ;

	if ((fout = open(targetbag.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0644)) < 0){
		return false;
	}
	// 首先写入当前目录的头结点
	// head = genHeader(sourcefile);
	// write(fout, head, BLOCKSIZE);
	char buffer[BLOCKSIZE];
	char flag[BLOCKSIZE] = {'\0'};

	// 获取源文件元数据信息
	stat(sourcefile.c_str(), &statbuf);
	if (S_ISREG(statbuf.st_mode)){	// 如果是普通文件，创建头结点并写入文件内容
		head = genHeader(sourcefile);
		write(fout, head, BLOCKSIZE);
		fin = open(sourcefile.c_str(), O_RDONLY);
		// 文件比一个块大，才多次读入，否则一次读写即可
		if (statbuf.st_size > BLOCKSIZE){
			for (int i = 0; i < (statbuf.st_size / BLOCKSIZE) - 1; i++){
				read(fin, buffer, BLOCKSIZE);
				write(fout, buffer, BLOCKSIZE);
			}
			// 最后一次读写补零
			read(fin, buffer, statbuf.st_size % BLOCKSIZE);
			write(fout, buffer, statbuf.st_size % BLOCKSIZE);
			write(fout, flag, BLOCKSIZE - statbuf.st_size % BLOCKSIZE); // 末尾补零
		}else{
			read(fin, buffer, statbuf.st_size);
			write(fout, buffer, statbuf.st_size);
			write(fout, flag, BLOCKSIZE - statbuf.st_size);
		}
		close(fin);
	}else if (S_ISLNK(statbuf.st_mode) || S_ISFIFO(statbuf.st_mode
				|| S_ISCHR(statbuf.st_mode)|| S_ISBLK(statbuf.st_mode) || S_ISSOCK(statbuf.st_mode)  )){  // 如果是链接文件和管道文件，只保留头结点即可
		head = genHeader(sourcefile);
		write(fout, head, BLOCKSIZE);
	}else{	
		std::cout << "抱歉,文件" << sourcefile << "类型暂不支持！"<<std::endl;
		return false;
	}

	write(fout, flag, BLOCKSIZE); //使用 write 函数向目标文件写入 flag 填充块，以满足文件对齐或结尾标记。
	std::cout << "打包成功！"<<std::endl;
	close(fout);
	packDirName = "";
	return true;

}



// 具体拆包函数，sourcebag为源包的文件描述符，targetdir为解包目标目录
bool EchoPack::turnBagToFile(int sourcebag, std::string targetdir){
	timespec times[2];
	std::string filepath;
	int fout;
	char buffer[BLOCKSIZE];
	headblock head[BLOCKSIZE];
	while (read(sourcebag, head, BLOCKSIZE)){
		// 不是头结点不在此读
		if (head->fileflag != '1')
			continue;
		times[0].tv_sec = time_t(atol(head->mtime_sec));
		times[0].tv_nsec = atol(head->mtime_nsec);
		times[1] = times[0];
		filepath = targetdir  + head->name;
		
		// 解包
		if (head->typeflag == '0'){		// 读到普通文件的头结点，生成对应普通文件
			if ((fout = open(filepath.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0644)) < 0) {
				std::cout<<filepath+"输出文件打开失败"<<std::endl;	
				return false;
			}
			if (atol(head->size) > BLOCKSIZE){
				for (int i = 0; i < (atoi(head->size) / BLOCKSIZE) - 1; i++){
					read(sourcebag, buffer, BLOCKSIZE);
					write(fout, buffer, BLOCKSIZE);
				}
				// 最后一次读写
				read(sourcebag, buffer, atoi(head->size) % BLOCKSIZE);
				write(fout, buffer, atoi(head->size) % BLOCKSIZE);
				read(sourcebag, buffer, BLOCKSIZE - (atoi(head->size) % BLOCKSIZE));
			}else{
				read(sourcebag, buffer, atoi(head->size));
				write(fout, buffer, atoi(head->size));
				read(sourcebag, buffer, BLOCKSIZE - atoi(head->size));
			}
			close(fout);
		}else if (head->typeflag == '1'){	// 若是软连接的头文件，直接生成对应头结点的软链接
			if (symlink(head->linkname, filepath.c_str()) < 0){
				perror("link");
				return false;
			}
		}else if (head->typeflag == '2'){	// 若是管道文件，生成对应管道文件
			if (mkfifo(filepath.c_str(), atoi(head->mode)) < 0){
				perror("pipe");
				return false;
			}
		}else if(head->typeflag == '3'){ // 设备文件
			if (mknod(filepath.c_str(), atoi(head->mode), atoi(head->rdev))< 0){
				perror("dev");
				return false;
			}
		}else if(head->typeflag == '4'){ // 套接字文件
			if (mknod(filepath.c_str(), S_IFSOCK | (atoi(head->mode) & 07777), 0)){
				perror("socket");
				return false;
			}
		}else{	  // 文件夹
			if (mkDir(filepath.c_str(), atoi(head->mode))){
				// std::cout << "成功创建目录" << filepath << std::endl;
			}else	std::cout << "创建失败"<<std::endl;
		}
		// 对应头结点生成对应文件后，设置文件元数据
		chmod(filepath.c_str(), atoi(head->mode));
		chown(filepath.c_str(), atoi(head->uid), atoi(head->gid));
		utimensat(AT_FDCWD, filepath.c_str(), times, AT_SYMLINK_NOFOLLOW);
	}
	return true;
}

// 解包，将打包后的.bo文件(sourcebag)解包到目录(targetfile)
bool EchoPack::unpackBag(std::string sourcebag, std::string targetfile){
	// 先简单校验是否为.bo文件
	int fin;
	if ( sourcebag.substr(sourcebag.find_last_of('.'))!= BUKindsSuffix["Pack"] ){
		std::cout << "当前解包文件格式不符，退出解包！"<<std::endl;
		return false;
	}else{
		fin = open(sourcebag.c_str(), O_RDONLY);
		if (turnBagToFile(fin, targetfile)){
			std::cout << "解包成功！"<<std::endl;
			close(fin);
			return true;
		}else	std::cout << "解包失败！"<<std::endl;

		close(fin);
		return false;
	}
}