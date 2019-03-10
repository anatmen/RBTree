#ifndef __RBT_H__
#define __RBT_H__

#include <cstdio>

#ifndef NULL
#define NULL			0
#endif

#define JLDEBUG			1

typedef	unsigned int		uPtr;
typedef int					TValue;

struct Node
{
	struct Node *left;
	struct Node *right;
	struct Node *parent;
	static struct Node *NIL;

	enum { Red, Black } color;
	TValue key;

	Node() : left(this), right(this), parent(this), key(0xFFFFFFFF), color(Node::Black){} // 空节点是黑色
	Node(TValue k) : left(Node::NIL), right(Node::NIL), parent(Node::NIL), key(k) {}
};

Node* MinNode(Node* node);
Node* MaxNode(Node* node);
Node* Successor(Node *node);
Node* Predecessor(Node *node);
void LeftRotate(Node **root, Node *node);
void RightRotate(Node **root, Node *node);
Node* Find(Node **root, TValue v);
Node* Insert(Node **root, TValue v);
void ReblanceInsert(Node **root, Node *newNode);
Node* Delete(Node **root, TValue v);
void ReblanceDelete(Node **root, Node *delNode, Node *parent);
Node* GetNodeByRank(Node *root, size_t rank);
size_t GetRankByNode(Node **root, Node *node);
void DrawNode(FILE *stream, Node *node, bool bLeft = false, bool bShowNIL = false);
void DrawEdge(FILE *stream, Node *from, Node *to, bool bLeft, bool bShowNIL = false);
void GenDotFile(Node **root, const char* pszFileName = "RBT.dot", bool bShowNIL = false);

#endif
