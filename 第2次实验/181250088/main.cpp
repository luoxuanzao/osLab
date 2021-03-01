#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<map>


using namespace std;

extern "C" {
	void asm_print(const char *, const int);
}
typedef unsigned char u8;	//1字节
typedef unsigned short u16;	//2字节
typedef unsigned int u32;	//4字节


int  BytsPerSec;	//每扇区字节数
int  SecPerClus;	//每簇扇区数
int  RsvdSecCnt;	//Boot记录占用的扇区数
int  NumFATs;	//FAT表个数
int  RootEntCnt;	//根目录最大文件数
int  FATSz;	//FAT扇区数
string out;

#pragma pack (1) /*指定按1字节对齐*/

//偏移11个字节
struct FAT12Head {
	u16  BPB_BytsPerSec;	//每扇区字节数
	u8   BPB_SecPerClus;	//每簇扇区数
	u16  BPB_RsvdSecCnt;	//Boot记录占用的扇区数
	u8   BPB_NumFATs;	//FAT表个数
	u16  BPB_RootEntCnt;	//根目录最大文件数
	u16  BPB_TotSec16;		//扇区总数
	u8   BPB_Media;		//介质描述符
	u16  BPB_FATSz16;	//FAT扇区数
	u16  BPB_SecPerTrk;  //	每磁道扇区数（Sector/track）
	u16  BPB_NumHeads;	//磁头数（面数）
	u32  BPB_HiddSec;	//隐藏扇区数
	u32  BPB_TotSec32;	//如果BPB_FATSz16为0，该值为FAT扇区数
};
//BPB至此结束，长度25字节

//根目录条目
struct RootEntry {
	char DIR_Name[11];
	u8   DIR_Attr;		//文件属性
	char reserved[10];
	u16  DIR_WrtTime;
	u16  DIR_WrtDate;
	u16  DIR_FstClus;	//开始簇号
	u32  DIR_FileSize;
};

class Node{
public:
	string name;
	string path;
	vector<Node*> children;
	int dirCount;
	int fileCount;
	bool isDir;
	bool isRoot;
	char content[10000];
	int size;
	Node(string name,bool isDir){
		this->name = name;
		this->dirCount = 0;
		this->fileCount = 0;
		this->isDir = isDir;
		this->isRoot = false;
	}

};
void fillBPB(FILE * fat12, struct FAT12Head* bpb_ptr);	//载入BPB
void readFile(FILE * fat12,Node* root);
void readContent(FILE * fat12,Node * file,int current);
void readChildren(FILE*	fat12,Node *node,int current);
int calFat(FILE *fat12,int currentVal);
void showResult(Node * root,map<string,Node*>* mapNode);
void split(const string &s, vector<string> &sv, const char flag = ' ');
void showNode(Node * root);
void showDetailNode(Node * root);
void myPrint(const char* p);

int main() {
	FILE* fat12;
	fat12 = fopen("/home/admin/os/tempLab02/check/a.img", "rb");	//打开FAT12的映像文件

	struct FAT12Head head;
	struct FAT12Head* head_ptr = &head;
	
	//载入BPB
	fillBPB(fat12, head_ptr);
    

	BytsPerSec = head_ptr->BPB_BytsPerSec;	//每扇区字节数
	SecPerClus = head_ptr->BPB_SecPerClus;	//每簇扇区数
	RsvdSecCnt = head_ptr->BPB_RsvdSecCnt;	//Boot记录占用的扇区数
	NumFATs = head_ptr->BPB_NumFATs;	//FAT表个数
	RootEntCnt = head_ptr->BPB_RootEntCnt;	//根目录最大文件数
	FATSz = head_ptr->BPB_FATSz16;
	

	Node root("/",true);
	root.isRoot = true;
	root.path = "/";
	Node* root_ptr = &root;
	
	readFile(fat12,root_ptr);

	// cout<<"结束!"<<endl;
	map<string,Node*> mapNode ;
	mapNode.insert(pair<string,Node*>(root.path,root_ptr));
	showResult(&root,&mapNode);
	Node* temp;

	while(true){
		// cout<<">";
		myPrint(">");
		string input;
		vector<string> input_list;
		getline(cin,input);
		split(input, input_list, ' ');
		for (auto it = input_list.begin(); it != input_list.end();) {//删除数组中空格产生的空位置
			if (*it=="") {
				it = input_list.erase(it);
			}
			else {
				it++;
			}
		}
		if(input_list[0].compare("exit") == 0){
			
			// cout<<"拜拜"<<endl;
			myPrint("拜拜\n");
			fclose(fat12);
			return 0;
		}
		else if(input_list[0].compare("ls") == 0){
			if(input_list.size() == 1){
				auto iter = mapNode.find("/");
				if(iter != mapNode.end()){
					temp = iter->second;
				}
				showNode(temp);	
			}
			else{
				//ls -l
				if(input_list[1].compare("-l")==0||input_list[1].compare("-ll")==0){
					if(input_list.size() == 2){
						auto iter = mapNode.find("/");
						if(iter != mapNode.end()){
							temp = iter->second;
							showDetailNode(temp);
						}
					}
					else{
						auto iter = mapNode.find(input_list[2]+"/");
						if(iter != mapNode.end()){
							temp = iter->second;
						}
						showDetailNode(temp);
					}
				}
				else if(input_list.size()==3 && (input_list[2].compare("-l")==0 || input_list[2].compare("-ll")==0)){
					auto iter = mapNode.find(input_list[1]+"/");
						if(iter != mapNode.end()){
							temp = iter->second;
							showDetailNode(temp);
						}
				}
				//


				// auto iter = mapNode.find(input_list[1]);
				// if(iter != mapNode.end()){
				// 	temp = iter->second;
				// 	cout<<"喜喜"<<endl;
				// }
				// else{
				// 	cout<<"找不到该目录"<<endl;
				// }
			}
		}
		else if(input_list[0].compare("cat") == 0){
			if(input_list.size() == 1){
				// cout<<"缺少文件名"<<endl;
				myPrint("缺少文件名\n");
			}
			else{
				temp = mapNode[input_list[1]];

				// cout<<temp->content<<endl;	
				out = temp->content;
				myPrint(out.c_str());
			}

		}
	}
	return 0 ;
}

void showNode(Node * root){
	out = root->path;
	out += ":\n";
	myPrint(out.c_str());
	// cout<<(root->path)<<":"<<endl;
	if(!root->isRoot){
		out = "\033[31m";
		out += ". .. ";
		out += "\033[0m";
		myPrint(out.c_str());
		// cout<<"\033[31m"<<"."<<" "<<".."<<"\033[0m"<<" ";
	}
	vector<Node*>children = root->children;
	for(int i=0;i<children.size();i++){
		if(children[i]->isDir){
			out = "\033[31m";
			out += children[i]->name;
			out += "\033[0m";
			out += " ";
			myPrint(out.c_str());
			// cout<<"\033[31m"<<children[i]->name<<"  "<<"\033[0m";//这里需要红色输出
		}
		else{
			out = children[i]->name;
			out+=" ";
			myPrint(out.c_str());
			// cout<<children[i]->name<<"  ";
		}
	}	
	cout<<endl;
	for(int i=0;i<children.size();i++){
		if(children[i]->isDir){
			showNode(children[i]);
		}
	}
}

void showDetailNode(Node * root){
	out = root->path +" "+ to_string(root->dirCount)+" "+to_string(root->fileCount)+":\n";
	// cout<<(root->path+" "+ to_string(root->dirCount)+" "+to_string(root->fileCount)+":\n");
	myPrint(out.c_str());
	vector<Node*>children = root->children;
	if(!root->isRoot){
		out = "\033[31m";
		out += ".\n..";
		out += "\033[0m";
		out += "\n";
		myPrint(out.c_str());
		// cout<<"\033[31m"<<"."<<endl;
		// cout<<".."<<"\033[0m"<<endl;
	}
	for(int i=0;i<children.size();i++){
		if(children[i]->isDir){
			out = "\033[31m";
			out += children[i]->name;
			out += "\033[0m";
			out += " ";
			out += to_string(children[i]->dirCount);
			out += " ";
			out += to_string(children[i]->fileCount);
			out += "\n";
			myPrint(out.c_str());
			// cout<<"\033[31m"<<children[i]->name<<"  "<<"\033[0m";//这里需要红色输出
			// cout<<children[i]->dirCount<<" "<<children[i]->fileCount<<endl;
		}
		else{
			out = children[i]->name;
			out += "  ";
			out += to_string(children[i]->size);
			out += "\n";
			myPrint(out.c_str());
			// cout<<children[i]->name<<"  ";
			// cout<<children[i]->size<<endl;
		}
	}
	cout<<endl;
	for(int i=0;i<children.size();i++){
		if(children[i]->isDir){
			showDetailNode(children[i]);
		}
	}	
	

}

void showResult(Node * root, map<string,Node*>* mapNode){
	mapNode->insert(pair<string,Node*>(root->path,root));
	if(root->isDir){
		// cout<<"文件夹"<<root->name<<endl;
		vector<Node *> nodeList = root->children;
		for(int i=0;i<nodeList.size();i++){
			showResult(nodeList[i],mapNode);
		}
	}

}

void readFile(FILE * fat12,Node* root){
	int base = BytsPerSec*(RsvdSecCnt + NumFATs * FATSz);
	int count = 0;
	int check = 0;
	while(count < RootEntCnt){
		check = fseek(fat12,base+count*32,SEEK_SET);
		if(check == -1){
			// cout<<"定位根目录失败了"<<endl;
			myPrint("定位根目录失败了\n");
			break;
		}
		RootEntry entry;
		RootEntry *entry_ptr = &entry;
		check = fread(entry_ptr,1,32,fat12);
		if(check != 32){
			// cout<<"entry读取失败了"<<endl;
			myPrint("entry读取失败了\n");
		}
		count++;
		if(entry_ptr->DIR_Name[0] == '\0') {continue;}
		int j;
		int boolean = 0;
		for (j = 0; j < 11; j++) {
			if (!(((entry_ptr->DIR_Name[j] >= 48) && (entry_ptr->DIR_Name[j] <= 57)) ||
				((entry_ptr->DIR_Name[j] >= 65) && (entry_ptr->DIR_Name[j] <= 90)) ||
				((entry_ptr->DIR_Name[j] >= 97) && (entry_ptr->DIR_Name[j] <= 122)) ||
				(entry_ptr->DIR_Name[j] == ' '))) {
				boolean = 1;	//非英文及数字、空格
				break;
			}
		}
		if (boolean == 1) continue;	//非目标文件不输出
		
		if((entry_ptr->DIR_Attr & 0x10) == 0){
			//文件
			char temp[12];
			int point=0;
			int k = 0;
			while(k<12 && entry_ptr->DIR_Name[k] != ' '){
				temp[point++] = entry_ptr->DIR_Name[k++];
			}
			temp[point++] = '.';
			while(entry_ptr->DIR_Name[k] == ' '){
				k++;
			}
		
			while(k<12 && entry_ptr->DIR_Name[k]!=' '){
				temp[point++] = entry_ptr->DIR_Name[k++];
			}
			while(point<12){temp[point++] = '\0';}
			Node *f = new Node(temp,false);
			f->path = root->path+ f->name;
			root->children.push_back(f);
			root->fileCount++;
			readContent(fat12,f,entry_ptr->DIR_FstClus);
			// cout<<"文件"<<f->name<<endl;
			// cout<<"内容"<<f->content<<endl;
		}
		else{
			//目录
			char temp[12];
			int point = 0;
			int k=0;
			while(k<12 && entry_ptr->DIR_Name[k] != ' '){
				temp[point++] = entry_ptr->DIR_Name[k++];
			}
			while(point<12){temp[point++] = '\0';}
			Node *dir = new Node(temp,true);
			dir->path = root->path + dir->name+'/';
			root->children.push_back(dir);
			root->dirCount++;
			// cout<<"目录"<<dir->name<<endl;
			readChildren(fat12,dir,entry_ptr->DIR_FstClus);
		}
		
	}

}
void readChildren(FILE*	fat12,Node *node,int current){
	int dataBase = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
	int currentClus = current;
	int count = 0;
	int check = 0;
	int value = 0;
	while(value<0xFF8){
		value = calFat(fat12, currentClus);//获取下一个簇
		if (value == 0xFF7
			) {
			cout<<"坏簇，读取失败!\n";
			break;
		}
		while(count<BytsPerSec){
			int startByte = dataBase+(currentClus-2)*SecPerClus*BytsPerSec+count;
			check = fseek(fat12,startByte,SEEK_SET);
			if(check == -1){
				cout<<"子文件夹定位错误";
			}
			RootEntry entry;
			RootEntry *entry_ptr = &entry;
			check = fread(entry_ptr,1,32,fat12);
			if(check != 32){
				cout<<"子文件夹读取错误";
			}
			count+=32;


			if(entry_ptr->DIR_Name[0] == '\0') {continue;}
			int j;
			int boolean = 0;
			for (j = 0; j < 11; j++) {
				if (!(((entry_ptr->DIR_Name[j] >= 48) && (entry_ptr->DIR_Name[j] <= 57)) ||
				((entry_ptr->DIR_Name[j] >= 65) && (entry_ptr->DIR_Name[j] <= 90)) ||
				((entry_ptr->DIR_Name[j] >= 97) && (entry_ptr->DIR_Name[j] <= 122)) ||
				(entry_ptr->DIR_Name[j] == ' '))) {
				boolean = 1;	//非英文及数字、空格
				break;
			}
		}
			if (boolean == 1) continue;	//非目标文件不输出

			if((entry_ptr->DIR_Attr & 0x10) == 0){
				//文件
				char temp[12];
				int point=0;
				int k = 0;
				while(k<12 && entry_ptr->DIR_Name[k] != ' '){
					temp[point++] = entry_ptr->DIR_Name[k++];
				}
				temp[point++] = '.';
				while(entry_ptr->DIR_Name[k] == ' '){
						k++;
				}
		
				while(k<12 && entry_ptr->DIR_Name[k]!=' '){
					temp[point++] = entry_ptr->DIR_Name[k++];
				}
				while(point<12){temp[point++] = '\0';}
				Node *f = new Node(temp,false);
				f->path = node->path+ f->name;
				node->children.push_back(f);
				node->fileCount++;
				readContent(fat12,f,entry_ptr->DIR_FstClus);
				// cout<<"子文件夹文件"<<f->name<<endl;
				// cout<<"子文件夹内容"<<f->content<<endl;
			}
			else{//目录
				char temp[12];
				int point = 0;
				int k=0;
				while(k<12 && entry_ptr->DIR_Name[k] != ' '){
					temp[point++] = entry_ptr->DIR_Name[k++];
				}
				while(point<12){temp[point++] = '\0';}
				Node *dir = new Node(temp,true);
				dir->path = node->path + dir->name+'/';
				node->children.push_back(dir);
				node->dirCount++;
				// cout<<"子文件夹目录"<<dir->name<<endl;
				readChildren(fat12,dir,entry_ptr->DIR_FstClus);
			}
			

		}
	}
	return;


}

void readContent(FILE * fat12,Node * file,int current){
int dataBase = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
	int currentClus = current;
	int value = 0;		//这里用value来进行不同簇的读取（超过512字节）
	char *p = file->content;
	if (currentClus == 0) {
		return;
	}
	int size = 0;
	while (value < 0xFF8) {
		value = calFat(fat12, currentClus);//获取下一个簇
		if (value == 0xFF7
			) {
			cout<<"坏簇，读取失败!\n";
			break;
		}
		char* str = (char*)malloc(SecPerClus*BytsPerSec);	//暂存从簇中读出的数据
		char *content = str;
		int startByte = dataBase + (currentClus - 2)*SecPerClus*BytsPerSec;
		int check;
		check = fseek(fat12, startByte, SEEK_SET);
		if (check == -1)
			cout<<"fseek in printChildren failed!";

		check = fread(content, 1, SecPerClus*BytsPerSec, fat12);//提取数据
		if (check != SecPerClus * BytsPerSec)
			cout<<"fread in printChildren failed!";

		int count = SecPerClus * BytsPerSec;
		int loop = 0;
		for (int i = 0; i < count; i++) {//读取赋值
			size++;
			*p = content[i];
			p++;
		}
		free(str);
		currentClus = value;
	}
	file->size = strlen(file->content);

}



void split(const string &s, vector<string> &sv, const char flag ) {
	sv.clear();
	istringstream iss(s);
	string temp;
	while (getline(iss, temp, flag)) {
		sv.push_back(temp);
	}
	return;
}

int calFat(FILE *fat12,int num){
	int fatBase = RsvdSecCnt * BytsPerSec;
	//FAT项的偏移字节
	int fatPos = fatBase + num * 3 / 2;
	//奇偶FAT项处理方式不同，分类进行处理，从0号FAT项开始
	int type = 0;
	if (num % 2 == 0) {
		type = 0;
	}
	else {
		type = 1;
	}

	//先读出FAT项所在的两个字节
	u16 bytes;
	u16* bytes_ptr = &bytes;
	int check;
	check = fseek(fat12, fatPos, SEEK_SET);
	if (check == -1)
		cout<<"fseek in getFATValue failed!";

	check = fread(bytes_ptr, 1, 2, fat12);
	if (check != 2)
		cout<<"fread in getFATValue failed!";

	//u16为short，结合存储的小尾顺序和FAT项结构可以得到
	//type为0的话，取byte2的低4位和byte1构成的值，type为1的话，取byte2和byte1的高4位构成的值
	if (type == 0) {
		bytes = bytes << 4;   //这里原文错误，原理建议看网上关于FAT表的文章
		return bytes >> 4;
	}
	else {
		return bytes >> 4;
	}
}


void myPrint(const char* p){
	asm_print(p,strlen(p));
}

void fillBPB(FILE* fat12, struct FAT12Head* head_ptr) {  //读取boot信息
	int check;

	//BPB从偏移11个字节处开始
	check = fseek(fat12, 11, SEEK_SET);
	if (check == -1)
		cout << "fseek in fillBPB failed!\n";

	//BPB长度为25字节
	check = fread(head_ptr, 1, 25, fat12);
	if (check != 25)
		cout<<"fread in fillBPB failed!\n";
}
