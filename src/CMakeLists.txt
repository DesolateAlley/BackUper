
# 查找Qt5的Widgets模块（如果在主目录已查找，可省略此行）
find_package(Qt5Widgets REQUIRED)
# 添加子模块
 
# 设置src中的源文件
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fileutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backup/echoBackup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pack/echoPack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/echoCompress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/encrypt/md5.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/encrypt/aes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/encrypt/echoEncrypt.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/window/fileExplor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/mainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/restoreFileWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/backupedView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/pwdInputWindow.cpp

)

# 将src中的源文件添加到项目
# target_sources(EchoBack PRIVATE ${SOURCES})

# 设置头文件
set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/GlobalVariable.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/AllQtHead.hpp  # include目录下的头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/fileutils.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/backup/echoBackup.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/pack/echoPack.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/compress/echoCompress.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/encrypt/echoEncrypt.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/fileExplor.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/mainWindow.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/restoreFileWindow.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/backupedView.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/pwdInputWindow.hpp   



)
 
# 定义可执行文件 EchoBack ，添加源文件和头文件
target_sources(EchoBack PRIVATE ${SOURCES} ${HEADER_FILES})

# 包含include目录，以便找到头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)