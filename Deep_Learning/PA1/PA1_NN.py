import dataloader
import NN
import numpy as np
import evaluation_code
import matplotlib.pyplot as plt

########## 데이터 로드

dataloader_train = dataloader.Dataloader('G:/mnist/dataset', is_train=True, shuffle=True, batch_size=100)
dataloader_test = dataloader.Dataloader('G:/mnist/dataset', is_train=False, shuffle=True, batch_size=600)

(X_test, Y_test) = dataloader_test.images, dataloader_test.labels
# train은 enumerate하면서 가져온다.

# 데이터 로드 확인하는 코드
# plt.imshow(X_train[0].squeeze(), cmap='Greys')
# plt.show()

########## 데이터 전처리

# X_train = X_train.astype(np.float64)
# X_test = X_test.astype(np.float64)
# load 메서드에서 해주고 있다.

# X_train = X_train / 255
# X_test = X_test / 255
# getitem 메서드에서 해주고 있다.

# reshaping: train은 batch 부분에서 해준다.
X_test = X_test.reshape(X_test.shape[0], 28*28)

# y to one-hot 인코딩 해주기 => 이미 one hot encoding이 된 상태이다.
# def one_hot_encoding(y, num_classes):
#     y = y.astype(int)  # 배열 전체를 정수형으로 변환하려면 astype을 써야 한다고 한다.
#     # num_classes: 레이블의 클래스 개수
#     one_hot = np.zeros((y.shape[0], num_classes))
#     one_hot[np.arange(y.shape[0]), y] = 1
#     return one_hot

# Y_train = one_hot_encoding(Y_train, 10)
# Y_test = one_hot_encoding(Y_test, 10)

########## initializing
# 모델 정의
model = NN.ThreeLayerNN()

# cross_entropy 정의
softmax_loss = NN.Softmax_loss()

# epoch 설정
epochs = 1

########## training

train_loss_at_iter_list = [] #graph 그리기 위해
test_loss_at_iter_list = []

for epoch in range(epochs):
    # 평가를 위한 metric var
    train_loss = 0
    train_loss_at_iter = 0
    train_correct = 0

    test_loss = 0
    test_loss_at_iter = 0
    test_correct = 0
    
    for batch_idx, batch in enumerate(dataloader_train):
        # 배치 단위로 데이터 가져오기
        # dataloader_train.batch_size = 8(default 값)
        X_batch, Y_batch = batch

        # input을 1-dim vector로 변경
        # print(X_batch.shape): (100, 1, 28, 28) -> 이걸 (100, 28*28)로 변환하기!!
        X_batch = X_batch.reshape(X_batch.shape[0], 28*28)
        # (function) reshape: Any 이건 무슨 뜻인지 모르겠다 => 그냥 vscode 에러인듯

        # forward Pass
        # print(X_batch[0])
        train_output = model.forward(X_batch)
        train_prediction = NN.softmax(train_output) 
        train_loss += softmax_loss.forward(train_output, Y_batch) # epoch에서 평가를 위한 코드
        train_loss_at_iter = softmax_loss.forward(train_output, Y_batch) # figure에서 평가를 위한 코드
        train_correct += np.sum(np.argmax(train_prediction, axis=1) == np.argmax(Y_batch, axis=1))
        
        train_loss_at_iter_list.append(train_loss_at_iter/dataloader_train.batch_size)

        # backward pass
        output_gradient = softmax_loss.backward(train_prediction, Y_batch) 
        model.backward(output_gradient)

        ########## testing
        test_output = model.forward(X_test)
        test_prediction = NN.softmax(test_output) 
        test_loss += softmax_loss.forward(test_output, Y_test) # epoch에서 평가를 위한 코드
        test_loss_at_iter = softmax_loss.forward(test_output, Y_test) # figure에서 평가를 위한 코드
        test_correct += np.sum(np.argmax(test_prediction, axis=1) == np.argmax(Y_test, axis=1))
        
        test_loss_at_iter_list.append(test_loss_at_iter/(dataloader_test.batch_size*len(dataloader_test)))
        ##########
        # # 디버깅
        # print(f"Batch {batch_idx+1}, Output: {output[0]}, Prediction: {prediction[0]}, Loss: {softmax_loss.forward(output, Y_batch)}")
        # 한 가지 의문점: testing과 backward 위치 바꾸면 matrix size error 생긴다. 
        # => input instance가 바뀌어서 그런 것 같다. forward와 backward 사이에 test를 넣으면 안된다!

    # Epoch당 평균 loss와 accuracy 출력하기
    # dataloader_train.batch_size: 배치 사이즈
    # dataloader_train: 배치 개수
    # X_train: 전체 데이터 개수
    data_len = dataloader_train.batch_size*len(dataloader_train)
    print(f"Epoch {epoch+1}/{epochs}, Loss: {train_loss/data_len}, Accuracy: {train_correct/data_len*100:.2f}%")

########## testing
test_loss = 0
test_correct = 0
test_output = model.forward(X_test)
test_prediction = NN.softmax(test_output) 
test_loss += softmax_loss.forward(test_output, Y_test) # epoch에서 평가를 위한 코드
test_correct += np.sum(np.argmax(test_prediction, axis=1) == np.argmax(Y_test, axis=1))

data_len = dataloader_test.batch_size*len(dataloader_test)
print(f"Test Loss: {test_loss/data_len}, Test Accuracy: {test_correct/data_len*100:.2f}%")
##########

########## 정리

# testing: test data도 보통 한 번에 처리하지 않고, 배치 사이즈로 처리한다고 한다!!
# initializing: loss/correct init 안하면 위에 값이 덮어씌워 진다...

# loss graph 그리기 (x축: iteration, y축: loss)
evaluation_code.loss_graph(1, train_loss_at_iter_list, test_loss_at_iter_list)

# confusion matrix 그리기
evaluation_code.confusion_matrix_(2, test_prediction, Y_test)

# top-3 images with probability 그리기
evaluation_code.top_3_images(3, test_prediction, X_test, Y_test)