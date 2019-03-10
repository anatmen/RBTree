
#include <cstdlib>
#include <cstdio>
#include "RBT.h"
#include <unordered_map>

// ��һ������ڵ���Ϊ NIL �ڵ�ĺô���
// 1. д����ʱ���Ե��������ڵ����ж���ɫ���������ж� NIL
// 2. ������ʱ�洢���ڵ�ʱ����ʱ��ɾ��ʱ���Ե��������ڵ����ж�
// ͬʱҲ��һ�����ؿӣ�
// 1. ���ýڵ�ĸ��ڵ�ʱ����Ҫ�ж��Ƿ�Ϊ NIL �ڵ㣬һ���������������(��ʱ�洢���ԣ�������Ҫע������Ч�ԣ��Ƿ���;�޸���)
struct Node* Node::NIL = new Node();

// �����������ǽ���ǰ�ڵ㽵��Ϊ��Ӧ�������ӽڵ�

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
	// node ������ NIL �ڵ�
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
	if (Node::NIL != node->right) // �����Ҫ©��
	{
		node->right->parent = node;
	}

	if (Node::NIL == parent) // �������仯
	{
		*root = right;
	}
	else // parent ��Ϊ NIL �����
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
	if (Node::NIL != node->left)  // �����Ҫ©��
	{
		node->left->parent = node;
	}

	if (Node::NIL == parent) // �������仯
	{
		*root = left;
	}
	else // parent  ��Ϊ NIL
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
	Node *newP = Node::NIL;			// ���ڼ�¼�½ڵ�ĸ��ڵ�
	Node *cur = *root;

	// ���Ҳ���ڵ��λ��( newP �����½ڵ� v �ĸ��ڵ�)
	while (cur != Node::NIL)
	{
		newP = cur;
		if (cur->key < v) // ѡ�� v �ò��������
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

	if (Node::NIL == newP) // ���ڵ�Ϊ NIL�����½ڵ�Ϊ root
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
	// ���½ڵ���ɫΪ��ɫ��Ϊʲô�Ǻ�ɫ��������Υ�������������
	newNode->color = Node::Red;

	// ��ƽ������
	ReblanceInsert(root, newNode);
	return newNode;
}

void ReblanceInsert(Node **root, Node *newNode)
{
	/*
	 * �����������
	 * 1. ������ڵ��Ǹ��ڵ㣬ֱ����Ϊ��ɫ(��Ϊ���Ǻ�ɫ��)
	 * 2. ������ڵ�ĸ��ڵ��Ǻ�ɫ�ģ���ôʲôҲ����������Ϊ����Υ���κκ��ɫ����
	 * 3. ������ڵ�ĸ��ڵ��Ǻ�ɫ����ô����������������ۣ�
	 *		���Ĵ��������ǽ���ɫ�ڵ��Ƶ����ڵ㣬Ȼ��ֱ����Ϊ��ɫ��
	 *		1) ��ǰ�ڵ������ڵ��Ǻ�ɫ
	 *		2) ��ǰ�ڵ������ڵ��Ǻ�ɫ���ҵ�ǰ�ڵ�ĸ��ڵ����Һ���
	 *		3) ��ǰ�ڵ������ڵ��Ǻ�ɫ���ҵ�ǰ�ڵ�ĸ��ڵ�������
	*/
	Node *cur = newNode;
	Node *parent = cur->parent;

	if (Node::NIL != parent && Node::Black == parent->color) // ��� 2
	{
		return;
	}

	// ������Ż���� while ѭ��
	while (Node::NIL != parent && parent->color == Node::Red)
	{
		Node *gParent = parent->parent; // ��� parent ����ɫ�Ǻ�ɫ����ô�������ʣ�gParent һ������
		if (parent == gParent->left) // ���ڵ����游�ڵ������
		{
			Node *uncle = gParent->right; // Nil �ڵ��Ǻ�ɫ
			// Case 1: ����ڵ��Ǻ�ɫ
			if (Node::Red == uncle->color) // ��ɫ�ڵ������ڵ��ƶ�
			{
				parent->color = Node::Black;
				uncle->color = Node::Black;
				gParent->color = Node::Red;
				cur = gParent;
			}
			// Case 2: �����Ǻ�ɫ���ҵ�ǰ�ڵ��Ǹ��ڵ������ (��ǰ�ڵ㡢���ҽڵ�ĸ��ڵ㶼������)
			else if (cur == parent->left)
			{
				parent->color = Node::Black;
				gParent->color = Node::Red;
				RightRotate(root, gParent);		// �൱�ڽ���ɫ�ڵ���������һ��
			}
			// Case 3: �����Ǻ�ɫ���ҵ��ҽڵ��Ǹ��ڵ���Һ���
			else 
			{
				cur = parent;
				LeftRotate(root, cur);					// ������ת�Ƶ� Case 2��Ȼ��Ϳ��Լ��������ƶ���
			}
		}
		else // �ԳƲ��������ڵ����游�ڵ���Һ���
		{
			Node *uncle = gParent->left;
			// Case 1: ����ڵ��Ǻ�ɫ
			if (Node::Red == uncle->color)
			{
				parent->color = Node::Black;
				uncle->color = Node::Black;
				gParent->color = Node::Red;
				cur = gParent;
			}
			// Case 2: �����Ǻ�ɫ���ҵ�ǰ�ڵ��Ǹ��ڵ���Һ��� (��ǰ�ڵ㡢���ҽڵ�ĸ��ڵ㶼���Һ���)
			else if (cur == parent->right)
			{
				parent->color = Node::Black;
				gParent->color = Node::Red;
				LeftRotate(root, gParent);
			}
			// Case 3: �����Ǻ�ɫ���ҵ��ҽڵ��Ǹ��ڵ������
			else
			{
				cur = parent;
				RightRotate(root, cur);			// ������ת�Ƶ� Case 2��Ȼ��Ϳ��Լ��������ƶ���
			}
		}

		// cur ������Ϊ NIL
		parent = cur->parent;
	}

	(*root)->color = Node::Black;
}

Node* Delete(Node **root, TValue v)
{
	Node *delNode = Find(root, v);
	if (Node::NIL == delNode) // �Ҳ����ڵ�
	{
		return delNode;
	}

	Node *toDel = Node::NIL; // ����ɾ���Ľڵ�
	if (Node::NIL == delNode->left || Node::NIL == delNode->right) // ֻ��һ���ӽڵ�
	{
		toDel = delNode;
	}
	else // ��̽ڵ㣬��ǰ���ڵ�Ҳ����
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

	// ע�⣺replace ������ NIL �ڵ㣬��������Ψһ���޸� NIL �ڵ�ĵط���
	// ������������ʱ���� parent �ڵ㣬�����ط��������� NIL �ĸ��ڵ�
	Node *replaceP = toDel->parent;

	// ������ɾ���Ľڵ��Ƴ���(���ڵ㲻��ָ����)
	if (Node::NIL == replaceP) // ����ɾ���Ľڵ�ĸ��ڵ�Ϊ NIL���� toDel == *root
	{
		*root = replace;
	}
	else if (toDel == replaceP->left) // ������
	{
		replaceP->left = replace;
	}
	else // ������
	{
		replaceP->right = replace;
	}

	if (toDel != delNode) // Ҫɾ���Ľڵ�(v)�������ӽڵ㣬��Ȼ����������
	{
		delNode->key = toDel->key; // �������ݣ�������������ת����Ҫɾ�� toDel������û������ɫ
	}

	// ����Ҫɾ���Ľڵ�Ϊ��ɫ��ɾ��֮���Υ���˺���������ˣ��ָ�һ��
	// ɾ��֮�󣬶���ڵ��֧����������һ����ɫ�ڵ�������Ҫ������лָ�
	if (toDel->color == Node::Black)
	{
		ReblanceDelete(root, replace, replaceP);
	}
	return toDel;
}

void ReblanceDelete(Node **root, Node *node, Node *parent)
{
	/*
	 * Ϊ�˷������������ node ����һ������ĺ�ɫ(��ûɾ����ɫ)�����������������
	 * 1. node �Ǻ�ɫ�ڵ㣬ֱ�ӽ� node ��Ϊ��ɫ��������ɾ�����ĺ�ɫ�ڵ�
	 * 2. node �Ǻ�ɫ�ڵ㣬�� node �Ǹ�����ʱ������������·���ĺ�ɫ�ڵ�һ������һ�����ʲ���
	 * 3. node �Ǻ�ɫ�ڵ㣬�� node ���Ǹ�����ô��Ϊ���� 4 ��������ۣ�
	 *		����˼���ǽ� node �ڵ�����Ķ����ɫ�����������ƶ�����ɼ򵥵���� 1��2�������Ը��ӵ� 3-1
	 *		���� node �Ǹ��ڵ�����ӣ�node ���ֵܽڵ�Ϊ Bro
	 *		1) Bro �Ǻ�ɫ���� Bro ���Һ����Ǻ�ɫ��Bro ����������
	 *		2) Bro �Ǻ�ɫ���� Bro ���Һ����Ǻ�ɫ��Bro �������Ǻ�ɫ
	 *		3) Bro �Ǻ�ɫ���� Bro �����������Ǻ�ɫ
	 *		4) Bro �Ǻ�ɫ���������ʣ�node �ĸ��ڵ㣬Bro ���ӽڵ㶼�Ǻ�ɫ
	 *		node ���нڵ���������ڵ����ƣ��ԳƲ���
	 */

	Node *cur = node;
	while (cur != *root && Node::Black == cur->color)	// ��� 3
	{
		if (cur == parent->left) // ����
		{
			Node *bro = parent->right;
			if (Node::Black == bro->color)
			{
				// Case 1: Bro ��ɫ���Һ����Ǻ�ɫ����������
				if (Node::Red == bro->right->color)
				{
					// ���� bro �� parent ����ɫ
					bro->color = parent->color;
					parent->color = Node::Black;
					bro->right->color = Node::Black;
					LeftRotate(root, parent); // ������ɾ���ĺ�ɫ�ڵ�
					cur = *root;
				}
				else // �Һ����Ǻ�ɫ
				{
					// Case 2: Bro ��ɫ���Һ����Ǻ�ɫ�������Ǻ�ɫ
					if (Node::Red == bro->left->color)
					{
						bro->left->color = Node::Black;
						bro->color = Node::Red;
						RightRotate(root, bro); // ת����  Case 1
					}
					// Case 3: Bro ��ɫ���������Ƕ��Ǻ�ɫ
					else
					{
						if (bro != Node::NIL) // ��©�״�
						{
							bro->color = Node::Red; // ���������ĺڽڵ�����һ����
						}
						cur = parent; // ���ת�ƴ������ɫ�Ľڵ�
					}
				}
			}
			else // Case 4: Bro ��ɫ���򸸽ڵ㣬Bro �������ӽڵ㶼�Ǻ�ɫ
			{
				bro->color = Node::Black;
				parent->color = Node::Red;
				LeftRotate(root, parent); // ת������ Case 1/2/3 �������ε�һ�֣��� Bro �Ǻ�ɫ
			}
		}
		else // �Գ����
		{
			Node *bro = parent->left;
			if (Node::Black == bro->color)
			{
				// Case 1: Bro ��ɫ�������Ǻ�ɫ���Һ�������
				if (Node::Red == bro->left->color)
				{
					bro->color = parent->color;
					parent->color = Node::Black;
					bro->left->color = Node::Black;
					RightRotate(root, parent); // ������ɾ���ĺ�ɫ�ڵ㣬�����˳���
					cur = *root;
				}
				else // �����Ǻ�ɫ
				{
					// Case 2: Bro ��ɫ�����Ӻ�ɫ���Һ��Ӻ�ɫ
					if (Node::Red == bro->right->color)
					{
						bro->right->color = Node::Black;
						bro->color = Node::Red;
						LeftRotate(root, bro); // ת���� Case 1
					}
					// Case 3: Bro ��ɫ�������ӽڵ㶼�Ǻ�ɫ
					else
					{
						if (bro != Node::NIL) // ��©�״�
						{
							bro->color = Node::Red; // ���������ĺڽڵ�����һ����
						}
						cur = parent; // ���ת�ƴ������ɫ�Ľڵ�
					}
				}
			}
			else // Bro �Ǻ�ɫ
			{
				bro->color = Node::Black;
				parent->color = Node::Red;
				RightRotate(root, parent);
			}
		}
		if (Node::NIL != cur) // ��©�״�
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

