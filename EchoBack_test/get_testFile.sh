#!/bin/bash

# 8种文件类型
file_types=("txt" "directory" "symlink" "hardlink" "char_device" "block_device" "pipe" "socket")

# 设置目标目录（每次循环使用不同的目录名）
target_dir0="$PWD/test_0"
# 创建目标目录（如果不存在）
mkdir -p "$target_dir0"
# 切换到目标目录
cd "$target_dir0"
i=0
# 根据外层选择的文件类型创建文件
for type in "${file_types[@]}"
do
    case $type in
        "txt")
            # 创建普通文件 test_${i}.txt，并将计数值写入文件内容
            touch "test_${i}.txt"
            echo "This is the ${i} th regular file" > "test_${i}.txt"
            ;;
        "directory")
            # 创建目录文件 test_directory_${i}
            mkdir "test_directory_${i}"
            ;;
        "symlink")
            # 创建软链接文件 test_softlink_${i}
            ln -s "test_${i}.txt" "test_softlink_${i}"
            ;;
        "hardlink")
            # 创建硬链接文件 test_hardlink_${i}
            ln "test_${i}.txt" "test_hardlink_${i}"
            ;;
        "char_device")
            # 创建字符设备文件 test_char_device_${i}
            sudo mknod "test_char_device_${i}" c 10 $((200 + i))
            ;;
        "block_device")
            # 创建块设备文件 test_block_device${i}
            sudo mknod "test_block_device_${i}" b 7 $((i))
            ;;
        "pipe")
            # 创建管道文件 test_pipe_${i}
            mkfifo "test_pipe_${i}"
            ;;
        "socket")
            # 创建套接字文件 test_socket_${i}
            nc -lU "test_socket_${i}" &
            ;;
    esac
done

# 压缩算法测试
cd "test_directory_${i}"
# 创建普通文件 test_${i}.txt，并将计数值写入文件内容
touch "test_InDir.txt"

# 生成随机内容
generate_content() {
    echo "测试文档 - 用于压缩算法测试"
    echo ""
    for i in {1..500}; do
        echo "段落 $i: $(head /dev/urandom | tr -dc 'a-zA-Z0-9 ' | fold -w 100 | head -n 1)"
    done
    echo ""
    echo "重复性数据:"
    for i in {1..100}; do
        echo "这是一个重复的句子。 " | tr -d '\n'
    done
}
generate_content >"test_InDir.txt"

cd ../..

# 循环 10 次，生成文件，每次使用不同的目标目录和后缀
for i in {1..10}
do
    # 通过 (i) % 5 选择外层循环文件类型组合
    outer_file_type_index=$(( i % 5 ))

    # 基于 (i) % 5 选择外层文件类型组合
    case $outer_file_type_index in
        0)
            # 组合1：txt, symlink, pipe, socket
            outer_selected_types=("txt" "symlink" "pipe" "socket")
            ;;
        1)
            # 组合2：txt, directory, pipe, socket
            outer_selected_types=("txt" "directory" "pipe" "socket")
            ;;
        2)
            # 组合3：txt, symlink, char_device, block_device
            outer_selected_types=("txt" "symlink" "char_device" "block_device")
            ;;
        3)
            # 组合4：txt, directory, char_device, block_device
            outer_selected_types=("txt" "directory" "char_device" "block_device")
            ;;
        4)
            # 组合5：symlink, directory, pipe, block_device
            outer_selected_types=("txt" "symlink" "directory" "pipe" )
            ;;
    esac

    # 设置目标目录（每次循环使用不同的目录名）
    target_dir="$PWD/test_$i"

    # 创建目标目录（如果不存在）
    mkdir -p "$target_dir"

    # 切换到目标目录
    cd "$target_dir"

    # 根据外层选择的文件类型创建文件
    for type in "${outer_selected_types[@]}"
    do
        case $type in
            "txt")
                # 创建普通文件 test_${i}.txt，并将计数值写入文件内容
                touch "test_${i}.txt"
                echo "This is the ${i} th regular file" > "test_${i}.txt"
                ;;
            "directory")
                # 创建目录文件 test_directory_${i}
                mkdir "test_directory_${i}"
                ;;
            "symlink")
                # 创建软链接文件 test_softlink_${i}
                ln -s "test_${i}.txt" "test_softlink_${i}"
                ;;
            "hardlink")
                # 创建硬链接文件 test_hardlink_${i}
                ln "test_${i}.txt" "test_hardlink_${i}"
                ;;
            "char_device")
                # 创建字符设备文件 test_char_device_${i}
                sudo mknod "test_char_device_${i}" c 10 $((200 + i))
                ;;
            "block_device")
                # 创建块设备文件 test_block_${i}
                sudo mknod "test_block_${i}" b 7 $((i))
                ;;
            "pipe")
                # 创建管道文件 test_pipe_${i}
                mkfifo "test_pipe_${i}"
                ;;
            "socket")
                # 创建套接字文件 test_socket_${i}
                nc -lU "test_socket_${i}" &
                ;;
        esac
    done

    # 内层循环 2 次，生成文件，每次使用不同的目标目录和后缀
    for j in {1..2}
    do
        # 通过 (i + j) % 5 选择内层循环文件类型组合，5种组合，每种组合包含4种文件类型
        inner_file_type_index=$(( (i + j) % 5 ))

        # 基于 (i + j) % 5 选择内层文件类型组合
        case $inner_file_type_index in
            0)
                # 组合1：txt, symlink, pipe, socket
                inner_selected_types=("txt" "symlink" "pipe" "socket")
                ;;
            1)
                # 组合2：txt, directory, pipe, socket
                inner_selected_types=("txt" "directory" "pipe" "socket")
                ;;
            2)
                # 组合3：txt, symlink, char_device, block_device
                inner_selected_types=("txt" "symlink" "char_device" "block_device")
                ;;
            3)
                # 组合4：txt, directory, char_device, block_device
                inner_selected_types=("txt" "directory" "char_device" "block_device")
                ;;
            4)
                # 组合5：symlink, directory, pipe, block_device
                inner_selected_types=("txt" "symlink" "directory" "block_device")
                ;;
        esac

        # 创建子目录
        subdir_level=$(( (i + j) % 3 ))  # 根据 i 和 j 控制目录的层级

        if [ $subdir_level -eq 0 ]; then
            subdir="subdir_A"
        elif [ $subdir_level -eq 1 ]; then
            subdir="subdir_B"
        else
            subdir="subdir_C"
        fi

        # 创建目标子目录
        mkdir -p "test_directory_${i}/${subdir}"

        # 切换到目标子目录
        cd "test_directory_${i}/${subdir}"

        # 根据选择的文件类型创建文件
        for type in "${inner_selected_types[@]}"
        do
            case $type in
                "txt")
                    # 创建普通文件 test_${i}_${j}.txt，并将计数值写入文件内容
                    touch "test_${i}_${j}.txt"
                    echo "This is the ${i}_${j} th regular file" > "test_${i}_${j}.txt"
                    ;;
                "directory")
                    # 创建目录文件 test_directory_${i}_${j}
                    mkdir "test_directory_${i}_${j}"
                    ;;
                "symlink")
                    # 创建软链接文件 test_softlink_${i}_${j}
                    ln -s "test_${i}_${j}.txt" "test_softlink_${i}_${j}"
                    ;;
                "hardlink")
                    # 创建硬链接文件 test_hardlink_${i}_${j}
                    ln "test_${i}_${j}.txt" "test_hardlink_${i}_${j}"
                    ;;
                "char_device")
                    # 创建字符设备文件 test_char_device_${i}_${j}
                    sudo mknod "test_char_device_${i}_${j}" c 10 $((200 + i))
                    ;;
                "block_device")
                    # 创建块设备文件 test_block_${i}_${j}
                    sudo mknod "test_block_${i}_${j}" b 7 $((i))
                    ;;
                "pipe")
                    # 创建管道文件 test_pipe_${i}_${j}
                    mkfifo "test_pipe_${i}_${j}"
                    ;;
                "socket")
                    # 创建套接字文件 test_socket_${i}_${j}
                    nc -lU "test_socket_${i}_${j}" &
                    ;;
            esac
        done

        # 返回上级目录
        cd "$target_dir"
    done

    # 返回上级目录
    cd ..
done
