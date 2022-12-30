# DS Project 报告

* 孙一｜21307130094
* 项目仓库：[project_data_strcuture_sp2022](https://github.com/Boreas618/project_data_strcuture_sp2022)
* 完成任务：bonus

## 思路

以下分为两个部分介绍项目思路。

**bvh_parser.cpp**

核心思路：

* 通过`getline(file,line)`逐行读取bvh文件，将读取到的字符串数据生成`stringstream`，运用其重载操作符`>>`完成数据载入等一系列操作。
* `bvh`和`JSON`本质上均为树结构，因此本部分的核心是将`bvh`文件描述的关节结构用树描述出来。
* 递归建树的核心思路是**遇到`{`即入栈，遇到`}`即出栈**。具体来说，当读取到`{`时，将当前访问的节点入栈，并开始写入其子节点；每次读取到`{`均将当前访问的节点入栈，以此记录树结构的层次信息；每次读取到`}`即出栈，即子节点读取完毕。
* 考虑到`bvh`的`HIERARCHY`和`MOTION`是分开的，因此我们采用**先建立树结构的Skeleton，再依次填充MOTION数据。**

函数说明：

* `void BuildSkeleton(joint &root, ifstream &file)`

  建立`bvh`的框架结构，反映目标关节树信息

* `void GenerateMeta(META &meta_data, ifstream &file)`

  依据`bvh`文件生成元数据

* `void LoadMotions(ifstream &file, joint &root)`

  遍历先前生成的Skeleton，将Motion数据写入关节树

小插曲：

* 一开始写的时候对`stringstream`不太熟悉，以为`bvh`文件的缩进会影响数据的导入，于是手搓了一个C++版的`trim()`函数，后来发现其实没必要（具体可以看项目仓库的commit记录）

**json.cpp**

核心思路：

* 全局变量`count_of_tabs`维护缩进的`tab`数目，函数`PrintTabs`输出缩进。
* 层序遍历关节树，按照`JSON`格式输出各访问节点的信息。对于缩进的控制，采用`count_of_tabs++`或`count_of_tabs--`来控制。

函数说明：

* `void strOutputVector(vector<string> v, ofstream &outfile)`

  不难发现，`channels`数据类型为`vector<string>`，如`["Xposition", "Yposition", "Zposition", "Zrotation", "Xrotation", "Yrotation"]`，因此，本工具函数将输出`vector<string>`为上述格式。特别要注意`vector`为空或只有一个元素的情形。

* `void lfOutputVector(vector<double> v, ofstream &outfile)`

  与上面一个函数大同小异，本工具函数用于输出`vector<double>`。上面`str`和这里的`lf`分别用于表示`vector`元素的类型。

* `void PrintMetaData(ofstream &outfile, META meta_data)`

  输出元数据。

* `void PrintTabs(ofstream &outfile)`

  输出Tab，前面已经介绍。

* `void PrintJointContent(joint *&cur, ofstream &outfile)`

  输出单个关节内容，递归调用输出关节树。

## 错误处理

* 若遇文件打开失败：

  ```c++
  if (!file.is_open())
      cout << "Error: failed to open: " << argv[1] << endl;
  else
      BuildSkeleton(root, file);
  ```

* 输出JSON时遇到空节点：

  ```c++
  if(cur == NULL){
      cout << "Invalid Joint: a null joint is found in the joint tree";
      return;
  }
  ```

* 若创建`output.json`失败

  ```c++
  if (!outfile.is_open()){
      cout << "Error: failed to create: output.json" << endl;
  } else {
      joint* cur = &root;
      PrintMetaData(outfile, meta_data);
      PrintJointContent(cur, outfile);
      outfile << "}";
  }
  ```

## 可改进的地方

* 原本想要用一点设计模式，但不太熟悉C++，并且得新冠了，所以没有写得下去。
* 错误处理并不完善
* 或许可以用非递归方法？（虽然目前的速度还行）

## 致谢

感谢助教团队和张老师～