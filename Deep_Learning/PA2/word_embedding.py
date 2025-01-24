# import keras
import numpy as np

import numpy as np

class Embedding:
    def __init__(self, vocab_len, glove_matrix):

        self.vocab_len = vocab_len  
        self.glove_dim = glove_matrix.shape[1]  
        self.glove_matrix = glove_matrix 

    def forward(self, input_indices):
        return self.glove_matrix[input_indices]

def return_pretrained_embedding(word_to_vec_map, words_to_index):

    vocab_len = len(words_to_index) + 1  # 모델이 다룰 수 있는 단어의 크기, 여기서 +1은 padding
    glove_dim = word_to_vec_map["cucumber"].shape[0]  # 차원 가져오기 50 or 100
    glove_matrix = np.zeros((vocab_len, glove_dim)) # 각 단어마다 emb_dim 차원에 저장
    # 질문: 이렇게 1대1 mapping하면 새로 들어오는 단어는 어떻게 임베딩할까? -> 이를 위해 "<UNK>"라는 걸 추가해 주어야 한다.

    for word, index in words_to_index.items():
        glove_matrix[index, :] = word_to_vec_map[word]
    # matrix 생성

    embedding_layer = Embedding(vocab_len, glove_matrix)

    return embedding_layer
