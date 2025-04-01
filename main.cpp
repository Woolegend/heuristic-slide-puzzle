#pragma warning(disable: 4996)

#include <iostream>
#include <stdlib.h>
#include "nodetypes.h"
#include "prototypes.h"
#include <math.h>

using namespace std;

Node *Root;		//초기 루트 노드 선언(전역 변수)
Node *Current;	//임시 포인터 변수
Leaf *Support;	//모든 노드들을 저장하기 위한 대표 포인터 
Leaf *Last;		//Support 리스트의 끝 노드를 가리킴(계속 이어서 연결하기 위함)

int	SS[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,0}};	//목표 상태
int IP[4][4];		//  초기 문제 저장 

int ChildNum=0;		// 확장된 노드수를 카운트하기 위한 전역 변수 
int sa, sh, sb;		// 각각 선택된 알고리즘, 휴리스틱, 보드 번호

// @Woolegend
// 각 숫자의 목표 좌표를 저장하는 배열
// 0과 16을 동일하게 취급해 {3,3}이 두 개 저장됨
int cord[17][2] = {{3,3},{0,0},{0,1},{0,2},{0,3},{1,0},{1,1},{1,2},{1,3},{2,0},{2,1},{2,2},{2,3},{3,0},{3,1},{3,2},{3,3}};


// 보드를 선택하기 위한 함수
// 선택된 보드의 번호는 전역변수 sb에 저장된다
void selectBoard() {
	printf("Board level : 1 ~ 8\n");
	printf("> ");
	scanf("%d", &sb);
	printf("\n");
}

// 알고리즘을 선택하기 위한 함수
// 선택된 알고리즘의 번호는 전역변수 sa에 저장된다
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

// 휴리스틱 선택하기 위한 함수
// 선택된 휴리스틱의 번호는 전역변수 sh에 저장된다
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

///// 파일로 부터 문제 배열값을 입력 받음 /////
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

	// 제 위치에 있지 않은 조각의 수를 모두 더한 값을 장래 비용(h(n))으로 사용
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


///// 초기 루트 노드를 문제의 배열 형태로 입력하고 각 포인터를 NULL로 만드는 초기화 함수 /////
void Init(Node *node, int array[4][4]){
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			node->element[i][j] = array[i][j];
		}
	}

	node->depth = 0;
	node->fnvalue = CalcFnValue(node);	///초기 평가함수값을 설정
	node->unfold = 0;

	node->parent = NULL;

	for(int i=0;i<4;i++){
		node->child[i] = NULL;
	}
}


///// 자식 노드를 만들기 위해 우선 부모의 배열값을 그대로 복사하고 나머지 변수들을 초기화하는 함수 /////
void CopyAndReset(Node *node, Node *temp){

	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			temp->element[i][j] = node->element[i][j];
		}
	}

	temp->fnvalue = -1.0;	///초기 평가함수값을 0으로 임의 설정
	temp->depth = 0;
	temp->unfold = 0;

	temp->parent = NULL;

	for(int i=0;i<4;i++){
		temp->child[i] = NULL;
	}


}


///// 현재 노드의 배열 상태가 ary[4][4]로 주어지는 배열 형태와 같은지를 확인해서 같으면 1 리턴, 아니면 0 리턴 /////
int Same(Node *node1, int ary[4][4]){

	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(node1->element[i][j] != ary[i][j]){
				return 0;
			}
		}
	}

	return 1; // 목표 노드(정답에 도달했음을 알림) 확인 또는 중복을 피하기 위해 기존 
}


///// 현재 넘겨지는 파라미터 노드의 배열 형태를 화면에 출력하는 함수 /////
void PrintBoard(Node *node){

	printf("┌───┬───┬───┬───┐\n");

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (node->element[y][x] == 0) {
				printf("│   ");
			}
			else {
				printf("│%3d", node->element[y][x]);
			}
		}
		printf("│\n");

		if(y != 3) printf("├───┼───┼───┼───┤\n");
	}

	printf("└───┴───┴───┴───┘\n");
}


///// 이미 생성된 모든 기존 노드와 중복이 있는지 검사. 중복되는 것이 있으면 1 리턴, 없으면 0 리턴 /////
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

	
///// 자식 노드의 배열값에 대해 빈칸과 임의의 숫자판 하나를 교환하기 위한 함수 /////
void Swap(int *a, int *b){

	int temp;

	temp = *a;
	*a = *b;
	*b = temp;
}


///// 0으로 표현되는 빈칸을 하나 움직여서 다음 상태(자식) 노드를 가능한 개수만큼 만들어서 자식으로 연결하는 함수 /////
///// 이미 생성된 모든 노드들과 매번 비교하면서 중복이 없을 경우에만 생성함. /////
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

		CopyAndReset(node, temp);	// 부모 배열을 그대로 복사 

		Swap(&temp->element[izero][jzero],&temp->element[izero-1][jzero]);  //조각 하나를 움직이는 것 처럼 두 값을 교환 

		temp->parent = node;

		if(DuplicateAll(temp)){
			free(temp);		// 중복된 경우이면 이미 할당한 메모리를 해제함 
		}
		else{
			node->child[i] = temp;
			ChildNum++;		// 노드가 생성될 때 마다 노드 확장수를 증가 
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

			node->child[i]->depth = node->depth + 1;		// 생성된 자식 노드들의 깊이값을 하나 증가 

			node->child[i]->fnvalue = CalcFnValue(node->child[i]);		//평가 함수값을 새로 계산하여 저장 
		}
	}

	node->unfold = 1;

}


///// 생성된 자식 노드들을 Support 리스트에 모두 매다는 함수 /////
void StoreLeaf(Node *current){

	int i;
	Leaf *templeaf;

	for(i = 0;i<4;i++){

		if(current->child[i]){

			templeaf = (Leaf *)malloc(sizeof(Leaf));		// 새로 생성된 노드들을 가리키기 위해 리스트 메모리 생성 

			Last->next = templeaf;
			templeaf->leaf = current->child[i];
			templeaf->next = NULL;
			Last = Last->next;

		}
	}
}
	

///// best first 탐색에 의해 비교 가능한 모든 후보 노드들의 평가값을 비교하여 가장 작은(좋은) 값을 가지는 노드를 선택 /////
///// 선택된 노드에 대한 포인터 주소가 리턴 /////
Node *ChooseBestNode(void){

	Leaf *trace=NULL, *best = NULL;
	Node *tempbest = NULL;
	float tempmin=9999999.0;

	trace = Support->next;

	while(trace){

		if((trace->leaf->fnvalue < tempmin) && trace->leaf->unfold == 0){
			best = trace;
			tempmin = trace->leaf->fnvalue;		
			// 펼쳐지지 않은 노드(unfold == 0인 노드)들 중에서 fnvalue가 가장 작은 노드를 찾음.
		}
		trace = trace->next;
	}

	tempbest = best->leaf;

	return tempbest;		//노드 주소 리턴 
}


int main(){

	int ch, done;
	Node *solution = NULL, *trace = NULL;


	Root = (Node *)malloc(sizeof(Node));	//초기 루트 노드에 메모리 할당
	Support = (Leaf *)malloc(sizeof(Leaf));	//초기 단순 연결 리스트에 메모리 할당 

	selectBoard();			// 보드 선택
	Input(IP);				// 문제 상태 입력 
	selectAlgorithm();		// 알고리즘 선택
	selectHeuristic();		// 휴리스틱 기대비용 측정법 선택
	Init(Root, IP);			// 문제 상태로 루트 노드 설정 

	if(Same(Root, SS)){		//초기 문제가 답과 같은 경우에는 바로 해결된 경우이므로 프로그램 종료 
		printf("Done!\n");
		return 0;
	}

	Current = Root;		//확장 대상의 노드를 가리키는 Current 포인터를 처음에는 루트를 가리키게 함

	Support->leaf = Root;
	Support->next = NULL;
	Last = Support;

	done = 0;
	
	PrintBoard(Current);		//초기 문제 배열 출력 
	printf("f(n) = %f  d = %d\n\n", Current->fnvalue, Current->depth);	//그 노드에 대한 평가값, 노드깊이값 출력  


	do{

//		PrintBoard(Current);		//각 단계별 보드 배열 출력 
//		printf("f(n) = %f  d = %d\n\n", Current->fnvalue, Current->depth);	//그 노드에 대한 평가값, 노드깊이값 출력  

		MakeChildren(Current);		//자식 노드 생성 
		
		for(ch=0;ch<4;ch++){ // 생성될 수 있는 최대 자식수는 4개이므로 4번 반복

			if(Current->child[ch]){ // 실제 생성된 자식에 대해서만
				if(Same(Current->child[ch], SS)){		// 생성된 자식들 중에 해답이 있는지 찾고, 있으면 표시하고 루프 종료 
					solution = Current->child[ch]; 
					PrintBoard(solution);
					printf("f(n) = %f  d = %d\n", solution->fnvalue, solution->depth); 
					// 정답을 발견했을 때의 평가함수값과 깊이값 출력
					printf("# of children : %d\n", ChildNum); // 정답을 찾기 위해 생성된 모든 자식 노드수의 갯수 출력
					printf("Done!!!\n");
					getchar();
					done = 1; // 답을 찾았음을 의미
					break;
				}
			}
		}

		if(done == 1){		// 해답이 찾아진 경우를 의미하고 이 때 do 반복문을 종료하도록 한다.
			break;
		}


		StoreLeaf(Current);		// 답이 없는 경우 생성된 Current의 자식들을 단순 연결 리스트가 가리키게 함.

		Current = ChooseBestNode();		// 다시 새로운 베스트 후보 노드를 선정 

//		getchar();

	}while(1);


	trace = solution;	// 문제가 해결된 경우 solution 노드가 발견된 정답 노드를 가리킴. 이를 이용하여 정답부터 
						// 문제까지 역순으로 모든 보드 형태를 출력하기 위함  

	while(trace){		// 역순으로 보드를 출력하는 루틴 
		PrintBoard(trace);
		printf("f(n) = %f  d = %d\n\n", trace->fnvalue, trace->depth);
		trace = trace->parent;

		getchar();
	}

	printf("d = %d\n", solution->depth); // 경로를 모두 출력한 후 마지막으로 다시 한 번 더 찾은 정답 노드의 깊이값 출력
	printf("# of children : %d\n", ChildNum); // 다시 한 번 더 생성된 모든 자식 노드들의 수 출력

	return 0;
}


