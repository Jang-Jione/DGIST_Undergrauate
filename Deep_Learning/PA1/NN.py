import numpy as np

# input은 1dim array로 들어온다!

class Linear:
    def __init__(self, input_size, output_size, learning_rate):
        # self.weight = np.random.randn(input_size, output_size)
        # Xavier 초기화
        self.weight = np.random.randn(input_size, output_size) * np.sqrt(2. / (input_size + output_size))
        self.bias = np.zeros(output_size)
        self.learning_rate = learning_rate

    def forward(self, input):
        return np.dot(input, self.weight) + self.bias

    def backward(self, input, output_gradient):
        input_gradient = np.dot(output_gradient, self.weight.T)
        weight_gradient = np.dot(input.T, output_gradient)
        
        # Update weights and bias
        self.weight -= self.learning_rate * weight_gradient
        self.bias -= self.learning_rate * np.sum(output_gradient, axis=0) # 얘는 열 별로 하나씩 있을 테니깐 axis = 0 붙여 주었다.

        return input_gradient

class ReLU:
    def forward(self, input):
        return np.maximum(0, input)

    def backward(self, input, output_gradient):
        grad = input > 0
        return grad * output_gradient

class ThreeLayerNN:
    def __init__(self):
        self.inputs = [None] * 5  # Initialize a list to hold inputs for each layer
        self.layer1 = Linear(28*28, 128, 0.01)  # 첫 번째 레이어 인스턴스
        self.layer2 = Linear(128, 64, 0.001)     # 두 번째 레이어 인스턴스
        self.layer3 = Linear(64, 10, 1e-5)      # 세 번째 레이어 인스턴스
        self.relu = ReLU()                       # ReLU 인스턴스
        # learning rate가 크면 NaN이 발생한다. => 0.001도 큰 값이라 한다.

    def forward(self, input):
        self.inputs[0] = input
        self.inputs[1] = self.layer1.forward(self.inputs[0])
        self.inputs[2] = self.relu.forward(self.inputs[1])
        self.inputs[3] = self.layer2.forward(self.inputs[2])
        self.inputs[4] = self.relu.forward(self.inputs[3])
        output = self.layer3.forward(self.inputs[4])
        # # 디버깅: 어디 레이어에서 죽었는지 확인하기 위해서!
        # print(output)
        # import sys
        # sys.exit()
        # print("loading...")
        return output

    def backward(self, output_gradient):
        inputs = self.inputs
        output_gradient = self.layer3.backward(inputs[4], output_gradient)
        output_gradient = self.relu.backward(inputs[3], output_gradient)
        output_gradient = self.layer2.backward(inputs[2], output_gradient)
        output_gradient = self.relu.backward(inputs[1], output_gradient)
        output_gradient = self.layer1.backward(inputs[0], output_gradient)

# threshold function 구현
# softmax 함수는 sigmoid를 일반화한 형태!
def softmax(input):
    # 분자: exponential 확률 값 array
    # 분모: exponential 확률 값 sum
    # return: sum한 array
    # max 값으로 빼주는 것은 overflow 막기 위함이다. 이를 빼줘도 결과는 동일하게 나온다고 한다.
    exp_input = np.exp(input - np.max(input, axis=1, keepdims=True))
    # # 디버깅: 어디 레이어에서 죽었는지 확인하기 위해서!
    # print (exp_input/np.sum(exp_input))
    # import sys
    # sys.exit()
    return exp_input / np.sum(exp_input, axis=1, keepdims=True)
    # sum 뒷 단에 달아준거: 차원 유지 + 행의 합 계산
    # 특히 axis=1는 각 샘플에 대해 계산하도록 돕는다고 한다 => 이거 안해주면 overflow
    # 이 값들을 one-hot encoding을 통해서 label과 매칭 해주면 된다.

# loss 구현 
def cross_entropy(prediction, ground_truth):
    # 1e-10는 overflow를 막기 위함이다.
    # 여기서는 axis=1를 쓰면 안된다.
    return -np.sum(ground_truth*np.log(prediction+(1e-10)))

# def softmax(x):
#     exp_input = np.exp(x - np.max(x, axis=1, keepdims=True))
#     sum_exp_input = np.sum(exp_input, axis=1, keepdims=True)
#     return exp_input / sum_exp_input if sum_exp_input.all() else np.zeros_like(exp_input)

# def cross_entropy(ground_truth, prediction):
#     # prediction의 값을 클립하여 NaN 방지
#     return -np.sum(ground_truth * np.log(np.clip(prediction, 1e-10, None)))

class Softmax_loss:
    def forward(self, input, ground_truth):
        loss = cross_entropy(softmax(input), ground_truth)
        # # 디버깅: 어디 레이어에서 죽었는지 확인하기 위해서!
        # print (loss)
        # import sys
        # sys.exit()
        return loss
    
    # softmax와 loss를 함께 미분하는 것이 수학적으로 더 간단하다고 한다.
    def backward(self, prediction, ground_truth):
        # # 디버깅: 어디 레이어에서 죽었는지 확인하기 위해서!
        # print (loss)
        # import sys
        # sys.exit()
        return prediction-ground_truth