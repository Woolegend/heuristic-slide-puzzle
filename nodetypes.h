typedef struct Node Node;
typedef struct Leaf Leaf;


struct Node{			//배열값과 부모, 자식을 연결하기 위한 노드 자료구조 
	int element[4][4];
	float fnvalue;		//평가함수값 저장 
	int depth;			//노드의 깊이값 저장 
	int unfold;			//자식을 확장한 노드의 판별을 위한 정수값 (리프노드 여부)

	Node *parent;		//부모 노드를 가리키는 포인터 

	Node *child[4];		//자식 노드를 가리키는 포인터 
};

struct Leaf{			//모든 노드들을 단순 연결 리스트로 가리키는 포인터 변수를 위한 자료구조 
	Node *leaf;			//트리의 각 노드를 가리킴 
	Leaf *next;			//다음 노드를 가리키는 포인터 	
};
