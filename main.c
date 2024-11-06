#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTICES 100
#define INF 1000000 /* 무한대 (연결이 없는 경우) */

// 간선 노드 구조체
typedef struct EdgeNode {
    int vertex;
    int weight;
    struct EdgeNode* next;
} EdgeNode;

// 그래프 구조체
typedef struct GraphType {
    int n; // 정점의 개수
    EdgeNode* adj_list[MAX_VERTICES];
} GraphType;

int distance[MAX_VERTICES]; /* 시작정점으로부터의 최단경로 거리 */
int found[MAX_VERTICES];    /* 방문한 정점 표시 */

typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    HeapNode heap[MAX_VERTICES];
    int size;
} MinHeap;

// minheap 초기화
void init_min_heap(MinHeap* h) {
    h->size = 0;
}

// minheap 삽입
void insert_min_heap(MinHeap* h, int vertex, int distance) {
    int i = ++(h->size);
    while (i != 1 && distance < h->heap[i / 2].distance) {
        h->heap[i] = h->heap[i / 2];
        i /= 2;
    }
    h->heap[i].vertex = vertex;
    h->heap[i].distance = distance;
}

// minheap 삭제
HeapNode delete_min_heap(MinHeap* h) {
    int parent, child;
    HeapNode item, temp;
    item = h->heap[1];
    temp = h->heap[(h->size)--];
    parent = 1;
    child = 2;
    while (child <= h->size) {
        if (child < h->size && h->heap[child].distance > h->heap[child + 1].distance)
            child++;
        if (temp.distance <= h->heap[child].distance)
            break;
        h->heap[parent] = h->heap[child];
        parent = child;
        child *= 2;
    }
    h->heap[parent] = temp;
    return item;
}

// 현재 distance와 found 배열을 출력
void print_status(GraphType* g) {
    printf("distance: ");
    for (int i = 0; i < g->n; i++) {
        if (distance[i] == INF)
            printf("* ");
        else
            printf("%d ", distance[i]);
    }
    printf("\nfound: ");
    for (int i = 0; i < g->n; i++) {
        printf("%d ", found[i]);
    }
    printf("\n\n");
}

// 다익스트라 알고리즘을 사용해 시작 정점에서 다른 모든 정점으로 최단경로를 찾는 함수
void shortest_path(GraphType* g, int start) {
    MinHeap h;
    init_min_heap(&h);
    int found_order[MAX_VERTICES];  // 방문 순서를 저장할 배열
    int order_index = 0;            // 방문 순서 배열의 인덱스

    for (int i = 0; i < g->n; i++) { /* 초기화 */
        distance[i] = INF;  // 모든 노드를 INF로 초기화
        found[i] = FALSE;
    }
    distance[start] = 0;  // 시작 노드의 거리만 0으로 설정
    insert_min_heap(&h, start, 0);  // 시작 노드를 큐에 추가

    print_status(g);  // 초기 상태 출력

    while (h.size > 0) {
        HeapNode node = delete_min_heap(&h);
        int u = node.vertex;

        if (found[u]) continue;
        found[u] = TRUE;
        found_order[order_index++] = u + 1;  // 방문 순서 저장 (1부터 시작하는 인덱스로)

        EdgeNode* edge = g->adj_list[u];
        while (edge != NULL) {
            int v = edge->vertex;
            int weight = edge->weight;
            if (!found[v] && distance[u] + weight < distance[v]) {
                distance[v] = distance[u] + weight;
                insert_min_heap(&h, v, distance[v]);
            }
            edge = edge->next;
        }
        print_status(g);  // 업데이트된 상태 출력
    }

    // 연결이 끊긴 노드 처리: 방문되지 않은 노드를 추가로 방문 표시하고 순서에 추가
    for (int i = 0; i < g->n; i++) {
        if (!found[i]) {
            found[i] = TRUE;
            found_order[order_index++] = i + 1;  // 방문 순서에 추가
        }
    }
    print_status(g);  // 최종 상태 출력

    // 방문 순서 출력
    printf("Found Order: ");
    for (int i = 0; i < order_index; i++) {
        printf("%d ", found_order[i]);
    }
    printf("\n");
}

// 그래프 생성과 각 간선 가중치 설정
void GenerateGraph(GraphType* g) {
    g->n = 10; // 노드 수
    int weights[10][10] = {
        {0, 3, INF, INF, INF, 11, 12, INF, INF, INF},  // 1번 노드
        {3, 0, 5, 4, 1, 7, 8, INF, INF, INF},          // 2번 노드
        {INF, 5, 0, 2, INF, INF, 6, 5, INF, INF},      // 3번 노드
        {INF, 4, 2, 0, 13, INF, INF, INF, INF, 16},    // 4번 노드
        {INF, 1, INF, 13, 0, 9, INF, INF, 18, 17},     // 5번 노드
        {11, 7, INF, INF, 9, 0, INF, INF, INF, INF},   // 6번 노드
        {12, 8, 6, INF, INF, INF, 0, 13, INF, INF},    // 7번 노드
        {INF, INF, 5, INF, INF, INF, 13, 0, INF, 15},  // 8번 노드
        {INF, INF, INF, INF, 18, INF, INF, INF, 0, 10},// 9번 노드
        {INF, INF, INF, 16, 17, INF, INF, 15, 10, 0}   // 10번 노드
    };

    for (int i = 0; i < g->n; i++) {
        g->adj_list[i] = NULL;
        for (int j = 0; j < g->n; j++) {
            if (weights[i][j] != INF && weights[i][j] != 0) {
                EdgeNode* newNode = (EdgeNode*)malloc(sizeof(EdgeNode));
                newNode->vertex = j;
                newNode->weight = weights[i][j];
                newNode->next = g->adj_list[i];
                g->adj_list[i] = newNode;
            }
        }
    }
}

int main(void) {
    GraphType* g = (GraphType*)malloc(sizeof(GraphType));
    if (g == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }

    GenerateGraph(g);
    shortest_path(g, 0);
    return 0;
}
