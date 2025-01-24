"""
Aritificial Intelligence Basics @ DGIST
Homework, Part 1
We also provide the Jupyter notebook for this python file. See the pdf guideline.
"""
import numpy as np
import pandas as pd
import re

from collections import defaultdict
from sklearn.model_selection import train_test_split
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB, BernoulliNB, ComplementNB


############################################################################################################
# Part 1 - Utilities to handle datasets.
# We provide the complete implementations for this utilities here, so you would not need to modify this part.
"""
Data loader for the given CSV file
"""
def read_csv():
    df = pd.read_csv("spam.csv", encoding = 'ISO-8859-1')
    df.dropna(inplace = True, axis = 1)
    df['is_spam'] = df.v1.map(lambda x: 1 if x =='spam' else 0)
    df.head()

    subset = df[['v2', 'is_spam']]
    tuples = [tuple(x) for x in subset.values]
    
    # Note: we here fix the random_state for comparison with other models like scikit-learn algorithms
    train_dataset, test_dataset = train_test_split(tuples, test_size = 0.3, random_state = 1)
    return train_dataset, test_dataset


"""
Tokenize the content word by word with spaces for an email
"""
def tokenize(content):
    content = content.lower()
    word = re.findall("[a-z0-9']+", content)
    return set(word)

"""
Get all words in a dataset
"""
def tokenize_dataset(dataset):
    words_set = set()
    for content, _ in dataset:
        words_set = words_set.union(tokenize(content))

    return list(words_set)
############################################################################################################


############################################################################################################
# Part 2 - Probability Model for the training dataset
# You must complete the implementation of the following class.
"""
The Model class that models the probability values during training
"""
class NBModel:
    def __init__(self, train_dataset):
        self.train_dataset = train_dataset
        self.all_words_list = tokenize_dataset(train_dataset)
        pass  # you may want to do some computation here to prepare the results of following member functions

    """
    Task 1. return if a word was presented in the training dataset
    """
    def word_exists(self, word):
        return True  # Modify this

    """
    Task 2. P(S)
    Return the probability (0.0~1.0) that an email is a spam
    """
    def spam_prob(self):
        return 0.0  # Modify this
    
    """
    Task 3. P(word_i|S=spam)
    Return the conditional probability (0.0~1.0) that a spam email has the given word
    """
    def spam_cond_prob(self, word):
        return 0.0  # Modify this

    """
    Task 4. P(word_i|S=ham)
    Return the conditional probability (0.0~1.0) that a ham email has the given word
    """
    def ham_cond_prob(self, word):
        return 0.0  # Modify this

    """
    You can add other member functions and variables if needed
    """
############################################################################################################


############################################################################################################
# Part 3 - Train / inference procedure
# You must include your implementation in the inference() function
"""
Train the model
- Note: you don't need to touch this function
  since you can implement all of your training strategies in the NBModel class
"""
def train(train_dataset):
    return NBModel(train_dataset)

"""
Inference with the model
- Note: you need to fill the ?? parts with the functions that you implement earlier, e.g., model.spam_cond_prob(word), etc
"""
def inference(model, test_dataset):
    n_correct = 0
    for content, is_spam in test_dataset:
        words_set = tokenize(content)  # The set of words in an tested email (i.e., inference)

        ########################################################
        spam_prob = 1.0  # You need to modify this line
        ham_prob = 1.0  # You need to modify this line
        for word in words_set:
            if model.word_exists(word):
                # Implement something here
                # Hint:
                # is_spam_prob *= ??
                # is_ham_prob *= ??
                pass
        ########################################################

        # If the probability of spam is higher than that of ham, then we predict it as a spam
        is_spam_prediction = spam_prob >= ham_prob
        if is_spam_prediction == is_spam:
            n_correct += 1

    n_samples = len(test_dataset)
    accuracy = n_correct / n_samples
    print("Accuracy {} ({} / {})\n".format(accuracy, n_correct, n_samples))
############################################################################################################


############################################################################################################
# Part 4 - Scikit-learn
# You need to modify this to complete your report
def run_sklearn(train_dataset, test_dataset):
    # Prepare the dataset and vectorize (make a matrix that counts the number of the appereances for each word)
    X_train_text, y_train = zip(*train_dataset)
    X_test_text, y_test = zip(*test_dataset)
    n_train = len(X_train_text)

    vectorizer = CountVectorizer()
    X = vectorizer.fit_transform(X_train_text + X_test_text)
    X_train = X[:n_train]
    X_test = X[n_train:]

    # Model Training
    model = MultinomialNB()   # Change this to test other algorithms
    model.fit(X_train, y_train)
    
    # Inference
    preds_test = model.predict(X_test)

    n_correct = 0
    for pred, real in zip(preds_test, y_test):
        if pred == real:
            n_correct += 1

    n_samples = len(test_dataset)
    accuracy = n_correct / n_samples
    print("Accuracy (scikit-learn) {} ({} / {})\n".format(accuracy, n_correct, n_samples))
############################################################################################################


def main():
    train_dataset, test_dataset = read_csv()

    # Run your implementation
    model = train(train_dataset)
    inference(model, test_dataset)

    # Run the sklearn implementation
    run_sklearn(train_dataset, test_dataset)
        

if __name__ == "__main__":
    main()