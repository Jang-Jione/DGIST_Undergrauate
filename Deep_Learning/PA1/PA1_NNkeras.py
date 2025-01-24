import dataloader
import keras
import numpy as np
import evaluation_code
import matplotlib.pyplot as plt

########## 데이터 로드

dataloader_train = dataloader.Dataloader('G:/mnist/dataset', is_train=True, shuffle=True, batch_size=100)
dataloader_test = dataloader.Dataloader('G:/mnist/dataset', is_train=False, shuffle=True, batch_size=50)

(X_test, Y_test) = dataloader_test.images, dataloader_test.labels
(X_train, Y_train) = dataloader_train.images, dataloader_train.labels

X_test = X_test / 255.0
X_train = X_train / 255.0
# 여기서는 전처리 해주기!! 여기서는 loader 도움 안받고 model이 알아서 stocastic gd를 하기 때문에!!

# sklearn 사용할 때는 셔플해 주어야 한다!! 안하면 0라벨만 학습하고, 조기 종료한다!!

X_test = X_test.reshape(X_test.shape[0], 28*28)
X_train = X_train.reshape(X_train.shape[0], 28*28)

y_train = np.argmax(Y_train, axis=1) # y가 원 핫 인코딩 되어 있으면 안된다고 한다.
y_test = np.argmax(Y_test, axis=1)    

########## initializing
# 모델 정의 ++ karas에서는 linear layer 쌓아서 구현할 수 있다. 여기서는 classifier 구현 되어 있는 거 한 번 써보자!
# 아.. 분석 할 때는 keras가 더 좋은 것 같다.. sklearn 이 자식.. batch size도 못 정한다고 한다..
# 그냥 karas 사용하자
# model = MLPClassifier(hidden_layer_sizes=(64,100,10), max_iter=10, solver='sgd', verbose=1)

# 모델 정의
model = keras.Sequential([
    keras.layers.Dense(63, input_shape=(784,)),
    keras.layers.Activation('relu'),
    keras.layers.Dense(100),
    keras.layers.Activation('relu'),
    keras.layers.Dense(10),
    keras.layers.Activation('softmax'),
])

model.compile(optimizer="sgd", loss="sparse_categorical_crossentropy")
history = model.fit(X_train, y_train, epochs=500, batch_size=100, validation_data=(X_test, y_test))

# loss graph 그리기 (x축: iteration, y축: loss)
train_loss = history.history['loss']
test_loss = history.history['val_loss']

x_len = np.arange(len(train_loss))
plt.plot(x_len, train_loss, label="train loss")
plt.plot(x_len, test_loss, label="test loss")
plt.xlabel("iteration") 
plt.ylabel("loss")
plt.legend()
plt.title('loss graph')
plt.show()

# confusion matrix 그리기! 여기 input은 원 핫 인코딩 안되어 있는 형태
test_prediction = model.predict(X_test)
evaluation_code.confusion_matrix_(2, test_prediction, Y_test)

# top-3 images with probability 그리기
evaluation_code.top_3_images(3, test_prediction, X_test, Y_test)