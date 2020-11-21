#!/usr/bin/env python
# encoding: utf-8

import argparse
import random
import os
import multiprocessing as mp
import threading

import jieba
import jieba.posseg as pseg
from loguru import logger


def cut_news_worker(args, data_list, i):
    """每个线程或进程实际执行的逻辑"""
    n_data = len(data_list)
    logger.info("进程 {} 开始分词，新闻数量为: {}".format(i, n_data))
    output_path = os.path.join(args.cut_dir, "cut_data_{}".format(i))
    n_done = 0
    with open(output_path, "w", encoding="utf-8") as fout:
        # 遍历每个文件
        for label_id, label_name, fname in data_list:
            line_str = str(label_name) + "\t" + str(label_id) + "\t"
            with open(fname, encoding="utf-8") as fin:
                # 第一行为标题
                line_no = 0
                for line in fin:
                    line = line.strip()
                    # 跳过空行
                    if len(line) == 0:
                        continue

                    # 标题
                    if line_no == 0:
                        title_str = ""
                        # 存储词和词性
                        words = pseg.cut(line)
                        for word, pos in words:
                            # 跳过空白符
                            if len(word.strip()) == 0:
                                continue
                            title_str += word.strip() + "/" + pos + ","
                        line_str += title_str.rstrip(",") + "\t"

                        line_no += 1
                        continue

                    # 正文分词
                    content_str = ""
                    words = pseg.cut(line)
                    for word, pos in words:
                        # 跳过空白符
                        if len(word.strip()) == 0:
                            continue
                        content_str += word.strip() + "/" + pos + ","
                    line_str += content_str

            # 存储格式: 标签名 \t 标签id \t 标题分词 \t 正文分词
            fout.write(line_str.rstrip(",") + "\n")

            n_done += 1
            if n_done % 100 == 0:
                logger.info("进程 {} 分词进度为: {} / {} = {:.2%}".format(i, n_done, n_data, n_done/n_data))
    logger.info("进程 {} 分词done!".format(i))

def cut_news_thread(args, label_map):
    """使用多线程对新闻数据集分词"""
    data_list = []
    # 遍历每个类别
    for label_name, label_id in label_map.items():
        data_dir = os.path.join(args.data_dir, label_name)
        # 遍历每个类别目录下的文件，把文件路径全都存储到data_list中
        for root, _, files in os.walk(data_dir):
            for f in files:
                fname = os.path.join(root, f)
                data_list.append((label_id, label_name, fname))
    n_data = len(data_list)
    logger.info("新闻总数为: {}".format(n_data))

    # 随机打乱数据
    random.shuffle(data_list)
    # 数据总量除以线程数，表示每个线程处理的数据量，最后一个线程处理的数据量会小于或等于前面的线程
    shard_size = n_data // args.n_worker + 1
    thread_list = []
    for i in range(args.n_worker):
        # 使用多线程或多进程进行计算或处理数据的基本方法是进行数据划分
        # 每个线程或进程分别处理数据的一部分
        # 第i个线程所处理数据的起始索引
        begin_idx = i * shard_size
        # 第i个线程所处理数据的末尾索引
        end_idx = min((i+1)*shard_size, n_data)
        # 构造worker线程分别计算或处理不同的数据，其中target参数为worker具体执行逻辑，args参数为传递给worker的参数列表
        p = threading.Thread(target=cut_news_worker, args=(args, data_list[begin_idx:end_idx], i))
        # 启动线程
        p.start()
        thread_list.append(p)
    for p in thread_list:
        # 必须对每个线程调用join
        p.join()

    # 将每个线程生成的文件合并到一个文件中
    output_path = os.path.join(args.cut_dir, "cut_news.txt")
    for i in range(args.n_worker):
        fname = os.path.join(args.cut_dir, "cut_data_{}".format(i))
        os.system("cat {} >> {}".format(fname, output_path))
    logger.info("分词done!")


def cut_news_process(args, label_map):
    """使用多进程对新闻数据集分词"""
    data_list = []
    # 遍历每个类别
    for label_name, label_id in label_map.items():
        data_dir = os.path.join(args.data_dir, label_name)
        # 遍历每个类别目录下的文件，把文件路径全都存储到data_list中
        for root, _, files in os.walk(data_dir):
            for f in files:
                fname = os.path.join(root, f)
                data_list.append((label_id, label_name, fname))
    n_data = len(data_list)
    logger.info("新闻总数为: {}".format(n_data))

    # 随机打乱数据
    random.shuffle(data_list)
    # 数据总量除以进程数，表示每个进程处理的数据量，最后一个进程处理的数据量会小于或等于前面的线程(进程)
    shard_size = n_data // args.n_worker + 1
    process_list = []
    for i in range(args.n_worker):
        # 使用多线程或多进程进行计算或处理数据的基本方法是进行数据划分
        # 每个线程或进程分别处理数据的一部分
        # 第i个进程所处理数据的起始索引
        begin_idx = i * shard_size
        # 第i个进程所处理数据的末尾索引
        end_idx = min((i+1)*shard_size, n_data)
        # 构造worker进程分别计算或处理不同的数据，其中target参数为worker具体执行逻辑，args参数为传递给worker的参数列表
        p = mp.Process(target=cut_news_worker, args=(args, data_list[begin_idx:end_idx], i))
        # 启动进程
        p.start()
        process_list.append(p)
    for p in process_list:
        # 必须对每个进程调用join
        p.join()

    # 将每个线程或进程生成的文件合并到一个文件中
    output_path = os.path.join(args.cut_dir, "cut_news.txt")
    for i in range(args.n_worker):
        fname = os.path.join(args.cut_dir, "cut_data_{}".format(i))
        os.system("cat {} >> {}".format(fname, output_path))
    logger.info("分词done!")

def main():
    parser = argparse.ArgumentParser(description="清华大学THUCNEWS新闻预处理")
    parser.add_argument("--data_dir", type=str, default="/da1/dataset/nlp/thucnews/data", help="原始新闻数据根目录")
    parser.add_argument("--cut_dir", type=str, default="/da1/dataset/nlp/thucnews/cut_data", help="分词后输出目录")
    parser.add_argument("--n_worker", type=int, default=8, help="分词使用线程数或进程数")
    args = parser.parse_args()

    # 解压数据之后我们得到的是中文目录，我们需要把对应的中文目录重命名为英文目录，路径名中最好是不要出现中文
    # 这里我们显式为每个类别赋予一个整数类别id
    label_map = {
        "constellation": 0,         # 星座
        "education": 1,             # 教育
        "entertainment": 2,         # 娱乐
        "fashion": 3,               # 时尚
        "finance": 4,               # 财经
        "game": 5,                  # 游戏
        "home": 6,                  # 家居
        "house": 7,                 # 房产
        "lottery": 8,               # 彩票
        "politics": 9,              # 时政
        "society": 10,              # 社会
        "sports": 11,               # 体育
        "stock": 12,                # 股票
        "technology": 13            # 科技
    }

    # cut_news_thread(args, label_map)

    cut_news_process(args, label_map)

if __name__ == "__main__":
    main()
