import numpy as np
import emo_util
import word_embedding
import optimizer

# linear
class Dense:
    def __init__(self, input_size, output_size, learning_rate, optimizer_):
        self.weight = np.random.randn(input_size, output_size) * np.sqrt(2. / (input_size + output_size))
        self.bias = np.zeros(output_size)
        self.learning_rate = learning_rate
        self.optimizer = optimizer_

    def forward(self, input):
        return np.dot(input, self.weight) + self.bias

    def backward(self, input, output_gradient):
        input_gradient = np.dot(output_gradient, self.weight.T)  
        weight_gradient = np.dot(input.T, output_gradient)  

        self.weight = optimization(self.optimizer, self.weight, weight_gradient, self.learning_rate)
        self.bias = optimization(self.optimizer, self.bias, np.sum(output_gradient, axis=0), self.learning_rate)

        return input_gradient

# relu
class ReLU:
    def forward(self, input):
        return np.maximum(0, input)

    def backward(self, input, output_gradient):
        grad = input > 0
        return grad * output_gradient
    
# softmax
def softmax(input):
    exp_input = np.exp(input - np.max(input, axis=1, keepdims=True))
    return exp_input / np.sum(exp_input, axis=1, keepdims=True)

# cross_entropy
def cross_entropy(prediction, ground_truth):
    return -np.sum(ground_truth*np.log(prediction+(1e-10)))

# softmax & lass's forward, backward
class Softmax_loss:
    def forward(self, input, ground_truth):
        loss = cross_entropy(softmax(input), ground_truth)
        return loss
    
    def backward(self, prediction, ground_truth):
        return prediction-ground_truth

# optimizer
def optimization(optimizer_, weight, gradient, learning_rate):
    if optimizer_ == "Adam":
        Adam = optimizer.Adam()
        return Adam.Adam(weight, gradient, learning_rate)
    else:
        return optimizer.SGD(weight, gradient, learning_rate)

# embedding
# PPT에 따르면 embedding layer도 learnable parameter를 가지고 있다.
# 우리가 사용하는 건 이미 pre train된 것!
# Embedding Layer
class Embedding:
    def __init__(self, glove_file):
        self.words_to_index, self.index_to_words, self.word_to_vec_map = emo_util.read_glove_vecs(glove_file)
        self.return_pretrained_embedding(self.word_to_vec_map, self.words_to_index)

    def return_pretrained_embedding(self, word_to_vec_map, words_to_index):
        self.embedding_layer = word_embedding.return_pretrained_embedding(word_to_vec_map, words_to_index)
        return self.embedding_layer

    def forward(self, word_indices):
        return self.embedding_layer.forward(word_indices)

# dropout 
class Dropout:
    def __init__(self, rate):
        self.rate = rate
        self.mask_h = None
        self.mask_c = None

    def forward(self, input):
        h, c = input
        self.mask_h = np.random.rand(*h.shape) > self.rate
        self.mask_c = np.random.rand(*c.shape) > self.rate
        return h * self.mask_h, c * self.mask_c

    def backward(self, output_gradient):
        h_grad, c_grad = output_gradient
        return h_grad * self.mask_h, c_grad * self.mask_c

class LongShortTermMem:
    def __init__(self, input_size, hidden_size, learning_rate, optimizer):
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.learning_rate = learning_rate
        self.optimizer = optimizer

        self.weight_f = np.random.randn(input_size + hidden_size, hidden_size) * np.sqrt(2. / (input_size + hidden_size))
        self.weight_i = np.random.randn(input_size + hidden_size, hidden_size) * np.sqrt(2. / (input_size + hidden_size))
        self.weight_C = np.random.randn(input_size + hidden_size, hidden_size) * np.sqrt(2. / (input_size + hidden_size))
        self.weight_o = np.random.randn(input_size + hidden_size, hidden_size) * np.sqrt(2. / (input_size + hidden_size))

        self.bias_f = np.zeros(hidden_size)
        self.bias_i = np.zeros(hidden_size)
        self.bias_C = np.zeros(hidden_size)
        self.bias_o = np.zeros(hidden_size)

    @staticmethod
    def sigmoid(x):
        return 1 / (1 + np.exp(-x))

    @staticmethod
    def sigmoid_derivative(x):
        return x * (1 - x)

    def forward(self, input, prev_h_state, prev_c_state):
        self.prev_h_state, self.prev_c_state = prev_h_state, prev_c_state
        # h와 input을 합쳐 주어야 한다고 한다. (참고) 이를 concat이라 한다.
        prev_h_state_and_input = np.hstack((prev_h_state, input))

        # lstm computation: PPT 참고
        self.f_gate = self.sigmoid(np.dot(prev_h_state_and_input, self.weight_f) + self.bias_f)
        self.i_gate = self.sigmoid(np.dot(prev_h_state_and_input, self.weight_i) + self.bias_i)
        self.current_c_state_weight = np.tanh(np.dot(prev_h_state_and_input, self.weight_C) + self.bias_C)
        self.current_c_state = self.f_gate * prev_c_state + self.i_gate * self.current_c_state_weight
        self.o_gate = self.sigmoid(np.dot(prev_h_state_and_input, self.weight_o) + self.bias_o)
        self.current_h_state = self.o_gate * np.tanh(self.current_c_state)

        return self.current_h_state, self.current_c_state

    def backward(self, h_output_gradient, c_output_gradient):
        # 위의 gate, state에 대해서 모두 GD 해야 한다.
        # 이 부분은 인터넷 참고
        o_gradient = h_output_gradient * np.tanh(self.current_c_state)
        c_gradient = h_output_gradient * self.o_gate * (1 - np.tanh(self.current_c_state) ** 2)
        c_gradient += c_output_gradient * self.f_gate

        f_gradient = c_gradient * self.prev_c_state * self.sigmoid_derivative(self.f_gate)
        i_gradient = c_gradient * self.current_c_state_weight * self.sigmoid_derivative(self.i_gate)
        c_weight_gradient = c_gradient * self.i_gate * (1 - self.current_c_state_weight ** 2)
        prev_h_input_gradient = np.dot(o_gradient, self.weight_o.T) + \
                                np.dot(f_gradient, self.weight_f.T) + \
                                np.dot(i_gradient, self.weight_i.T) + \
                                np.dot(c_weight_gradient, self.weight_C.T)

        h_gradient = prev_h_input_gradient[:, :self.hidden_size]

        weight_f_gradient = np.dot(prev_h_input_gradient.T, f_gradient)
        weight_i_gradient = np.dot(prev_h_input_gradient.T, i_gradient)
        weight_C_gradient = np.dot(prev_h_input_gradient.T, c_weight_gradient)
        weight_o_gradient = np.dot(prev_h_input_gradient.T, o_gradient)

        bias_f_gradient = np.sum(f_gradient, axis=0)
        bias_i_gradient = np.sum(i_gradient, axis=0)
        bias_C_gradient = np.sum(c_weight_gradient, axis=0)
        bias_o_gradient = np.sum(o_gradient, axis=0)

        self.weight_f = optimization(self.optimizer, self.weight_f, weight_f_gradient, self.learning_rate)
        self.weight_i = optimization(self.optimizer, self.weight_i, weight_i_gradient, self.learning_rate)
        self.weight_C = optimization(self.optimizer, self.weight_C, weight_C_gradient, self.learning_rate)
        self.weight_o = optimization(self.optimizer, self.weight_o, weight_o_gradient, self.learning_rate)

        self.bias_f = optimization(self.optimizer, self.bias_f, bias_f_gradient, self.learning_rate)
        self.bias_i = optimization(self.optimizer, self.bias_i, bias_i_gradient, self.learning_rate)
        self.bias_C = optimization(self.optimizer, self.bias_C, bias_C_gradient, self.learning_rate)
        self.bias_o = optimization(self.optimizer, self.bias_o, bias_o_gradient, self.learning_rate)

        return h_gradient, c_gradient

class LSTM:
    def __init__(self, input_size, hidden_size, output_size, learning_rate, glove_file, dropout_rate=0.5, optimizer = "SGD"):
        self.hidden_size = hidden_size
        self.embedding = Embedding(glove_file)
        self.lstmcell1 = LongShortTermMem(input_size, hidden_size, 1e-15, optimizer)
        self.dropout1 = Dropout(dropout_rate)
        self.lstmcell2 = LongShortTermMem(hidden_size, hidden_size, 1e-15, optimizer)
        self.dropout2 = Dropout(dropout_rate)
        self.relu = ReLU()
        self.dense = Dense(hidden_size, output_size, learning_rate, optimizer)

    def forward(self, inputs):
        batch_size, seq_length = inputs.shape
        self.seq_length = seq_length
        self.inputs = [None] * 7

        self.inputs[0] = inputs
        self.inputs[1] = self.embedding.forward(self.inputs[0])

        self.inputs[2] = np.zeros((batch_size, seq_length, 2, self.hidden_size))  
        self.inputs[4] = np.zeros((batch_size, seq_length, 2, self.hidden_size))  
        h1 = np.zeros((batch_size, self.hidden_size))
        h2 = np.zeros((batch_size, self.hidden_size))
        c1 = np.zeros((batch_size, self.hidden_size))
        c2 = np.zeros((batch_size, self.hidden_size))
        for t in range(seq_length):
            h1, c1 = self.lstmcell1.forward(self.inputs[1][:, t, :], h1, c1)
            h1, c1 = self.dropout1.forward((h1, c1))  
            self.inputs[2][:, t, 0, :] = h1
            self.inputs[2][:, t, 1, :] = c1

            h2, c2 = self.lstmcell2.forward(h1, h2, c2)
            h2, c2 = self.dropout2.forward((h2, c2))  
            self.inputs[4][:, t, 0, :] = h2
            self.inputs[4][:, t, 1, :] = c2

        self.inputs[5] = h2
        self.inputs[6] = self.relu.forward(self.inputs[5])

        output = self.dense.forward(self.inputs[6])
        return output

    def backward(self, output_gradient):
        output_gradient = self.dense.backward(self.inputs[6], output_gradient)
        output_gradient = self.relu.backward(self.inputs[5], output_gradient)

        h2_gradient = output_gradient
        c2_gradient = np.zeros_like(h2_gradient)
        h1_gradient = np.zeros_like(h2_gradient)
        c1_gradient = np.zeros_like(h2_gradient)
        for t in reversed(range(self.seq_length)):
            h2_gradient, c2_gradient = self.dropout2.backward((h2_gradient, c2_gradient))
            h2_gradient, c2_gradient = self.lstmcell2.backward(h2_gradient, c2_gradient)

            h1_gradient, c1_gradient = self.dropout1.backward((h1_gradient, c1_gradient))
            h1_gradient, c1_gradient = self.lstmcell1.backward(h1_gradient, c1_gradient)

        return h1_gradient

