# recursive하게 DP를 풀면 recursive error가 생긴다. 
# 이를 배열로 만들어서 풀자

def Dp(fish_bun, rescue_days):
    Dp_table = [[0] * (fish_bun + 1) for _ in range(10)]
    
    # 첫날 초기화
    for i in range(10):
        if i <= fish_bun:
            Dp_table[i][fish_bun - i] = 1  # 첫날 붕어빵 i개를 먹었을 때

    for days in range(1, rescue_days):  
        next_Dp_table = [[0] * (fish_bun + 1) for _ in range(10)]  
        
        for remaining_buns in range(fish_bun + 1): 
            for yesterday in range(10):  
                if Dp_table[yesterday][remaining_buns] > 0: 
                    if yesterday == 0: # 0일 때는 또 0이 안되게 1개 먹여주기
                        if remaining_buns >= 1:
                            next_Dp_table[1][remaining_buns - 1] += Dp_table[yesterday][remaining_buns]
                    else:
                        for i in range(max(0, yesterday - 1),min(10, yesterday + 2),1): 
                            if remaining_buns >= i:
                                next_Dp_table[i][remaining_buns - i] += Dp_table[yesterday][remaining_buns]
        
        Dp_table = next_Dp_table  # 현재 dp 배열을 다음 배열로 갱신

    result = 0
    for yesterday in range(10):
        for remaining_buns in range(fish_bun + 1):
            result += Dp_table[yesterday][remaining_buns] 

    return result

rescue_days, fish_bun = map(int, input().split())

print(Dp(fish_bun, rescue_days))
