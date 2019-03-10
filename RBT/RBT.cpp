
#include <cstdlib>
#include <cstdio>
#include "RBT.h"
#include <unordered_map>

// 用一个特殊节点作为 NIL 节点的好处：
// 1. 写代码时可以当成正常节点来判断颜色，不用先判断 NIL
// 2. 用来临时存储父节点时，此时在删除时可以当成正常节点来判断
// 同时也有一个隐藏坑：
// 1. 设置节点的父节点时，需要判断是否为 NIL 节点，一般情况不允许设置(临时存储可以，但是需要注意其有效性，是否被中途修改了)
struct Node* Node::NIL = new Node();

// 左旋右旋都是将当前节点降格为对应的左右子节点

Node* MinNode(Node* node)
{
	Node *cur = node;

	while (cur->left != Node::NIL)
	{
		cur = cur->left;
	}
	return cur;
}

Node* MaxNode(Node* node)
{
	Node *cur = node;
	while (cur->right != Node::NIL)
	{
		cur = cur->right;
	}

	return cur;
}

Node* Successor(Node *node)
{
	// node 不能是 NIL 节点
	Node *cur = node;

	if (cur->right != Node::NIL)
	{
		return MinNode(cur->right);
	}

	while (cur == cur->parent->right)
	{
		cur = cur->parent;
	}
	return cur;
}

Node* Predecessor(Node *node)
{
	Node *cur = node;
	if (cur->left != Node::NIL)
	{
		return MaxNode(cur->left);
	}

	while (cur == cur->parent->left)
	{
		cur = cur->parent;
	}
	return cur;
}

void LeftRotate(Node **root, Node *node)
{
	Node *right = node->right;
	Node *parent = node->parent;
	right->parent = parent;
	node->parent = right;
	node->right = right->left;
	right->left = node;
	if (Node::NIL != node->right) // 这个不要漏了
	{
		node->right->parent = node;
	}

	if (Node::NIL == parent) // 根发生变化
	{
		*root = right;
	}
	else // parent 不为 NIL 则更新
	{
		if (parent->left == node)
		{
			parent->left = right;
		}
		else
		{
			parent->right = right;
		}
	}
}

void RightRotate(Node **root, Node *node)
{
	Node *left = node->left;
	Node *parent = node->parent;
	left->parent = node->parent;
	node->parent = left;
	node->left = left->right;
	left->right = node;
	if (Node::NIL != node->left)  // 这个不要漏了
	{
		node->left->parent = node;
	}

	if (Node::NIL == parent) // 根发生变化
	{
		*root = left;
	}
	else // parent  不为 NIL
	{
		if (parent->left == node)
		{
			parent->left = left;
		}
		else
		{
			parent->right = left;
		}
	}
}

Node* Find(Node **root, TValue v)
{
	Node *cur = *root;

	while (cur != Node::NIL)
	{
		if (v == cur->key)
		{
			break;
		}
		if (cur->key > v)
		{
			cur = cur->left;
		}
		else
		{
			cur = cur->right;
		}
	}
	return cur;
}

Node* Insert(Node **root, TValue v)
{
	Node *newP = Node::NIL;			// 用于记录新节点的父节点
	Node *cur = *root;

	// 查找插入节点的位置( newP 保存新节点 v 的父节点)
	while (cur != Node::NIL)
	{
		newP = cur;
		if (cur->key < v) // 选择 v 该插入的子树
		{
			cur = cur->right;
		}
		else
		{
			cur = cur->left;
		}
	}

	Node *newNode = new Node(v);
	newNode->parent = newP;

	if (Node::NIL == newP) // 父节点为 NIL，则新节点为 root
	{
		*root = newNode;
	}
	else if (newP->key > v)
	{
		newP->left = newNode;
	}
	else
	{
		newP->right = newNode;
	}
	newNode->left = Node::NIL;
	newNode->right = Node::NIL;
	// 将新节点着色为红色，为什么是红色？尽量少违背红黑树的性质
	newNode->color = Node::Red;

	// 再平衡红黑树
	ReblanceInsert(root, newNode);
	return newNode;
}

void ReblanceInsert(Node **root, Node *newNode)
{
	/*
	 * 分三种情况：
	 * 1. 被插入节点是根节点，直接设为黑色(因为根是黑色的)
	 * 2. 被插入节点的父节点是黑色的，那么什么也不用做，因为不会违背任何红黑色性质
	 * 3. 被插入节点的父节点是红色，那么分以下三种情况讨论，
	 *		核心处理方法则是将红色节点移到根节点，然后直接设为黑色：
	 *		1) 当前节点的叔叔节点是红色
	 *		2) 当前节点的叔叔节点是黑色，且当前节点的父节点是右孩子
	 *		3) 当前节点的叔叔节点是黑色，且当前节点的父节点是左孩子
	*/
	Node *cur = newNode;
	Node *parent = cur->parent;

	if (Node::NIL != parent && Node::Black == parent->color) // 情况 2
	{
		return;
	}

	// 情况三才会进入 while 循环
	while (Node::NIL != parent && parent->color == Node::Red)
	{
		Node *gParent = parent->parent; // 如果 parent 的颜色是红色，那么根据性质，gParent 一定存在
		if (parent == gParent->left) // 父节点是祖父节点的左孩子
		{
			Node *uncle = gParent->right; // Nil 节点是黑色
			// Case 1: 叔叔节点是红色
			if (Node::Red == uncle->color) // 红色节点往根节点移动
			{
				parent->color = Node::Black;
				uncle->color = Node::Black;
				gParent->color = Node::Red;
				cur = gParent;
			}
			// Case 2: 叔叔是黑色，且当前节点是父节点的左孩子 (当前节点、当且节点的父节点都是左孩子)
			else if (cur == parent->left)
			{
				parent->color = Node::Black;
				gParent->color = Node::Red;
				RightRotate(root, gParent);		// 相当于将红色节点往根移了一层
			}
			// Case 3: 叔叔是黑色，且当且节点是父节点的右孩子
			else 
			{
				cur = parent;
				LeftRotate(root, cur);					// 将情形转移到 Case 2，然后就可以继续往根移动了
			}
		}
		else // 对称操作，父节点是祖父节点的右孩子
		{
			Node *uncle = gParent->left;
			// Case 1: 叔叔节点是红色
			if (Node::Red == uncle->color)
			{
				parent->color = Node::Black;
				uncle->color = Node::Black;
				gParent->color = Node::Red;
				cur = gParent;
			}
			// Case 2: 叔叔是黑色，且当前节点是父节点的右孩子 (当前节点、当且节点的父节点都是右孩子)
			else if (cur == parent->right)
			{
				parent->color = Node::Black;
				gParent->color = Node::Red;
				LeftRotate(root, gParent);
			}
			// Case 3: 叔叔是黑色，且当且节点是父节点的左孩子
			else
			{
				cur = parent;
				RightRotate(root, cur);			// 将情形转移到 Case 2，然后就可以继续往根移动了
			}
		}

		// cur 不可能为 NIL
		parent = cur->parent;
	}

	(*root)->color = Node::Black;
}

Node* Delete(Node **root, TValue v)
{
	Node *delNode = Find(root, v);
	if (Node::NIL == delNode) // 找不到节点
	{
		return delNode;
	}

	Node *toDel = Node::NIL; // 真正删除的节点
	if (Node::NIL == delNode->left || Node::NIL == delNode->right) // 只有一个子节点
	{
		toDel = delNode;
	}
	else // 后继节点，找前驱节点也可以
	{
		toDel = Successor(delNode);
	}

	Node *replace = Node::NIL;
	if (toDel->left != Node::NIL)
	{
		replace = toDel->left;
	}
	else
	{
		replace = toDel->right;
	}

	// 注意：replace 可能是 NIL 节点，而这里是唯一能修改 NIL 节点的地方，
	// 仅仅是用来临时保存 parent 节点，其他地方不能设置 NIL 的父节点
	Node *replaceP = toDel->parent;

	// 将真正删除的节点移除树(父节点不在指向它)
	if (Node::NIL == replaceP) // 真正删除的节点的父节点为 NIL，则 toDel == *root
	{
		*root = replace;
	}
	else if (toDel == replaceP->left) // 左子树
	{
		replaceP->left = replace;
	}
	else // 右子树
	{
		replaceP->right = replace;
	}

	if (toDel != delNode) // 要删除的节点(v)有两个子节点，不然这里就相等了
	{
		delNode->key = toDel->key; // 拷贝数据，这样就想问题转换成要删除 toDel，但是没拷贝颜色
	}

	// 真正要删除的节点为黑色，删了之后就违反了红黑树性质了，恢复一下
	// 删除之后，顶替节点分支的子树少了一个黑色节点所以需要对其进行恢复
	if (toDel->color == Node::Black)
	{
		ReblanceDelete(root, replace, replaceP);
	}
	return toDel;
}

void ReblanceDelete(Node **root, Node *node, Node *parent)
{
	/*
	 * 为了方便分析，假设 node 包含一个额外的黑色(即没删除黑色)，有如下三种情况：
	 * 1. node 是红色节点，直接将 node 设为黑色即补回了删除掉的黑色节点
	 * 2. node 是黑色节点，且 node 是根，此时整颗树的所有路径的黑色节点一起少了一，性质不变
	 * 3. node 是黑色节点，且 node 不是根，那么分为以下 4 中情况讨论：
	 *		核心思想是将 node 节点包含的额外黑色不断往树根移动，变成简单的情况 1、2，或者稍复杂的 3-1
	 *		假设 node 是父节点的左孩子，node 的兄弟节点为 Bro
	 *		1) Bro 是黑色，且 Bro 的右孩子是红色，Bro 的左孩子随意
	 *		2) Bro 是黑色，且 Bro 的右孩子是黑色，Bro 的左孩子是红色
	 *		3) Bro 是黑色，且 Bro 的两个孩子是黑色
	 *		4) Bro 是红色，根据性质，node 的父节点，Bro 的子节点都是黑色
	 *		node 是有节点的情况和左节点类似，对称操作
	 */

	Node *cur = node;
	while (cur != *root && Node::Black == cur->color)	// 情况 3
	{
		if (cur == parent->left) // 左孩子
		{
			Node *bro = parent->right;
			if (Node::Black == bro->color)
			{
				// Case 1: Bro 黑色，右孩子是红色，左孩子随意
				if (Node::Red == bro->right->color)
				{
					// 交换 bro 和 parent 的颜色
					bro->color = parent->color;
					parent->color = Node::Black;
					bro->right->color = Node::Black;
					LeftRotate(root, parent); // 补足了删除的黑色节点
					cur = *root;
				}
				else // 右孩子是黑色
				{
					// Case 2: Bro 黑色，右孩子是黑色，左孩子是红色
					if (Node::Red == bro->left->color)
					{
						bro->left->color = Node::Black;
						bro->color = Node::Red;
						RightRotate(root, bro); // 转换成  Case 1
					}
					// Case 3: Bro 黑色，两孩子是都是黑色
					else
					{
						if (bro != Node::NIL) // 易漏易错
						{
							bro->color = Node::Red; // 左右子树的黑节点数量一致了
						}
						cur = parent; // 向根转移带额外黑色的节点
					}
				}
			}
			else // Case 4: Bro 红色，则父节点，Bro 的两个子节点都是黑色
			{
				bro->color = Node::Black;
				parent->color = Node::Red;
				LeftRotate(root, parent); // 转换成了 Case 1/2/3 三种情形的一种，即 Bro 是黑色
			}
		}
		else // 对称情况
		{
			Node *bro = parent->left;
			if (Node::Black == bro->color)
			{
				// Case 1: Bro 黑色，左孩子是红色，右孩子随意
				if (Node::Red == bro->left->color)
				{
					bro->color = parent->color;
					parent->color = Node::Black;
					bro->left->color = Node::Black;
					RightRotate(root, parent); // 补足了删除的黑色节点，可以退出了
					cur = *root;
				}
				else // 左孩子是黑色
				{
					// Case 2: Bro 黑色，左孩子黑色，右孩子红色
					if (Node::Red == bro->right->color)
					{
						bro->right->color = Node::Black;
						bro->color = Node::Red;
						LeftRotate(root, bro); // 转换成 Case 1
					}
					// Case 3: Bro 黑色，两个子节点都是黑色
					else
					{
						if (bro != Node::NIL) // 易漏易错
						{
							bro->color = Node::Red; // 左右子树的黑节点数量一致了
						}
						cur = parent; // 向根转移带额外黑色的节点
					}
				}
			}
			else // Bro 是红色
			{
				bro->color = Node::Black;
				parent->color = Node::Red;
				RightRotate(root, parent);
			}
		}
		if (Node::NIL != cur) // 易漏易错
		{
			parent = cur->parent;
		}
	} // while

	cur->color = Node::Black;
}


void DrawNode(FILE *stream, Node *node, bool bLeft/* = falsle*/, bool bShowNIL/* = false*/)
{
	if (Node::NIL == node)
	{
		fprintf(stream, "%c%X[label=nil, shape=circle, style=dashed, color=Black, %s];\n", bLeft ? 'L' : 'R', (uPtr)node, bShowNIL ? "" : "style=invis");
	}
	else
	{
		fprintf(stream, "Node%X[label=\"%d\", shape=circle, color=%s, fontcolor=%s];\n",
					(uPtr)node, node->key, node->color == Node::Red ? "Red" : "Black", node->color == Node::Red ? "Red" : "Black");
	}
}

void DrawEdge(FILE *stream, Node *from, Node *to, bool bLeft, bool bShowNIL/* = false*/)
{
	if (Node::NIL == from)
	{
		return;
	}
	if (Node::NIL == to)
	{
		fprintf(stream, "Node%X -> %c%X[%s];\n", (uPtr)from, bLeft ? 'L' : 'R', (uPtr)to, bShowNIL ? "" : "style=invis");
	}
	else
	{
		fprintf(stream, "Node%X -> Node%X;\n", (uPtr)from, (uPtr)to);
	}
}

void GenDotFile(Node **root, const char* pszFileName/*="RBT.dot"*/, bool bShowNIL/*=false*/)
{
	FILE *fp = NULL;
	fp = fopen(pszFileName, "wb");

	if (NULL == fp)
	{
		printf("File to open file[%s] to write.\n", pszFileName);
		return;
	}

	fprintf(fp, "digraph RBT{\n");
	std::unordered_map<Node*, bool> visited;

	Node *cur = *root;
	const char *szInvis = bShowNIL ? "" : "style=invis";
	const char *szColor = "";
	std::string str;

	while (cur != Node::NIL)
	{
		if (visited.end() == visited.find(cur))
		{
			DrawNode(fp, cur, bShowNIL);
			if (cur->left != Node::NIL)
			{
				if (visited.end() == visited.find(cur->left))
				{
					DrawNode(fp, cur->left, bShowNIL);
					DrawEdge(fp, cur, cur->left, true, bShowNIL);
					cur = cur->left;
					continue;
				}
			}
			else
			{
				DrawNode(fp, cur->left, true, bShowNIL);
				DrawEdge(fp, cur, cur->left, true, bShowNIL);
			}

			visited[cur] = true;
			if (cur->right != Node::NIL)
			{
				if (visited.end() == visited.find(cur->right))
				{
					DrawNode(fp, cur->right, bShowNIL);
					DrawEdge(fp, cur, cur->right, false, bShowNIL);
					cur = cur->right;
					continue;
				}
			}
			else
			{
				DrawNode(fp, cur->right, false, bShowNIL);
				DrawEdge(fp, cur, cur->right, false, bShowNIL);
			}
		}
		cur = cur->parent;
	}
	fprintf(fp, "}\n");
	fclose(fp);
}

