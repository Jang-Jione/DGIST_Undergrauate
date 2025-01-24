import emo_util
import LSTM
import numpy as np
import evaluation_code

########## 데이터 로드 
# X contains 127 sentences (strings): 문장
# Y contains a integer label between 0 and 4 corresponding to an emoji for each sentence: 이모지에 대응되는 숫자로 표현된 label

X_train, Y_train = emo_util.read_csv('G:/emo/dataset/train_emoji.csv')
X_test, Y_test = emo_util.read_csv('G:/emo/dataset/test_emoji.csv')

########## 데이터 전처리
# 여기서는 loader 사용하지 않았으므로 추가 전처리를 진행해 주어야 한다.
# 1. data vectorization
word_to_index, index_to_word, word_to_vec_map = emo_util.read_glove_vecs('G:/emo/embedding/glove.6B.50d.txt')
max_len = len(max(X_train, key=len).split())  # 문장의 최대 길이 계산

#####
# word_to_index에 <UNK>와 <PAD> 토큰 추가
word_to_index["<PAD>"] = len(word_to_index)
word_to_index["<UNK>"] = len(word_to_index)

def sentences_to_indices(sentences, word_to_index, max_len):
    # 인덱스 return 
    sentences_indices = []
    
    for sentence in sentences:
        sentence_words = sentence.split()
        sentence_indices = [word_to_index.get(word.lower(), word_to_index.get("<UNK>", 0)) for word in sentence_words]
        sentence_indices = sentence_indices[:max_len] + [word_to_index.get("<PAD>", 0)] * (max_len - len(sentence_indices))
        sentences_indices.append(sentence_indices)
    
    return np.array(sentences_indices)

def one_hot_encoding(labels, C):
    # one-hot 인코딩 하는 과정 -> PA1에서는 loader가 해줬는데, 여기서는 우리가 해주어야 한다.
    # labels: 정수형 레이블 (0부터 C-1까지)
    one_hot_labels = np.eye(C)[labels]  # eye(C)로 C x C 크기의 단위 행렬을 만들고, 인덱스로 해당 원소를 선택
    return one_hot_labels
#####

X_train_indices = sentences_to_indices(X_train, word_to_index, max_len)
X_test_indices = sentences_to_indices(X_test, word_to_index, max_len)

# 2. one-hot encoding
Y_train_oh = one_hot_encoding(Y_train, C=5)
Y_test_oh = one_hot_encoding(Y_test, C=5)

########## 초기화
# 모델 정의
model = LSTM.LSTM(input_size=100,  # input size는 glove(=embedding)에 dependent한 값
            hidden_size=128,
            output_size=5,
            learning_rate=0.1,
            glove_file='G:/emo/embedding/glove.6B.100d.txt',
            dropout_rate=0.0,
            optimizer="SGD") # 여기서는 drop out 0으로 안하면 성능 향상이 더디다.

# cross_entropy 정의
softmax_loss = LSTM.Softmax_loss()

# epoch 설정
epochs = 700
batch_size = 16  # 배치 크기

# loss 기록용 리스트
train_loss_buffer = []

########## 학습
for epoch in range(epochs):
    # 평가를 위한 metric var
    train_loss = 0
    train_correct = 0

    test_loss = 0
    test_correct = 0
    
    # 데이터를 배치 크기만큼 나누어서 학습
    num_batches = len(X_train) // batch_size
    for batch_idx in range(num_batches):
        # 배치 데이터 추출
        start_idx = batch_idx * batch_size
        end_idx = start_idx + batch_size
        X_batch = X_train_indices[start_idx:end_idx]
        Y_batch = Y_train_oh[start_idx:end_idx]

        # forward Pass
        train_output = model.forward(X_batch)
        train_prediction = LSTM.softmax(train_output) 
        train_loss += softmax_loss.forward(train_output, Y_batch)  # epoch에서 평가를 위한 코드
        train_correct += np.sum(np.argmax(train_prediction, axis=1) == np.argmax(Y_batch, axis=1))

        # backward Pass
        output_gradient = softmax_loss.backward(train_prediction, Y_batch)
        model.backward(output_gradient)

    train_loss_buffer.append(train_loss / len(X_train))
    print(f"Epoch {epoch+1}/{epochs}, Loss: {train_loss/len(X_train)}, Accuracy: {train_correct/len(X_train)*100:.2f}%")

########## 테스트
test_loss = 0
test_correct = 0
test_output = model.forward(X_test_indices)
test_prediction = LSTM.softmax(test_output)
test_loss += softmax_loss.forward(test_output, Y_test_oh)
test_correct += np.sum(np.argmax(test_prediction, axis=1) == np.argmax(Y_test_oh, axis=1))

data_len = len(X_test)
print(f"Test Loss: {test_loss/data_len}, Test Accuracy: {test_correct/data_len*100:.2f}%")

########## 평가
evaluation_code.loss_graph(1, train_loss_buffer)

emo_util.print_predictions(X_test, np.argmax(test_prediction, axis=1))
