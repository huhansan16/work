#pragma once
#ifndef SKIP_H
#define SKIP_H
#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include<string>
using namespace std;



#define STORE_FILE "/home/huhansan/projects/skip_list/dumpFile"
mutex mtx;
string delimiter = ";";




template<typename K,typename V>
class Node {

public:
	Node() {}

	Node(K k, V v, int);

	~Node();

	K get_key() const;

	V get_value() const;

	void set_value(V val);

	Node<K, V>** forwards; //每个节点都维护一个节点指针链表，指向同一层的下一个节点。头节点也是

	int node_level;
private:
	K key;
	V value;

};
template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) {
	key = k;
	value = v;
	node_level = level;
	this->forwards = new Node<K, V>*[node_level + 1];
	memset(this->forwards, 0, sizeof(Node<K, V>*) * (level + 1));



}
template<typename K, typename V>
Node<K, V>::~Node() {
	delete[]forwards;
};
template<typename K, typename V>
K Node<K, V>::get_key() const {
	return key;
};
template<typename K, typename V>
V Node<K, V>::get_value() const {
	return value;
};
template<typename K, typename V>
void Node<K, V>::set_value(V val) {
	this->value = val;
};

template <typename K, typename V>
class SkipList {
public:
	SkipList(int);
	~SkipList();
	int get_random_level();
	Node<K, V>* create_node(K, V, int);
	int insert_element(K, V);
	void display_list();
	bool search_element(K);
	void delete_element(K);
	void dump_file();
	void load_file();
	int size();

private:
	void get_key_value(const string& str, string* key, string* value);
	bool is_valid(const string& str);

private:
	int _max_level;      //最大层数
	int _skip_list_level;  //当前有多少层
	Node<K, V>* _header; //链表头节点
	std::ofstream _file_writer;
	std::ifstream _file_reader;

	int _element_count; //链表中的节点数

};
template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level) {
	this->_max_level = max_level; //最大层数
	this->_skip_list_level = 0;   //现在的层数
	this->_element_count = 0;   //链表中的节点数
	K k;
	V v;
	this->_header = new Node<K, V>(k, v, _max_level);//头节点

}
template<typename K, typename V>
SkipList<K, V>::~SkipList() {

	if (_file_writer.is_open()) {
		_file_writer.close();
	}
	if (_file_reader.is_open()) {
		_file_reader.close();
	}
	delete _header;
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
	Node<K, V>* node = new Node<K, V>(k, v, level);
	return node;
}



template<typename K, typename V>
bool SkipList<K, V>::search_element(K key) {
	cout << "search_element-----------------" << endl;
	Node<K, V>* cur = this->_header;
	for (int i = _skip_list_level; i >= 0; i--) {
		//找到这一层中的节点，其next节点大于等于要查找的key
		while (cur->forwards[i] != nullptr && cur->forwards[i]->get_key() < key) {
			cur = cur->forwards[i];
			//找到之后i--向下一层继续查找，直到找到最后一层
		}
	}
	cur = cur->forwards[0];
	if (cur == nullptr) return false;
	if (cur->get_key() == key) {
		cout << "Found key: " << key << ", value: " << cur->get_value() << endl;
		return true;
	}
	cout << "Not Found :" << key  << endl;
	return false;

}


//随机生成待插入节点的next层数
template<typename K, typename V>
int SkipList<K, V>::get_random_level() {

	int k = 1;
	while (rand() % 2) {
		k++;
	}
	k = (k < _max_level) ? k : _max_level;//next层数不超过_max_level
	return k;
	


};


template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value) {
	mtx.lock();
	Node<K, V>* cur = this->_header;
	Node<K, V>* update[_max_level + 1];//把向下查询时开始向下的那个节点放入update
	memset(update, 0, sizeof(Node<K, V>*) * (_max_level + 1));

	for (int i = _skip_list_level; i >= 0; i--) {
		while (cur->forwards[i] != NULL && cur->forwards[i]->get_key() < key) {
			cur = cur->forwards[i];
		}
		update[i] = cur;
	}
	cur = cur->forwards[0]; //要插入的后面节点

	if (cur != NULL && cur->get_key() == key) {
		std::cout << "key: " << key << ", exists" << std::endl;
		mtx.unlock();
		return 1;
	}
	if (cur == NULL || cur->get_key() != key) {
		int random_level = get_random_level(); //随机生成待插入节点的层数
		//如果随机数比当前层数大，例如你插入第一个数，只有0层，但是你随机生成了3层
		if (random_level > _skip_list_level) {
			for (int i = _skip_list_level + 1; i < random_level + 1; i++) {
				update[i] = _header;
			}
			_skip_list_level = random_level;
		}

		//第0层开始插入数据
		Node<K, V>* insert_node = create_node(key, value, random_level);
		for (int i = 0; i <= random_level; i++) {
			insert_node->forwards[i] = update[i]->forwards[i]; //把插入节点的next改为update的next
			update[i]->forwards[i] = insert_node;  //update的next改为 insert_node
		}
		cout << "Successfully inserted key:" << key << ", value:" << value <<endl;
		_element_count++;

	}
	mtx.unlock();
	return 0;

}

template<typename K, typename V>
void SkipList<K, V>::display_list() {
	cout << "\n*****Skip List*****" << "\n";
	for (int i = 0; i <= _skip_list_level; i++) {
		Node<K, V>* node = this->_header->forwards[i];
		std::cout << "Level " << i << ": ";
		while (node != NULL) {
			std::cout << node->get_key() << ":" << node->get_value() << ";";
			node = node->forwards[i];
		}
		cout <<endl;
	}
}

template<typename K, typename V>
int SkipList<K, V>::size() {
	return _element_count;
}


template<typename K, typename V>
bool SkipList<K, V>::is_valid(const string& str) {

	if (str.empty()) {
		return false;
	}
	if (str.find(delimiter) == std::string::npos) {//没有找到";"
		return false;
	}
	return true;
}
template<typename K, typename V>
void SkipList<K, V>::get_key_value(const std::string& str, std::string* key, std::string* value) {

	if (!is_valid(str)) {
		return;
	}
	*key = str.substr(0, str.find(delimiter));
	*value = str.substr(str.find(delimiter) + 1, str.length());
}


template<typename K, typename V>
void SkipList<K, V>::delete_element(K key) {
	mtx.lock();
	Node<K, V>* current = this->_header;
	Node<K, V>* update[_max_level + 1];

	memset(update, 0, sizeof(Node<K, V>*) * (_max_level + 1));
	for (int i = _skip_list_level; i >= 0; i--) {
		while (current->forwards[i] != NULL && current->forwards[i]->get_key() < key) {
			current = current->forwards[i];
		}
		update[i] = current;
	}
	current = current->forwards[0];
	if (current != NULL && current->get_key() == key) {

		for (int i = 0; i <= _skip_list_level; i++) {

			if (update[i]->forwards[i] != current) //下面一层的next不是要删除的key，上面的层就可以不用判断了
				break;
			update[i]->forwards[i] = current->forwards[i];

		}
		//移除后这一层只剩下头节点，则
		while (_skip_list_level > 0 && _header->forwards[_skip_list_level] == 0) {
			_skip_list_level--;
		}
		cout << "Successfully deleted key " << key << endl;
		_element_count--;
	
	
	}
	mtx.unlock();
	return;
}
// Dump data in memory to file 
template<typename K, typename V>
void SkipList<K, V>::dump_file() {

	std::cout << "dump_file-----------------" << std::endl;
	_file_writer.open(STORE_FILE);
	Node<K, V>* node = this->_header->forwards[0];

	while (node != NULL) {
		_file_writer << node->get_key() << ":" << node->get_value() << "\n";
		std::cout << node->get_key() << ":" << node->get_value() << ";\n";
		node = node->forwards[0];
	}

	_file_writer.flush();
	_file_writer.close();
	return;
}

// Load data from disk
template<typename K, typename V>
void SkipList<K, V>::load_file() {

	_file_reader.open(STORE_FILE);
	std::cout << "load_file-----------------" << std::endl;
	std::string line;
	std::string* key = new std::string();
	std::string* value = new std::string();
	while (getline(_file_reader, line)) {
		get_key_value(line, key, value);
		if (key->empty() || value->empty()) {
			continue;
		}
		insert_element(*key, *value);
		std::cout << "key:" << *key << "value:" << *value << std::endl;
	}
	_file_reader.close();
}

#endif