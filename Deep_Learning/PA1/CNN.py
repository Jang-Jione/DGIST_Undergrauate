import numpy as np

# input은 1dim array로 들어온다!

class Linear:
    def __init__(self, input_size, output_size, learning_rate):
        # self.weight = np.random.randn(input_size, output_size)
        # Xavier 초기화
        self.weight = np.random.randn(input_size, output_size) * np.sqrt(2. / (input_size * output_size))
        self.bias = np.zeros(output_size)
        self.learning_rate = learning_rate

    def forward(self, input):
        return np.dot(input, self.weight) + self.bias

    def backward(self, input, output_gradient):
        input_gradient = np.dot(output_gradient, self.weight.T)
        weight_gradient = np.dot(input.T, output_gradient)
        
        # Update weights and bias
        self.weight -= self.learning_rate * weight_gradient
        self.bias -= self.learning_rate * np.sum(output_gradient, axis=0)

        return input_gradient

class ReLU:
    def forward(self, input):
        return np.maximum(0, input)

    def backward(self, input, output_gradient):
        grad = input > 0
        return grad * output_gradient

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
    
########## Convolution layer 구현

class filter:
    def __init__ (self, filter_num, channel, height, width, learning_rate):
        self.learning_rate = learning_rate # filter의 learning rate는 0.001
        self.shape = (filter_num, channel, height, width)
        self.weight = np.random.randn(filter_num, channel, height, width) * np.sqrt(2. / (channel * height * width))
    
    def getWeight(self):
        return self.weight
    
    def updateWeight(self, weight_gradient):
        self.weight -= self.learning_rate * weight_gradient

class Convolution:
    def forward(self, input, filter, stride, padding=0):
        batch_size, input_channel, input_height, input_width = input.shape
        filter_num, filter_channel, filter_height, filter_width = filter.getWeight().shape

        input_0padding = np.pad(input, [(0, 0), (0, 0), (padding, padding), (padding, padding)], 'constant')

        activation_map_height = ((input_height - filter_height + 2 * padding) // stride) + 1
        activation_map_width = ((input_width - filter_width + 2 * padding) // stride) + 1

        activation_map = np.zeros((batch_size, filter_num, activation_map_height, activation_map_width))

        for i in range(batch_size):  
            for j in range(filter_num):  
                for k in range(activation_map_height):
                    for z in range(activation_map_width):
                        activation_map[i, j, k, z] = np.sum(
                            input_0padding[i, :, k * stride: k * stride + filter_height, z * stride: z * stride + filter_width] 
                            * filter.getWeight()[j]
                        ) 

        return activation_map

    # backward 연산 함수 (backpropagation)
    def backward(self, input, filter, output_gradient, stride, padding=0):
        batch_size, input_channel, input_height, input_width = input.shape
        filter_num, filter_channel, filter_height, filter_width = filter.getWeight().shape

        input_0padding = np.pad(input, [(0, 0), (0, 0), (padding, padding), (padding, padding)], 'constant')
        input_gradient = np.zeros_like(input_0padding)
        weight_gradient = np.zeros_like(filter.getWeight())

        # 필터 업데이트 위한 계산
        for i in range(batch_size):
            for j in range(filter_num):
                for k in range((input_height - filter_height + 2 * padding) // stride + 1):
                    for z in range((input_width - filter_width + 2 * padding) // stride + 1):
                        weight_gradient[j] += input_0padding[i][:, k * stride: k * stride + filter_height, z * stride: z * stride + filter_width] \
                                              * output_gradient[i, j, k, z]

        # 입력에 대한 그래디언트 계산 (필터를 뒤집어서 합성곱해야 한다고 한다!)
        flipped_filter = np.flip(filter.getWeight(), axis=(2, 3))  # 필터를 180도 뒤집기!!
        output_gradient_0padding = np.pad(output_gradient, [(0, 0), (0, 0), (filter_height - 1, filter_height - 1), (filter_width - 1, filter_width - 1)], 'constant')

        for i in range(batch_size):
            for j in range(input_channel):
                for k in range(input_height):
                    for z in range(input_width):
                        input_gradient[i, j, k, z] = np.sum(
                            output_gradient_0padding[i, :, k:k + filter_height, z:z + filter_width] * flipped_filter[:, j, :, :]
                        )

        # 필터 업데이트
        filter.updateWeight(weight_gradient)

        return input_gradient[:, :, padding:padding + input_height, padding:padding + input_width]


class pooling_filter:
    def __init__ (self, height, width):
        self.shape = (height, width)

class MaxPooling:
    def forward(self, activation_map, pooling_filter, stride):
        batch_size, activation_map_channel, activation_map_height, activation_map_width = activation_map.shape
        filter_height, filter_width = pooling_filter.shape

        output_height = ((activation_map_height - filter_height) // stride) + 1
        output_width = ((activation_map_width - filter_width) // stride) + 1   

        output = np.zeros((batch_size, activation_map_channel, output_height, output_width))
        self.backprop_matrix = np.zeros_like(activation_map)

        for i in range(batch_size):
            for j in range(activation_map_channel):
                for k in range(output_height):
                    for z in range(output_width):
                        pooling_region = activation_map[i, j, 
                                                      k*stride:(k*stride)+filter_height, 
                                                      z*stride:(z*stride)+filter_width]
                        output[i, j, k, z] = np.max(pooling_region)

                        # 최대값의 위치 미리 저장 -> forward에서도 미리 저장했듯이 미리 저장하면 속도 더 빨라지지 않을까!?
                        max_index = np.unravel_index(np.argmax(pooling_region), pooling_region.shape)
                        max_h = k*stride + max_index[0]
                        max_w = z*stride + max_index[1]

                        if max_h < self.backprop_matrix.shape[2] and max_w < self.backprop_matrix.shape[3]:
                            self.backprop_matrix[i, j, max_h, max_w] = 1

        return output

    def backward(self, output_gradient, stride):
        # 입력 크기의 gradient 초기화 (초기값 0)
        input_gradient = np.zeros_like(self.backprop_matrix)

        batch_size, activation_map_channel, output_height, output_width = output_gradient.shape
        filter_height, filter_width = self.backprop_matrix.shape[2] // output_height, self.backprop_matrix.shape[3] // output_width

        for i in range(batch_size):  
            for j in range(activation_map_channel):  
                for k in range(output_height):
                    for z in range(output_width):
                        # backprop_matrix에서 1인 위치에만 output_gradient 전파
                        input_gradient[i, j, k*stride:(k*stride)+filter_height, z*stride:(z*stride)+filter_width] += (
                            self.backprop_matrix[i, j, k*stride:(k*stride)+filter_height, z*stride:(z*stride)+filter_width] 
                            * output_gradient[i, j, k, z]
                        )

        return input_gradient


class ConvLayerNN:
    def __init__(self):
        self.inputs = [None] * 7  # Initialize a list to hold inputs for each layer
        self.convLayer1 = Convolution() # convolution layer 인스턴스
        self.convLayer2 = Convolution() # convolution layer 인스턴스
        self.poolingLayer1 = MaxPooling() # pooling layer 인스턴스
        self.poolingLayer2 = MaxPooling() # pooling layer 인스턴스
        # 세 번째 레이어 인스턴스 -> 마지막 layer의 output에 dependent 하므로 뒤에서 정의함
        self.relu = ReLU()                       # ReLU 인스턴스

    def forward(self, input):
        self.inputs[0] = input

        self.convLayer1_filter = filter(32, self.inputs[0].shape[1], 3, 3, 0.001) # 필터 개수, 채널, 높이, 너비 -> 필터 개수 너무 작으면 업데이트 안될듯 -> 크기 너무 작아지기 때문에
        self.inputs[1] = self.convLayer1.forward(self.inputs[0], self.convLayer1_filter, 1) # input, filter, stride, padding = 0
        self.inputs[2] = self.relu.forward(self.inputs[1])
        self.poolingLayer1_filter = pooling_filter(2,2) # 높이, 너비 # pooling layer로 이미지 사이즈를 줄여야 학습 속도를 효과적으로 높일 수 있다.
        self.inputs[3] = self.poolingLayer1.forward(self.inputs[2], self.poolingLayer1_filter, 2) # input, filter, stride -> stride도 급하게 줄이면 학습을 잘 못한다.

        self.convLayer2_filter = filter(64,self.inputs[3].shape[1], 3, 3, 0.0001) # 필터 개수, 채널, 높이, 너비
        self.inputs[4] = self.convLayer2.forward(self.inputs[3], self.convLayer2_filter, 1) # input, filter, stride, padding = 0
        self.inputs[5] = self.relu.forward(self.inputs[4])
        self.poolingLayer2_filter = pooling_filter(2,2) # 높이, 너비
        self.inputs[6] = self.poolingLayer2.forward(self.inputs[5], self.poolingLayer2_filter, 2)
        
        # 마지막 input 단에 입력 1-dim vector로 넣어주기
        # print(self.inputs[7].shape) # 이 때 reshpe는 마지막 단의 input size를 계산하고 넣어주어야 한다! -> 3*22*22
        # reshaping = self.inputs[7].shape[1]*self.inputs[7].shape[2]*self.inputs[7].shape[3] # 1-dim으로 reshaping 해주기 위해 차원 계산
        # 걍 -1하면 1차원 된다..
        self.linearInput = self.inputs[6].reshape(self.inputs[6].shape[0],-1) # reshaping은 =을 꼭 붙여 주어야 한다!
        self.linearLayer = Linear(self.linearInput.shape[1], 10, 0.00001)   
        output = self.linearLayer.forward(self.linearInput)
        # # 디버깅: 어디 레이어에서 죽었는지 확인하기 위해서!
        # print(output)
        # import sys
        # sys.exit()
        # print ("loading...")
        return output

    def backward(self, output_gradient):
        # backprop 문제: conv간 차원이 다 달라서 연산이 어렵다
        # filter에서 GD 어떻게 할지도 생각해 봐야 한다.
        inputs = self.inputs
        # np.clip(output_gradient, -1, 1, out=output_gradient) # 그래디언트 발산 방지
        output_gradient = self.linearLayer.backward(self.linearInput, output_gradient)
        # 디버깅: linear까지 backward하고 나머지 고정해보기 -> linear만 업뎃해도 loss 준다고 한다. 차근 차근 해보면 어디서 backward가 잘못된지 알 수 있을 것이다.
        # 걍 학습이 잘 안된다. backward forward 구조는 잘 짠듯, parameter 잘 튜닝해 보기!!
        # 해결책1: 파라미터 intializing 잘 하기, 2: learning rate: high -> low로 튜닝하기 => init loss가 17에서 4로 줄었다!!
        # 해결책3: filter 개수 늘리기, 4: learning rate 크기 줄이기
        output_gradient = output_gradient.reshape(inputs[6].shape)
        output_gradient = self.poolingLayer2.backward(output_gradient, 2)
        output_gradient = self.relu.backward(inputs[4], output_gradient)
        output_gradient = self.convLayer2.backward(inputs[3], self.convLayer2_filter, output_gradient, 1)
        output_gradient = self.poolingLayer1.backward(output_gradient, 2)
        output_gradient = self.relu.backward(inputs[1], output_gradient)
        output_gradient = self.convLayer1.backward(inputs[0], self.convLayer1_filter, output_gradient, 1)
