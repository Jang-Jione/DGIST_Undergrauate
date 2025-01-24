def EdmondsKarp(adjacency_matrix, edge_dict, source, sink):
    def Bfs():
        parent = [-1] * len(adjacency_matrix) # parent 활용해서 메모리 줄이기!
        parent[source] = source 
        queue = [source]
        i = 0  
        
        while i < len(queue):
            u = queue[i]
            i += 1
            for v in adjacency_matrix[u]:
                if parent[v] == -1 and edge_dict[(u,v)] > 0:
                    parent[v] = u
                    if v == sink:
                        return parent
                    queue.append(v)
        return None

    min_cut = 0

    while True:
        parent = Bfs()

        if not parent:  # 더 이상 경로가 없으면 종료
            break
            
        flow = float('inf')
        v = sink
        while v != source:
            u = parent[v]
            if flow <= edge_dict[(u,v)]: # flow 구하기
                flow = flow
            else:
                flow = edge_dict[(u,v)]
            v = parent[v]
        
        v = sink
        while v != source:
            u = parent[v]
            edge_dict[(u,v)] -= flow
            edge_dict[(v,u)] += flow
            v = parent[v]
            
        min_cut += flow
        
    return min_cut

N, M = map(int, input().split())
N = N + 1 # 0,0부터 N,N까지의 좌표 있으므로 +1 해주기!
x1, y1, x2, y2 = map(int, input().split())
obstacles = set()
for _ in range(M):
    a, b = map(int, input().split())
    obstacles.add((a, b))

### 그래프 구성
if abs(x1 - x2) + abs(y1 - y2) == 1:
    print(-1)
else:
    def get_in_node(x, y):
            return x * N + y
            
    def get_out_node(x, y):
        return x * N + y + N * N

    V = 2 * N * N 
    adjacency_matrix = [[] for _ in range(V)]
    edge_dict = {}

    for x in range(N):
        for y in range(N):
            if (x, y) in obstacles:
                continue
            
            if (x, y) != (x1, y1) and (x, y) != (x2, y2):
                in_node = get_in_node(x, y)
                out_node = get_out_node(x, y)
                adjacency_matrix[in_node].append(out_node)
                adjacency_matrix[out_node].append(in_node)
                edge_dict[(in_node, out_node)] = 1
                edge_dict[(out_node, in_node)] = 0 # in node / out node 만들어 두기 -> 여기에만 weight 줌

    for x in range(N):
        for y in range(N):
            if (x, y) in obstacles:
                continue
                
            out_node = get_out_node(x, y)
            
            for nx, ny in [(x-1,y), (x+1,y), (x,y-1), (x,y+1)]:
                if 0 <= nx < N and 0 <= ny < N and (nx, ny) not in obstacles:
                    in_node = get_in_node(nx, ny)
                    adjacency_matrix[out_node].append(in_node)
                    adjacency_matrix[in_node].append(out_node)
                    edge_dict[(out_node, in_node)] = float('inf')
                    edge_dict[(in_node, out_node)] = 0 # 나머지는 경로 탐색을 위해서 그냥 연결만 해두고 -> weight는 0으로 줌

    source = get_out_node(x1, y1)
    sink = get_in_node(x2, y2)
        
    print(EdmondsKarp(adjacency_matrix, edge_dict, source, sink))
###