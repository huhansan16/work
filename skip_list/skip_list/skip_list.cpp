#include <iostream>
#include "skip.h"
int main() {
    #define FILE_PATH "/home/huhansan/projects/skip_list/dumpFile"
	SkipList<std::string, std::string> skipList(6);
	skipList.insert_element("1", "redis五大数据类型");
	skipList.insert_element("3", "list");
	skipList.insert_element("7", "set");
	skipList.insert_element("8", "string");
	skipList.insert_element("9", "zset");
	skipList.insert_element("19", "hash");
	cout << "skipList size:" << skipList.size() <<endl;
	//skipList.dump_file();//将数据写入文件

	 skipList.load_file();

	skipList.search_element("9");
	skipList.search_element("18");

	skipList.display_list();

	skipList.delete_element("3");
	skipList.delete_element("7");

	std::cout << "skipList size:" << skipList.size() << std::endl;

	skipList.display_list();

}