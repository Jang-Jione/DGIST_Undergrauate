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
# 이는 keras lib 사용해서 가져와야 한다.
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

    def forward(self, input):
        self.mask = np.random.rand(*input.shape) > self.rate
        return input * self.mask

    def backward(self, output_gradient):
        return output_gradient * self.mask

class RecurrentNet:
    def __init__(self, input_size, hidden_size, learning_rate, optimizer_):
        self.weight_xh = np.random.randn(input_size, hidden_size) * np.sqrt(2. / (input_size + hidden_size))
        self.weight_hh = np.random.randn(hidden_size, hidden_size) * np.sqrt(2. / (hidden_size + hidden_size))
        self.bias = np.zeros((1, hidden_size))
        self.learning_rate = learning_rate
        self.optimizer = optimizer_ 

    def forward(self, input, prev_h_layer):
        self.input = input
        self.prev_h_layer = prev_h_layer
        self.current_h_layer = np.tanh(
            np.dot(input, self.weight_xh) + np.dot(prev_h_layer, self.weight_hh) + self.bias
        )
        return self.current_h_layer

    def backward(self, input, output_gradient):
        tanh_derivative = 1 - self.current_h_layer ** 2
        output_gradient *= tanh_derivative

        weight_xh_gradient = np.dot(input.T, output_gradient)
        weight_hh_gradient = np.dot(self.prev_h_layer.T, output_gradient)
        bias_gradient = np.sum(output_gradient, axis=0, keepdims=True)

        self.weight_xh = optimization(self.optimizer, self.weight_xh, weight_xh_gradient, self.learning_rate)
        self.weight_hh = optimization(self.optimizer, self.weight_hh, weight_hh_gradient, self.learning_rate)
        self.bias = optimization(self.optimizer, self.bias, bias_gradient, self.learning_rate)

        output_gradient = np.dot(output_gradient, self.weight_hh.T)
        return output_gradient


class RNN:
    def __init__(self, input_size, hidden_size, output_size, learning_rate, glove_file, dropout_rate=0.5, optimizer = "SGD"):
        self.hidden_size = hidden_size
        self.embedding = Embedding(glove_file)
        self.rnncell1 = RecurrentNet(input_size, hidden_size, 0.00001, optimizer)
        self.dropout1 = Dropout(dropout_rate)
        self.rnncell2 = RecurrentNet(hidden_size, hidden_size, 0.00001, optimizer)
        self.dropout2 = Dropout(dropout_rate)
        self.dense = Dense(hidden_size, output_size, learning_rate, optimizer)
        self.relu = ReLU()
        self.learning_rate = learning_rate

    def forward(self, inputs):
        batch_size, seq_length = inputs.shape
        self.seq_length = seq_length
        self.inputs = [None] * 7

        # Embedding layer
        self.inputs[0] = inputs
        self.inputs[1] = self.embedding.forward(self.inputs[0])

        # Hidden states
        self.inputs[2] = np.zeros((batch_size, seq_length, self.hidden_size))
        self.inputs[4] = np.zeros((batch_size, seq_length, self.hidden_size))
        h1 = np.zeros((batch_size, self.hidden_size))
        h2 = np.zeros((batch_size, self.hidden_size))
        for t in range(seq_length):
            h1 = self.rnncell1.forward(self.inputs[1][:, t, :], h1)
            self.inputs[2][:, t] = h1
            self.inputs[3] = self.dropout1.forward(h1)

            h2 = self.rnncell2.forward(self.inputs[3], h2)
            self.inputs[4][:, t] = h2

        self.inputs[5] = self.dropout2.forward(h2)
        self.inputs[6] = self.relu.forward(self.inputs[5])

        output = self.dense.forward(self.inputs[6])
        return output

    def backward(self, output_gradient):
        output_gradient = self.dense.backward(self.inputs[6], output_gradient)
        output_gradient = self.relu.backward(self.inputs[5], output_gradient)
        output_gradient = self.dropout2.backward(output_gradient)

        h2_gradient = output_gradient
        h1_gradient = np.zeros_like(h2_gradient)
        for t in reversed(range(self.seq_length)):
            h2_gradient = self.rnncell2.backward(self.inputs[2][:, t], h2_gradient)
            h2_gradient = self.dropout1.backward(h2_gradient)
            h1_gradient = self.rnncell1.backward(self.inputs[1][:, t, :], h2_gradient)

        return h1_gradient
