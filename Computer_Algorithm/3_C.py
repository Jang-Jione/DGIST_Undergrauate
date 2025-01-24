# 이 문제는 일정 시간 뒤에 가중치가 업데이트 되는 문제!
# Bellman 돌고, 해당 node로 가는게 최소 시간보다 크면, node 업데이트 하고, 한 번 더 돌리기
# 아 다익스트라라는 제한이 생겼다......... 다시 풀어야 됨..... ㅜㅜㅜㅜ

def Dijkstra(nodes, edges, time, node, get_bicycle):
   Greedy_table = [float('inf')] * (nodes + 1)
   visited = [False] * (nodes + 1)
   Greedy_table[1] = 0  
   
   for _ in range(nodes):
       min_value = float('inf')
       min_node = -1
       
       # 방문하지 않은 노드 중 최단 거리가 가장 짧은 노드 선택
       for i in range(1, nodes + 1):
           if not visited[i] and Greedy_table[i] < min_value:
               min_value = Greedy_table[i]
               min_node = i
       
       if min_node == -1:  # 더 이상 갈 수 있는 노드가 없음
           break
           
       visited[min_node] = True
       
       for next_node, weight in node[min_node]:
           if Greedy_table[min_node] >= time and min_node in get_bicycle:
               new_dist = Greedy_table[min_node] + weight/2  # 자전거를 탈 수 있는 경우
           else:
               new_dist = Greedy_table[min_node] + weight               
           if new_dist < Greedy_table[next_node]:
               Greedy_table[next_node] = new_dist
   
   if Greedy_table[nodes] == float('inf'):
       return -1
   return round(Greedy_table[nodes])

nodes, edges, bicycle, time = map(int, input().split())
edge_info = [tuple(map(int, input().split())) for _ in range(edges)]
node = {i: [] for i in range(1, nodes + 1)} 
for start_node, finish_node, weight in edge_info:
   # node에 둘 다 넣어주기
   node[start_node].append((finish_node,weight))
   node[finish_node].append((start_node,weight))
get_bicycle = [int(input()) for _ in range(bicycle)]

result = Dijkstra(nodes, edges, time, node, get_bicycle)
print(result)