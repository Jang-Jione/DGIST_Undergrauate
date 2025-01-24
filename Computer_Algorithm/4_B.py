# def Greedy(table, table_info, mem):
#     MST = []
#     result = 0
#     while(len(table_info)>0):
#         min_distance = float('inf')
#         for a,b,distance in (table_info):
#             min_distance = min(min_distance,distance)
#             # min_distance를 edge로 가지는 a,b MST에 추가
#         cycle_checker = 0
#         for a,b,distance in (table_info):
#             if min_distance == distance:
#                 if (MST.count(a) < 2 and MST.count(b) < 2): # 사이클 조건
#                     MST.append(a)
#                     MST.append(b)
#                     result += distance
#                     break
#                 table_info.remove([a,b,distance])
#             if (MST.count(a) >= 1):
#                 cycle_checker += 1
#         if cycle_checker == table-1: # 사이클 조건2
#             break

#     if result > mem:
#         return -1

#     return result

# def Greedy(table, table_info, mem):
#     table_info.sort(key=lambda x: x[2]) # 미리 정렬해 두기 GPT 참고

#     MST = []
#     cycle_checking = set()
#     cycle_checking2 = set()
#     result = 0

#     for a, b, distance in table_info:
#         if len(cycle_checking) == table and len(cycle_checking2) == table-2:
#             # 다 하나씩은 count 되어야 하고, 두 개 빼고(양 끝 값)는 두 번씩 count 되어야 한다.
#             break

#         if (MST.count(a) < 1 and MST.count(b) < 1) or (MST.count(a) < 1 and MST.count(b) < 2) or (MST.count(a) < 2 and MST.count(b) < 1): # 사이클 조건
#             MST.append(a)
#             MST.append(b)
#             cycle_checking.add(a)
#             cycle_checking.add(b)
#             result += distance
#             continue
        
#         if MST.count(a) == 2:
#             cycle_checking2.add(a)

#         if MST.count(b) == 2:
#             cycle_checking2.add(b)

#     if result > mem:
#         return -1
    
#     return result

def Greedy(table, table_info, mem):
    table_info.sort(key=lambda x: x[2])  # 거리순 정렬
    
    MST = set([0])  # 주방부터 시작해서 연결된 노드들
    result = 0  # 총 거리
    
    # 모든 테이블이 연결될 때까지 반복
    while len(MST) < table:
        for a, b, distance in table_info:
            if (a in MST) != (b in MST): # 사이클 조건
                MST.add(a)
                MST.add(b)
                result += distance
                break
                
    if result > mem:  # 메모리 초과 체크
        return -1
        
    return result

# table 개수는 주방을 포함한다.
table, mem = map(int,input().split())
line = table*(table-1)//2
table_info = [list(map(int, input().split())) for _ in range(line)]
# a, b, distance

print(Greedy(table, table_info, mem))

# 0 1 1
# 0 2 5
# 0 3 2
# 1 2 6
# 1 3 4
# 2 3 1
# 0 1 -> 1 0(이 때는 메모리 안씀) -> 0 3 -> 3 2
# 이 문제는 MST 찾는 문제! -> kruskal 사용
# 시간 초과 -> 미리 정렬해 두고 하기