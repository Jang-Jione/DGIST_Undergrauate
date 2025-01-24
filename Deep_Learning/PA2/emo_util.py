import csv
import numpy as np
import emoji
import pandas as pd

def read_glove_vecs(glove_file):
    with open(glove_file, 'r', encoding="utf8") as f:
        words = set()
        word_to_vec_map = {}
        for line in f:
            line = line.strip().split()
            curr_word = line[0]
            words.add(curr_word)
            word_to_vec_map[curr_word] = np.array(line[1:], dtype=np.float64)
        
        i = 1
        words_to_index = {}
        index_to_words = {}
        for w in sorted(words):
            words_to_index[w] = i
            index_to_words[i] = w
            i = i + 1
    
    # words to index: 단어를 인덱스로 mapping
    # index to words: 인덱스를 단어로 mapping
    # word to vec map: 단어를 glove vector로 mapping
    return words_to_index, index_to_words, word_to_vec_map


def read_csv(filename = 'emojify_data.csv'):
    # csv file read 함수
    phrase = []
    emoji = []

    with open (filename) as csvDataFile:
        csvReader = csv.reader(csvDataFile)

        for row in csvReader:
            phrase.append(row[0])
            emoji.append(row[1])

    X = np.asarray(phrase)
    Y = np.asarray(emoji, dtype=int)

    return X, Y

# 이모지 딕셔너리 (PA2 PPT 참고)
emoji_dictionary = {"0": "\u2764\uFE0F",    # :heart: prints a black instead of red heart depending on the font
                    "1": ":baseball:",
                    "2": ":smile:",
                    "3": ":disappointed:",
                    "4": ":fork_and_knife:"}

def label_to_emoji(label):
    # label을 emoji로 바꿔주는 함수
    """
    Converts a label (int or string) into the corresponding emoji code (string) ready to be printed
    """
    return emoji.emojize(emoji_dictionary[str(label)])
              
    
def print_predictions(X, pred):
    # just 결과 출력 (이 함수는 사용하지 않을듯)
    print()
    for i in range(X.shape[0]):
        print(X[i], label_to_emoji(int(pred[i])))
    