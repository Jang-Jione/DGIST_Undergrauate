# 전체 프로세스 시간/코어 수에 가깝도록 조합지어주게 짜기 -> 예외가 넘 많음,,
# 조합들의 최대가 최소가 되도록 짜기 -> ㄴ..나중에 봐도 이해할 수 있겠지..
# ex. (3,4) (2,5) // (3,5) (2,4) 라는 두 조합이 있으면 
# 각 조합의 최대는 (3,4) (3,5)고 (3,4)가 더 작으므로 이것 선택하기!!

def MaxatMin(process, cpu):
    def backtrack(current_proc, partitions):
        nonlocal best_partitions, min_max_value
        
        if current_proc == len(process):
            if len(partitions) == cpu:  
                partition_sums = [sum(p) for p in partitions]
                max_sum = max(partition_sums) # 최대 값 구하기
                if max_sum < min_max_value:
                    min_max_value = max_sum # 최대 값 중에서도! 최소 값 구하기 -> 내가 적으면서도 헷갈린다..
                    best_partitions = [p[:] for p in partitions]
            return
        
        if len(partitions) > cpu:
            return
            
        for i in range(len(partitions)): # 조합 제작
            partitions[i].append(process[current_proc])
            backtrack(current_proc + 1, partitions)
            partitions[i].pop()
            
        if len(partitions) < cpu: # 조합 제작
            partitions.append([process[current_proc]])
            backtrack(current_proc + 1, partitions)
            partitions.pop()

    best_partitions = None
    min_max_value = float('inf')
    
    backtrack(0, [])
    
    return min_max_value

processes, cpu, deadline = map(int, input().split())
process = [int(input()) for _ in range(processes)]

result = MaxatMin(process, cpu)

if deadline < result:
    print(-1)
else:
    print(deadline-result)