#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_NODES 15000
#define MAX_LINE 100

typedef struct {
    int size;
    int** matrix; 
} Graph;

Graph* create_graph(int nodes) {
    Graph* g = malloc(sizeof(Graph));
    g->size = nodes;
    g->matrix = malloc(nodes * sizeof(int*));

    for (int i = 0; i < nodes; i++) {
        g->matrix[i] = malloc(nodes * sizeof(int));
        for (int j = 0; j < nodes; j++) {
            g->matrix[i][j] = -1; 
        }
    }
    return g;
}

void add_edge(Graph* g, int u, int v, int weight) {
    g->matrix[u][v] = weight;
}

int find_closest(int dist[], int visited[], int nodes) {
    int min_dist = INT_MAX, closest = -1;

    for (int v = 0; v < nodes; v++) {
        if (!visited[v] && dist[v] < min_dist) {
            min_dist = dist[v];
            closest = v;
        }
    }
    return closest;
}

void show_path(int parent[], int target, int* id_to_vertex) {
    int path[MAX_NODES], steps = 0;

    for (int at = target; at != -1; at = parent[at]) {
        path[steps++] = id_to_vertex[at];
    }

    printf("Shortest path: ");
    for (int i = steps-1; i >= 0; i--) {
        printf("%d%s", path[i], i ? ", " : "");
    }
}

void find_shortest_path(Graph* g, int start, int end, int* id_to_vertex) {
    int nodes = g->size;
    int dist[nodes], visited[nodes], parent[nodes];

    for (int i = 0; i < nodes; i++) {
        dist[i] = INT_MAX;
        visited[i] = 0;
        parent[i] = -1;
    }
    dist[start] = 0;

    for (int count = 0; count < nodes-1; count++) {
        int u = find_closest(dist, visited, nodes);
        if (u == -1) break;

        visited[u] = 1;

        for (int v = 0; v < nodes; v++) {
            if (!visited[v] && g->matrix[u][v] != -1 && 
                dist[u] + g->matrix[u][v] < dist[v]) {
                dist[v] = dist[u] + g->matrix[u][v];
                parent[v] = u;
            }
        }
    }

    if (dist[end] == INT_MAX) {
        printf("No path from %d to %d\n", id_to_vertex[start], id_to_vertex[end]);
    } else {
        show_path(parent, end, id_to_vertex);
        printf(" (Distance: %d km)\n", dist[end]);
    }
}

void free_graph(Graph* g) {
    for (int i = 0; i < g->size; i++) free(g->matrix[i]);
    free(g->matrix);
    free(g);
}

int main() {
    char* filename = "airline_distances.txt";
    int source, target;

    printf("Enter start and end airports (e.g., 123 456): ");
    scanf("%d %d", &source, &target);

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Can't open %s\n", filename);
        return 1;
    }

    int max_id = -1;
    int vertex_count = 0;
    int vertex_ids[MAX_NODES] = {0};
    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) == 3) {
            if (!vertex_ids[u]) vertex_count++;
            if (!vertex_ids[v]) vertex_count++;
            vertex_ids[u] = vertex_ids[v] = 1;
            if (u > max_id) max_id = u;
            if (v > max_id) max_id = v;
        }
    }

    int id_to_index[max_id+1];
    int index_to_id[vertex_count];
    int index = 0;

    for (int i = 0; i <= max_id; i++) {
        if (vertex_ids[i]) {
            id_to_index[i] = index;
            index_to_id[index++] = i;
        }
    }

    if (!vertex_ids[source] || !vertex_ids[target]) {
        printf("Error: Airport not found\n");
        fclose(file);
        return 1;
    }
    
    rewind(file);
    Graph* g = create_graph(vertex_count);

    while (fgets(line, MAX_LINE, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) == 3) {
            add_edge(g, id_to_index[u], id_to_index[v], w);
        }
    }

    fclose(file);

    find_shortest_path(g, id_to_index[source], id_to_index[target], index_to_id);

    free_graph(g);
    return 0;
}