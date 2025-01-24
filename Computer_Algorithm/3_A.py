def Change(students, pos): # 위치 변경
    if pos == 7:
        students = students[7]+students[:7]
        return students
    else: 
        students = students[1:]+students[0]
        return students

def Wakeup(students): # 깨우기
    for _ in range(7,-1,-1): 
        if students[_] == "0":
            students = students[:_]+"1"+students[_+1:]
            break
    return students

def Reverse(students): # 뒤바꾸기
    buffer = []
    for _ in range(8):
        if students[_] == "1":
            buffer.append("0")
        else:
            buffer.append("1")
    return ''.join(buffer)

def Sleep(students): # 달구 쉴 때 졸기
    return "0"+students[1:7]+"0"

def Bfs():
    students = input() 

    timer = 0  # 시간 재기
    L = [(students, timer, 0)]  # (현재 상태, 시간, 행동 수)
    visited = set() # 방문

    while L:
        buffer = L
        L = []

        for students, timer, action_count in buffer:
            visitor = (students, action_count)  # students 같더라도 action_count 0이냐 3이냐에 따라 다른 결과 나올 수 있으므로
            if visitor in visited:
                continue
            
            visited.add(visitor)
            
            if students == "11111111":
                return timer
                
            # 3번 연속 행동 후에 달구 쉼
            if action_count == 3:
                students = Sleep(students)
                L.append((students, timer+1, 0))
                continue
                
            L.append((Change(students, 0), timer+1, action_count+1))  # 자리 바꾸기 (pos=0)
            L.append((Change(students, 7), timer+1, action_count+1))  # 자리 바꾸기 (pos=7)
            L.append((Wakeup(students), timer+1, action_count+1))     # 깨우기
            L.append((Reverse(students), timer+1, action_count+1))    # 뒤바꾸기

    return -1

print(Bfs())