#pragma warning(disable: 4996)

#include <iostream>
#include <stdlib.h>
#include "nodetypes.h"
#include "prototypes.h"
#include <math.h>

using namespace std;

Node *Root;		//�ʱ� ��Ʈ ��� ����(���� ����)
Node *Current;	//�ӽ� ������ ����
Leaf *Support;	//��� ������ �����ϱ� ���� ��ǥ ������ 
Leaf *Last;		//Support ����Ʈ�� �� ��带 ����Ŵ(��� �̾ �����ϱ� ����)

int	SS[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0}};	//��ǥ ����
int IP[4][4];		//  �ʱ� ���� ���� 

int ChildNum=0;		// Ȯ��� ������ ī��Ʈ�ϱ� ���� ���� ���� 
int sa, sh, sb;		// ���� ���õ� �˰���, �޸���ƽ, ���� ��ȣ

// @Woolegend
// �� ������ ��ǥ ��ǥ�� �����ϴ� �迭
// 0�� 16�� �����ϰ� ����� {3,3}�� �� �� �����
int cord[17][2] = {{3,3},{0,0},{0,1},{0,2},{0,3},{1,0},{1,1},{1,2},{1,3},{2,0},{2,1},{2,2},{2,3},{3,0},{3,1},{3,2},{3,3}};


// ���带 �����ϱ� ���� �Լ�
// ���õ� ������ ��ȣ�� �������� sb�� ����ȴ�
void selectBoard() {
	printf("Board level : 1 ~ 8\n");
	printf("> ");
	scanf("%d", &sb);
	printf("\n");
}

// �˰����� �����ϱ� ���� �Լ�
// ���õ� �˰����� ��ȣ�� �������� sa�� ����ȴ�
void selectAlgorithm() {
	printf("Algorithm\n");
	printf("1. A* Algorithm		: f(n) = e(n) + h(n)\n");
	printf("2. Emphasize h(n) * 2	: f(n) = e(n) + h(n) * 2\n");
	printf("3. Emphasize e(n) * 2	: f(n) = e(n) * 2 + h(n)\n");
	printf("4. Breadth First Search	: f(n) = e(n)\n");
	printf("5. Best First Search	: f(n) = h(n)\n");
	printf("6. Emphasize h(n)^2	: f(n) = e(n) + h(n)^2\n");
	printf("> ");
	scanf("%d", &sa);
	printf("\n");
}

// �޸���ƽ �����ϱ� ���� �Լ�
// ���õ� �޸���ƽ�� ��ȣ�� �������� sh�� ����ȴ�
void selectHeuristic() {
	printf("Heuristic\n");
	printf("1. Miss Match\n");
	printf("2. Euclidean Distance\n");
	printf("3. Manhattan Distance\n");
	printf("4. Manhattan Distance + Conflict\n");
	printf("5. Smoothing Manhattan Distance\n");
	printf("6. Smoothing Manhattan Distance + Conflict\n");

	printf("> ");
	scanf("%d", &sh);
	printf("\n");
}

///// ���Ϸ� ���� ���� �迭���� �Է� ���� /////
void Input(int ary[4][4]){

	FILE *in;
	int temp; 

	switch (sb) {
	case 1:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input1.txt", "r");
		break;
	case 2:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input2.txt", "r");
		break;
	case 3:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input3.txt", "r");
		break;
	case 4:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input4.txt", "r");
		break;
	case 5:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input5.txt", "r");
		break;
	case 6:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input6.txt", "r");
		break;
	case 7:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input7.txt", "r");
		break;
	case 8:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input8.txt", "r");
		break;
	default:
		in = fopen("C:\\Users\\410-28\\Desktop\\slide_puzzle\\heuristic-slide-puzzle-main\\resource\\input1.txt", "r");
	}


	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			fscanf(in, "%d", &temp);
			IP[i][j] = temp;
		}
	}

	fclose(in);
}

float CalcFnValue(Node *node){
	int i, j,  temp_dist=0, dist_sum=0, num, dist, temp[10];
	float fnvalue, count=0;

	int SMOOTING_CONST = 4;
	int CONCLICT_CONST = 2;

	// �� ��ġ�� ���� ���� ������ ���� ��� ���� ���� �巡 ���(h(n))���� ���
	for(i=0;i<4;i++){  
		for(j=0;j<4;j++){
			switch (sh) {
			case 1 :
				// Miss Match
				if (node->element[i][j] != SS[i][j]) {
					count++;
				}
				break;
			case 2 :
				// Euclidean Distance
				num = node->element[i][j];
				dist = ceil(sqrt(pow(cord[num][0] - i, 2) + pow(cord[num][1] - j, 2)));
				count += dist;
				break;
			case 3 :
				// Manhattan Distance
				num = node->element[i][j];
				dist = abs(cord[num][0] - i) + abs(cord[num][1] - j);
				count += dist;
				break;
			case 4:
				// Conflict
				num = node->element[i][j];
				dist = abs(cord[num][0] - i) + abs(cord[num][1] - j);
				temp[0] = CONCLICT_CONST * (i < 3 && cord[num][0] == i && node->element[i][j] == node->element[i + 1][j] + 4);
				temp[1] = CONCLICT_CONST * (j < 3 && cord[num][1] == j && node->element[i][j] == node->element[i][j + 1] + 1);
				count += dist + temp[0] + temp[1];
				break;
			case 5:
				num = node->element[i][j];
				dist = (sqrt(abs(cord[num][0] - i) + abs(cord[num][1] - j))) * SMOOTING_CONST;
				count += dist;
				break;
			case 6:
				num = node->element[i][j];
				dist = (sqrt(abs(cord[num][0] - i) + abs(cord[num][1] - j))) * SMOOTING_CONST;
				temp[0] = CONCLICT_CONST * (i < 3 && cord[num][0] == i && node->element[i][j] == node->element[i + 1][j] + 4);
				temp[1] = CONCLICT_CONST * (j < 3 && cord[num][1] == j && node->element[i][j] == node->element[i][j + 1] + 1);
				count += dist + temp[0] + temp[1];
			default : 
				if (node->element[i][j] != SS[i][j]) {
					count++;
				}
			}
		}
	}

	switch (sa) {
		case 1:
			// A* Algorithm : f(n) = e(n) + h(n)
			fnvalue = (float)(node->depth + count);
			break;
		case 2:
			// Emphasize h(n) * 2 : f(n) = e(n) + h(n) * 2
			fnvalue = (float)(node->depth + 2 * count);
			break;
		case 3:
			// Emphasize e(n) * 2 : f(n) = e(n) * 2 + h(n)
			fnvalue = (float)(2 * node->depth + count);
			break;
		case 4:
			// Breadth First Search : f(n) = e(n)
			fnvalue = (float)(node->depth);
			break;
		case 5:
			// Best First Search : f(n) = h(n)
			fnvalue = (float)(count);
			break;
		case 6:
			// Emphasize h(n)^2 : f(n) = e(n) + h(n)^2
			fnvalue = (float)(node->depth + count * count);
		default :
			fnvalue = (float)(node->depth + count);
	}

	return fnvalue;
}


///// �ʱ� ��Ʈ ��带 ������ �迭 ���·� �Է��ϰ� �� �����͸� NULL�� ����� �ʱ�ȭ �Լ� /////
void Init(Node *node, int array[4][4]){
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			node->element[i][j] = array[i][j];
		}
	}

	node->depth = 0;
	node->fnvalue = CalcFnValue(node);	///�ʱ� ���Լ����� ����
	node->unfold = 0;

	node->parent = NULL;

	for(int i=0;i<4;i++){
		node->child[i] = NULL;
	}
}


///// �ڽ� ��带 ����� ���� �켱 �θ��� �迭���� �״�� �����ϰ� ������ �������� �ʱ�ȭ�ϴ� �Լ� /////
void CopyAndReset(Node *node, Node *temp){

	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			temp->element[i][j] = node->element[i][j];
		}
	}

	temp->fnvalue = -1.0;	///�ʱ� ���Լ����� 0���� ���� ����
	temp->depth = 0;
	temp->unfold = 0;

	temp->parent = NULL;

	for(int i=0;i<4;i++){
		temp->child[i] = NULL;
	}


}


///// ���� ����� �迭 ���°� ary[4][4]�� �־����� �迭 ���¿� �������� Ȯ���ؼ� ������ 1 ����, �ƴϸ� 0 ���� /////
int Same(Node *node1, int ary[4][4]){

	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(node1->element[i][j] != ary[i][j]){
				return 0;
			}
		}
	}

	return 1; // ��ǥ ���(���信 ���������� �˸�) Ȯ�� �Ǵ� �ߺ��� ���ϱ� ���� ���� 
}


///// ���� �Ѱ����� �Ķ���� ����� �迭 ���¸� ȭ�鿡 ����ϴ� �Լ� /////
void PrintBoard(Node *node){

	printf("����������������������������������\n");

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (node->element[y][x] == 0) {
				printf("��   ");
			}
			else {
				printf("��%3d", node->element[y][x]);
			}
		}
		printf("��\n");

		if(y != 3) printf("����������������������������������\n");
	}

	printf("����������������������������������\n");
}


///// �̹� ������ ��� ���� ���� �ߺ��� �ִ��� �˻�. �ߺ��Ǵ� ���� ������ 1 ����, ������ 0 ���� /////
int DuplicateAll(Node *node){

	int dup=0;
	Leaf *trace;

	trace = Support;
	
	while(trace){

		if(Same(node, trace->leaf->element)){
			dup = 1;
			break;
		}

		trace = trace->next;
	}

	return dup;
}

	
///// �ڽ� ����� �迭���� ���� ��ĭ�� ������ ������ �ϳ��� ��ȯ�ϱ� ���� �Լ� /////
void Swap(int *a, int *b){

	int temp;

	temp = *a;
	*a = *b;
	*b = temp;
}


///// 0���� ǥ���Ǵ� ��ĭ�� �ϳ� �������� ���� ����(�ڽ�) ��带 ������ ������ŭ ���� �ڽ����� �����ϴ� �Լ� /////
///// �̹� ������ ��� ����� �Ź� ���ϸ鼭 �ߺ��� ���� ��쿡�� ������. /////
void MakeChildren(Node *node){

	int izero, jzero, i;
	Node *temp;


	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(node->element[i][j] == 0){
				izero = i;
				jzero = j;
				break;
			}
		}

	}

	if((izero == 1 || izero == 2) && (jzero == 1 || jzero == 2)){
	
		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);	// �θ� �迭�� �״�� ���� 

		Swap(&temp->element[izero][jzero],&temp->element[izero-1][jzero]);  //���� �ϳ��� �����̴� �� ó�� �� ���� ��ȯ 

		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);		// �ߺ��� ����̸� �̹� �Ҵ��� �޸𸮸� ������ 
		}
		else{
			node->child[i] = temp;
			ChildNum++;		// ��尡 ������ �� ���� ��� Ȯ����� ���� 
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero-1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}


		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero+1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}


		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero+1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}


	if((izero ==1 || izero == 2) && (jzero == 0)){

		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero-1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero+1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}


		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero+1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}

	if((izero ==1 || izero == 2) && (jzero == 3)){

		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero-1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero+1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}


		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero-1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}


	if((jzero ==1 || jzero == 2) && (izero == 0)){

		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero-1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero+1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}


		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero+1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}


	if((jzero ==1 || jzero == 2) && (izero == 3)){

		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero-1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero+1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}


		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero-1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}

	if((izero ==0) && (jzero == 0)){

		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero+1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero+1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}


	if((izero ==0) && (jzero == 3)){

		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero+1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero-1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}


	if((izero ==3) && (jzero == 0)){

		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero-1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero+1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}


	if((izero ==3) && (jzero == 3)){

		i=0;

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero-1][jzero]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}

		temp = (Node *)malloc(sizeof(Node));

		CopyAndReset(node, temp);

		Swap(&temp->element[izero][jzero],&temp->element[izero][jzero-1]);
		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);
		}
		else{
			node->child[i] = temp;
			ChildNum++;
			i++;
		}
	}

	for(i=0;i<4;i++){

		if(node->child[i]){

			node->child[i]->depth = node->depth + 1;		// ������ �ڽ� ������ ���̰��� �ϳ� ���� 

			node->child[i]->fnvalue = CalcFnValue(node->child[i]);		//�� �Լ����� ���� ����Ͽ� ���� 
		}
	}

	node->unfold = 1;

}


///// ������ �ڽ� ������ Support ����Ʈ�� ��� �Ŵٴ� �Լ� /////
void StoreLeaf(Node *current){

	int i;
	Leaf *templeaf;

	for(i = 0;i<4;i++){

		if(current->child[i]){

			templeaf = (Leaf *)malloc(sizeof(Leaf));		// ���� ������ ������ ����Ű�� ���� ����Ʈ �޸� ���� 

			Last->next = templeaf;
			templeaf->leaf = current->child[i];
			templeaf->next = NULL;
			Last = Last->next;

		}
	}
}
	

///// best first Ž���� ���� �� ������ ��� �ĺ� ������ �򰡰��� ���Ͽ� ���� ����(����) ���� ������ ��带 ���� /////
///// ���õ� ��忡 ���� ������ �ּҰ� ���� /////
Node *ChooseBestNode(void){

	Leaf *trace=NULL, *best = NULL;
	Node *tempbest = NULL;
	float tempmin=9999999.0;

	trace = Support->next;

	while(trace){

		if((trace->leaf->fnvalue < tempmin) && trace->leaf->unfold == 0){
			best = trace;
			tempmin = trace->leaf->fnvalue;		
			// �������� ���� ���(unfold == 0�� ���)�� �߿��� fnvalue�� ���� ���� ��带 ã��.
		}
		trace = trace->next;
	}

	tempbest = best->leaf;

	return tempbest;		//��� �ּ� ���� 
}


int main(){

	int ch, done;
	Node *solution = NULL, *trace = NULL;


	Root = (Node *)malloc(sizeof(Node));	//�ʱ� ��Ʈ ��忡 �޸� �Ҵ�
	Support = (Leaf *)malloc(sizeof(Leaf));	//�ʱ� �ܼ� ���� ����Ʈ�� �޸� �Ҵ� 

	selectBoard();			// ���� ����
	Input(IP);				// ���� ���� �Է� 
	selectAlgorithm();		// �˰��� ����
	selectHeuristic();		// �޸���ƽ ����� ������ ����
	Init(Root, IP);			// ���� ���·� ��Ʈ ��� ���� 

	if(Same(Root, SS)){		//�ʱ� ������ ��� ���� ��쿡�� �ٷ� �ذ�� ����̹Ƿ� ���α׷� ���� 
		printf("Done!\n");
		return 0;
	}

	Current = Root;		//Ȯ�� ����� ��带 ����Ű�� Current �����͸� ó������ ��Ʈ�� ����Ű�� ��

	Support->leaf = Root;
	Support->next = NULL;
	Last = Support;

	done = 0;
	
	PrintBoard(Current);		//�ʱ� ���� �迭 ��� 
	printf("f(n) = %f  d = %d\n\n", Current->fnvalue, Current->depth);	//�� ��忡 ���� �򰡰�, �����̰� ���  


	do{

//		PrintBoard(Current);		//�� �ܰ躰 ���� �迭 ��� 
//		printf("f(n) = %f  d = %d\n\n", Current->fnvalue, Current->depth);	//�� ��忡 ���� �򰡰�, �����̰� ���  

		MakeChildren(Current);		//�ڽ� ��� ���� 
		
		for(ch=0;ch<4;ch++){ // ������ �� �ִ� �ִ� �ڽļ��� 4���̹Ƿ� 4�� �ݺ�

			if(Current->child[ch]){ // ���� ������ �ڽĿ� ���ؼ���
				if(Same(Current->child[ch], SS)){		// ������ �ڽĵ� �߿� �ش��� �ִ��� ã��, ������ ǥ���ϰ� ���� ���� 
					solution = Current->child[ch]; 
					PrintBoard(solution);
					printf("f(n) = %f  d = %d\n", solution->fnvalue, solution->depth); 
					// ������ �߰����� ���� ���Լ����� ���̰� ���
					printf("# of children : %d\n", ChildNum); // ������ ã�� ���� ������ ��� �ڽ� ������ ���� ���
					printf("Done!!!\n");
					getchar();
					done = 1; // ���� ã������ �ǹ�
					break;
				}
			}
		}

		if(done == 1){		// �ش��� ã���� ��츦 �ǹ��ϰ� �� �� do �ݺ����� �����ϵ��� �Ѵ�.
			break;
		}


		StoreLeaf(Current);		// ���� ���� ��� ������ Current�� �ڽĵ��� �ܼ� ���� ����Ʈ�� ����Ű�� ��.

		Current = ChooseBestNode();		// �ٽ� ���ο� ����Ʈ �ĺ� ��带 ���� 

//		getchar();

	}while(1);


	trace = solution;	// ������ �ذ�� ��� solution ��尡 �߰ߵ� ���� ��带 ����Ŵ. �̸� �̿��Ͽ� ������� 
						// �������� �������� ��� ���� ���¸� ����ϱ� ����  

	while(trace){		// �������� ���带 ����ϴ� ��ƾ 
		PrintBoard(trace);
		printf("f(n) = %f  d = %d\n\n", trace->fnvalue, trace->depth);
		trace = trace->parent;

		getchar();
	}

	printf("d = %d\n", solution->depth); // ��θ� ��� ����� �� ���������� �ٽ� �� �� �� ã�� ���� ����� ���̰� ���
	printf("# of children : %d\n", ChildNum); // �ٽ� �� �� �� ������ ��� �ڽ� ������ �� ���

	return 0;
}


