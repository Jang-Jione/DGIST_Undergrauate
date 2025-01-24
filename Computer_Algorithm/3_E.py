def Graph(node):
    visited = set()
    result = 0
    for start in node:
        # 연결된 노드가 없는 경우 무시
        if start not in visited and len(node[start]) > 0:
            result += Dfs(start, node[start][0], node, visited, None) 
    return result

def Dfs(start, next, node, visited, parent):
    buffer_x, buffer_y = start
    current_x, current_y = start
    next_x, next_y = next
    path_length = 0

    while (current_x, current_y) in node:
        # 종료 조건: 이미 방문한 노드
        if (current_x, current_y) in visited:
            return 0

        visited.add((current_x, current_y))  # 현재 노드 방문 처리

        # 현재 노드와 연결된 노드 리스트 확인
        neighbors = node[(current_x, current_y)]
        if len(neighbors) == 0:
            return 0  # 연결된 노드가 없으면 종료

        # 연결된 노드 중 다음 노드 탐색
        next_candidates = [n for n in neighbors if n != parent]  # 부모 노드는 제외
        if not next_candidates:
            return 0  # 다음으로 갈 노드가 없으면 종료

        next_x, next_y = next_candidates[0]  

        parent = (current_x, current_y)

        current_x, current_y = next_x, next_y
        path_length += 1

        if (next_x, next_y) == (buffer_x, buffer_y) and path_length >= 3:
            return 1

    return 0


# 입력 처리
edges = int(input())
edge_info = [tuple(map(int, input().split())) for _ in range(edges)]
node = {}
for start_x, start_y, finish_x, finish_y in edge_info:
    if (start_x, start_y) not in node:
        node[(start_x, start_y)] = []
    node[(start_x, start_y)].append((finish_x, finish_y))
    if (finish_x, finish_y) not in node:
        node[(finish_x, finish_y)] = []
    node[(finish_x, finish_y)].append((start_x, start_y))

for key in list(node.keys()):
    if len(node[key]) >= 3:
        node[key] = []  # 연결된 노드가 3개 이상이면 무시 처리 -> 이러면 당연히 조건 위반!

result = Graph(node)
print(result)
