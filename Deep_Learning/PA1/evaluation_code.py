import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import confusion_matrix

def loss_graph(figure, train, test):
    plt.figure(figure)
    
    plt.xlabel("iteration")
    plt.ylabel("loss")
    
    plt.plot(train, label='train Loss')  # Train Loss 라인
    plt.plot(test, label='test Loss')  # Test Loss 라인
    
    plt.legend()
    plt.title('loss graph')
    plt.show()

def confusion_matrix_(figure, prediction, Y_test):
    plt.figure(figure)
    # arumax 함수 사용해서 레이블 변환하기
    gt = np.argmax(Y_test, axis=1)
    prediction = np.argmax(prediction, axis=1)

    # sklearn lib 사용해서 confusion matrix 구현!
    cm = confusion_matrix(gt, prediction)

    # 각 값 확률로 바꾸기!
    cm_prob = cm.astype('float') / cm.sum(axis=1)[:, np.newaxis]

    sns.heatmap(np.round(cm_prob), annot=True, fmt=".2f", cmap='Blues', cbar=True)
    plt.xlabel('prediction')
    plt.ylabel('gt')
    plt.title('confusion matrix')
    plt.show()

def top_3_images(figure, prediction, X_test, Y_test):
    # 레이블 변환 (argmax 사용)
    gt = np.argmax(Y_test, axis=1)
    pred = np.argmax(prediction, axis=1)

    top_3_list = [[] for _ in range(10)]  # 각 클래스별로 top 3 이미지 저장할 리스트

    # top 3 image 가져오기
    for i in range(len(gt)):
        true_label = gt[i]
        pred_label = pred[i]
        pred_prob = np.max(prediction[i])  # 해당 예측의 최대 확률 값

        # 실제 라벨과 예측이 일치하는 경우에만
        if true_label == pred_label:
            # 해당 클래스에 저장된 이미지가 3개 이하일 때만 추가
            if len(top_3_list[true_label]) < 3:
                top_3_list[true_label].append((X_test[i], pred_prob))
            else:
                # 이미 3개 이상 저장되었다면, 확률이 더 높은 경우만 교체
                min_prob_index = np.argmin([p for _, p in top_3_list[true_label]])
                if pred_prob > top_3_list[true_label][min_prob_index][1]:
                    top_3_list[true_label][min_prob_index] = (X_test[i], pred_prob)

    # 하나의 figure에 모든 라벨의 top 3 이미지 시각화 (왼쪽에 라벨)
    fig, axs = plt.subplots(10, 4, figsize=(15, 40))  # 10개의 라벨, 각 라벨당 3개의 이미지 + 1열은 라벨 표시
    fig.suptitle("top-3 images with probability", fontsize=10)

    for label in range(10):
        # 첫 번째 열에는 라벨을 추가
        axs[label, 0].text(0.5, 0.5, f"label {label}", fontsize=14, ha='center', va='center')
        axs[label, 0].axis('off')  # 텍스트만 추가하고 축은 끔
        
        # 나머지 3개의 열에 이미지를 추가
        if top_3_list[label]:
            for j, (img, prob) in enumerate(top_3_list[label]):
                ax = axs[label, j + 1]  # 첫 번째 열(0)은 라벨에 사용하므로 j+1부터 이미지 삽입
                ax.imshow(img.reshape(28, 28), cmap='gray')  # 1D 배열을 28x28로 변환
                ax.axis('off')  # 이미지 축 숨김

                # 확률을 이미지 오른쪽에 추가
                axs[label, j + 1].text(1.1, 0.5, f"prob: {prob:.2f}", fontsize=10, ha='left', va='center', transform=ax.transAxes)  # 오른쪽에 확률 추가

        else:
            # 이미지가 없는 경우 나머지 서브플롯 숨김
            for j in range(1, 4):
                axs[label, j].axis('off')

    plt.tight_layout(rect=[0, 0, 1, 0.96])  # 전체 레이아웃 조정
    plt.show()
