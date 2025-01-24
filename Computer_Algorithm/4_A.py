# def Greedy(cards, goal_num):
#     possible_set = GetPossiSet(cards)
#     impossible_set = set()
#     for i in range(1,goal_num+1):
#         if i in possible_set:
#             continue
#         impossible_set.add(i)

#     additional_cards = 0
#     while impossible_set:
#         smallest_impossible = min(impossible_set)  
#         cards.append(smallest_impossible)  
#         possible_set = GetPossiSet(cards)  
#         impossible_set = set()  
#         for i in range(1, goal_num + 1):
#             if i not in possible_set:
#                 impossible_set.add(i)
#         additional_cards += 1

#     return additional_cards

# def GetPossiSet(cards): # 가능한 조합을 모두 구해주는 함수
#     possible_set = set()
#     possible_set.add(0)
#     for card in cards:
#         new_created = set(possible_set)  # 기존에 만든 수들 복사
#         for number in possible_set:
#             new_created.add(number + card)
#         possible_set = new_created
#     return possible_set

def Greedy(cards, goal_num):
    cards.sort()

    possible_num = 0
    additional_pick = 0

    for card in cards:
        if (possible_num >= goal_num):
            break
        if (card <= possible_num+1): 
            possible_num += card
            continue
        if (card >= possible_num+2): 
            while (card >= possible_num+2):
                additional_pick += 1
                possible_num += possible_num+1 # 계속 빈칸 채우면서 뽑기 (card <= possible_num+1) 이 조건 만족할 때 까지
                if (possible_num >= goal_num):
                    break # 엣지 case: 1 1 / 3
            possible_num += card
    card = possible_num+1 # 카드를 효율적으로 뽑기
    while (possible_num < goal_num):
        if (possible_num >= goal_num):
            break
        additional_pick += 1
        possible_num += card
        card = possible_num+1 # 카드를 효율적으로 뽑기
    return additional_pick

card, goal_num = map(int,input().split())
cards = list(map(int, input().split()))

# output은 추가로 뽑아야 하는 카드 개수 (ex. 예제에서는 2가 필요하므로 정답은 1!)
print(Greedy(cards, goal_num))
# print(GetPossiSet(cards))

# Greedy: 
# 1. 가장 작은 수를 포함시키는 건 optimal을 배제하지 않는다. -> X
# 2. 안되는 수 중 가장 작은 걸 포함시키는 건 optimal을 배제하지 않는다. -> X
# 우선 가능한 조합을 구하는 것 자체가 load가 너무 크다.. -> 1,2는 이것 때문에 안된다.
# 3. 카드 하나씩 추가하면서 가능한 숫자 1씩 늘리기, 만약 카드 +2이상 늘여나면 while 돌면서 가능한 숫자 추가
# 다음 숫자 추가하는 것이 optimal을 배제하지 않는다.