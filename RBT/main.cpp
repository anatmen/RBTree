/*
 * 可视化红黑树插入删除网站: https://www.cs.usfca.edu/~galles/visualization/RedBlack.html
 * dot -> 图形
 * 执行命令：dot -Tpng RBT.dot -o RBT.png
 */
#include <cstdio>
#include <cstdlib>
#include <random>
#include <ctime>
#include <unordered_map>
#include "RBT.h"


int Help(const char* szSelf)
{
	printf("i - insert node\n"
			 "d - delete node\n"
			 "o - generate dot file\n"
			 "s - show tree\n"
			 "q - quit\n"
			 "? - this help\n"
			 "h - this help\n"
			);
	/*
	 * -o 输出 dot 描述
	 * -n 是否显示 NIL
	 * -f 文件输入
	 * -r 随机产生输入
	 * -c 清空树
	*/
	return 0;
}

int main(int argc, char** argv)
{
	char cmd = 0;
	//char szParam[64];
	int val;
	Node *root = Node::NIL;
	struct INPUT
	{
		char cmd;
		int val;
	};
	std::vector<struct INPUT> inputs;
	Node *node;
	bool bRunning = true;
	struct INPUT in;
	while (bRunning)
	{
		if (cmd != '\n' && cmd != '\r' && cmd != ' ')
		{
			printf("Your cmd > ");
		}
		cmd = getchar();
		switch (cmd)
		{
		case 'i': // 插入节点
			scanf("%d", &val);
			in.cmd = cmd;
			in.val = val;
			inputs.push_back(in);
			node = Insert(&root, val);
			printf("Insert node with val=%d!\n", val);
			break;

		case 'd': // 删除节点
			scanf("%d", &val);
			in.cmd = cmd;
			in.val = val;
			inputs.push_back(in);
			node = Delete(&root, val);
			if (Node::NIL == node)
			{
				printf("Node with val=%d NOT FOUND!\n", val);
			}
			else
			{
				delete node;
				node = NULL;
			}
			break;

		case 'o': // 生成 dot 文件
			GenDotFile(&root, "RBT.dot", false);
			break;

		case 's': // 显示图片
			GenDotFile(&root, "tmp.dot", false);
			system("dot -Tpng tmp.dot -o tmp.png");
			system("tmp.png");
			break;

		case 'l': // 显示历史记录
			for (size_t i = 0; i < inputs.size(); ++i)
			{
				printf("cmd: %c %d\n", inputs[i].cmd, inputs[i].val);
			}
			break;

		case 'c': // 清空树
			break;

		case 'q': // Quit
			bRunning = false;
			break;

		case '\n':
		case '\r':
		case ' ':
			break;

		case '?':
		case 'h':
		default:
			Help("");
		}
	}

	return 0;
}