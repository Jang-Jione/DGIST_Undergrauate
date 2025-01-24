import matplotlib.pyplot as plt

def loss_graph(figure, train):
    plt.figure(figure)
    
    plt.xlabel("iteration")
    plt.ylabel("loss")
    
    plt.plot(train, label='train Loss')  # Train Loss 라인
    
    plt.legend()
    plt.title('loss graph')
    plt.show()
