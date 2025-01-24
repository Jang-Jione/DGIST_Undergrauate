# 핵심 K이상인 edge들 capacity inf로 두고 풀기

def EdmondsKarp(node, ajacency_matrix, source, sink):
    residual = [[0] * node for _ in range(node)]
    max_flow = 0
    
    path = Bfs(node, ajacency_matrix, residual, source, sink)

    if path is None: # 만약 경로 없다면 0 리턴 -> 암것도 설치 안해도 되므로
        return 0

    while path is not None: # residual에서 경로 못 찾을 때 까지 반복
        flow = float("inf")

        for u,v in path:
            flow = min(flow, ajacency_matrix[u][v] - residual[u][v])
        
        max_flow += flow
        
        for u,v in path:
            residual[u][v] += flow  # 정방향 유량 증가
            residual[v][u] -= flow  # 역방향 유량 감소
            
        path = Bfs(node, ajacency_matrix, residual, source, sink)
    
    if max_flow == float('inf'):
        return -1
        
    return max_flow

def Bfs(node, ajacency_matrix, residual, source, sink):
    visited = [False] * node
    path_edges = {i: [] for i in range(node)}  # 각 정점까지의 간선 정보를 저장
    
    queue = [source]
    visited[source] = True
    
    while queue:
        current = queue.pop(0)
        
        if current == sink:
            return path_edges[current]
            
        # 다음 정점 탐색
        for next in range(node):
            residual_capacity = ajacency_matrix[current][next] - residual[current][next]
            if not visited[next] and residual_capacity > 0:
                visited[next] = True
                queue.append(next)
                path_edges[next] = path_edges[current] + [(current, next)]
    
    return None  # sink까지 도달할 수 없는 경우
##########

node, edge, bomb_threshold = map(int, input().split()) # 거점, 다리, 최소 폭탄 머시기
table_info = [tuple(map(int, input().split())) for _ in range(edge)]

ajacency_matrix = [[0] * node for _ in range(node)] # ajacency matrix 만들기
for u, v, w in table_info:
    ajacency_matrix[u][v] = w
    ajacency_matrix[v][u] = w 

for i in range(node):
    for j in range(node):
        if ajacency_matrix[i][j] >= bomb_threshold:
            ajacency_matrix[i][j] = float('inf') # 핵심!!!!: 유량 inf로 두고 하기

print(EdmondsKarp(node, ajacency_matrix, 0, node-1)) # 노드 개수, 그래프, s, t

# def FordFulkerson(node, info_table):
#     # list 말고 adjacency matrix 이용하기
#     original_graph = [[0] * node for _ in range(node)]
#     for start, finish, weight in info_table:
#         original_graph[start][finish] = weight # 정방향은 weight
#         original_graph[finish][start] = 0 # 역방향은 flow: 현재는 0

#     while True:
#         residual_graph = GetResidual(node, original_graph)
#         updated_original_graph = FindPathResidual(node, original_graph, residual_graph)
        
#         if updated_original_graph == original_graph:
#             break # 종료 조건: 변화가 없다면 종료

#         original_graph = updated_original_graph # 오리지널 그래프 업데이트 하기
    
#     max_flow = 0
#     for i in range(node):
#         max_flow += original_graph[i][node-1]

#     print(original_graph)
#     return max_flow

# def GetResidual(node, original_graph):
#     residual_graph = [[0] * node for _ in range(node)]
#     for i in range(node):
#         for j in range(node):
#                 # 정방향은 remaining weight
#                 residual_graph[i][j] = original_graph[i][j] - original_graph[j][i]
#                 # 역방향은 flow
#                 residual_graph[j][i] = original_graph[j][i]
#     return residual_graph

# def FindPathResidual(node, original_graph, residual_graph):
#     init_node = 0
#     path_checker = set([0])
#     path = [0]
#     path_weight = []

#     while True:  # path를 구하는 loop
#         found = False
#         for i in range(node):
#             if i not in path_checker:
#                 path_checker.add(i)
#                 path.append(i)
#                 path_weight.append(residual_graph[init_node][i])
#                 init_node = i
#                 found = True
#                 break

#         if not found:  # 경로를 더 이상 확장할 수 없으면 중단
#             break

#         if init_node == node - 1:  # 싱크 노드에 도달하면 종료
#             break

#     if init_node != node - 1:  # 유효한 경로가 없으면 원본 그래프 반환
#         return original_graph

#     # ex. path: [0, 2, 3] / path_weight: [4, 6]
#     # flow는 min(path_weight)로 결정
#     min_flow = min(path_weight)

#     # 업데이트된 그래프 생성
#     updated_graph = [row[:] for row in original_graph]

#     for i in range(len(path) - 1):
#         u, v = path[i], path[i + 1]
#         # 정방향 업데이트 
#         updated_graph[u][v] -= min_flow
#         # 역방향 업데이트
#         updated_graph[v][u] += min_flow

#     return updated_graph

# node, edge, bomb_threshold = map(int,input().split())
# info_table = [list(map(int, input().split())) for _ in range(edge)]
# # info_table = [row for row in info_table if row[2] < bomb_threshold] # bomb_threshold인 곳은 미리 지워주기 
# # -> 문제 이해 잘못함 걍 한 번에 못 지운다는 이야기인듯?

# print(FordFulkerson(node, info_table))

# def ShortestPath(graph):
#     # 다익스트라로 shortest path 찾는 문제로 풀면 된다!
#     distances = [float('inf')] * node
#     visited = [False] * node
#     distances[0] = 0  # 시작 노드의 거리 0
    
#     for _ in range(node):
#         min_distance = float('inf')
#         current_node = -1
#         for i in range(node):
#             if not visited[i] and distances[i] < min_distance:
#                 min_distance = distances[i]
#                 current_node = i
        
#         if current_node == -1:  
#             break
        
#         # 현재 노드 방문 처리
#         visited[current_node] = True
        
#         # 인접 노드 업데이트
#         for neighbor, weight in graph[current_node]:
#             if not visited[neighbor]:
#                 new_distance = distances[current_node] + weight
#                 if new_distance < distances[neighbor]:
#                     distances[neighbor] = new_distance
    
#     return distances[node - 1] if distances[node - 1] != float('inf') else -1

# node, edge, bomb_threshold = map(int, input().split())
# info_table = [list(map(int, input().split())) for _ in range(edge)]
# info_table = [row for row in info_table if row[2] < bomb_threshold] # bomb_threshold인 곳은 미리 지워주기 

# graph = [[] for _ in range(node)]
# for a, b, weight in info_table:
#     graph[a].append((a, weight))
#     graph[b].append((b, weight))

# print(ShortestPath(graph))
