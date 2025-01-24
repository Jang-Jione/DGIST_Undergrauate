import numpy as np

def SGD(weight, gradient, learning_rate):
    updated_weight = weight - learning_rate*gradient 
    return updated_weight

class Adam:
    # 인터넷 참고
    def __init__(self, beta1=0.9, beta2=0.999, epsilon=1e-8):
        self.beta1 = beta1
        self.beta2 = beta2
        self.epsilon = epsilon
        self.m = None  
        self.v = None  
        self.t = 0    

    def Adam(self, weight, gradient, learning_rate):
        if self.m is None:
            self.m = np.zeros_like(weight)
            self.v = np.zeros_like(weight)

        self.t += 1

        self.m = self.beta1 * self.m + (1 - self.beta1) * gradient
        self.v = self.beta2 * self.v + (1 - self.beta2) * (gradient ** 2)

        m_hat = self.m / (1 - self.beta1 ** self.t)
        v_hat = self.v / (1 - self.beta2 ** self.t)

        updated_weight = weight - learning_rate * m_hat / (np.sqrt(v_hat) + self.epsilon)
        return updated_weight

