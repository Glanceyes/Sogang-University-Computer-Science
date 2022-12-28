#-*- coding: utf-8 -*-

import datetime
import time
import sys
import MeCab
import operator
from pymongo import MongoClient
from bson import ObjectId
from itertools import combinations

import pprint
"""
class MyPrettyPrinter(pprint.PrettyPrinter):
    def format(self, _object, context, maxlevels, level):
        if isinstance(_object, unicode):
            return "'%s'" % _object.encode('utf8'), True, False
        elif isinstance(_object, str):
            _object = unicode(_object,'utf8')
            return "'%s'" % _object.encode('utf8'), True, False
        return pprint.PrettyPrinter.format(self, _object, context, maxlevels, level)
"""
def itemSetName(number):
    numberString = {1: "1", 2: "2", 3: "3"}.get(number, "3")
    setName = "candidate_L" + numberString
    return setName

def makeCandidateL1(allWordSet, col):
    for doc in col.find({}, {"word_set": 1, "_id": 0}):
        for word in doc['word_set']:
            if word in allWordSet.keys():
                allWordSet[word] += 1
    """
    print MyPrettyPrinter().pformat(allWordSet)
    """
def makeCandidateL2(newWordList, col):
    candidateL2 = {}
    newWordListKeys = []
    for item in newWordList:
        newWordListKeys.extend(item['item_set'])
        """
        print MyPrettyPrinter().pformat(item['item_set'])
        """
    
    for counter1 in range(len(newWordListKeys)):
        for counter2 in range(counter1 + 1, len(newWordListKeys)):
            keysList = [newWordListKeys[counter1], newWordListKeys[counter2]]
            keysSet = frozenset(keysList)
            candidateL2[keysSet] = 0
            for doc in col.find({}, {"word_set": 1, "_id": 0}):
                if keysSet.issubset(set(doc['word_set'])) is True:
                    candidateL2[keysSet] += 1
            if candidateL2[keysSet] is 0:
                del candidateL2[keysSet]
    return candidateL2

def makeCandidateL3(newWordList, col):
    candidateL3 = {}
    newWordListKeys = []
    for item in newWordList:
        itemSet = set(item['item_set'])
        newWordListKeys.append(itemSet)
    
    for counter1 in range(len(newWordListKeys)):
        for counter2 in range(counter1 + 1, len(newWordListKeys)):
            keys = newWordListKeys[counter1] | newWordListKeys[counter2]
            if len(keys) is 3:
                keysSet = frozenset(keys)
                candidateL3[keysSet] = 0
            else: continue
            for doc in col.find({}, {"word_set": 1, "_id": 0}):
                if keysSet.issubset(set(doc['word_set'])) is True:
                    candidateL3[keysSet] += 1
            if candidateL3[keysSet] is 0:
                del candidateL3[keysSet]

    return candidateL3

def makeNewWordList(allWordSet, min_sup, col):
    newWordList = []
    for keys, value in allWordSet.items():
        itemSet = []        
        if type(keys) is frozenset:
            itemSet = list(keys)
        else:
            itemSet.append(keys)
        if value >= min_sup:
            newWordList.append({"item_set": itemSet, "support": value})
    col.drop()
    col.insert(newWordList)
    
    return newWordList
    """
    print MyPrettyPrinter().pformat(newWordSet)
    """
def strongRuleL2(col1, col2, min_conf):
    for doc2 in col2.find({}, {"item_set": 1, "support": 1, "_id": 0}):
        item1 = doc2["item_set"][0]
        item2 = doc2["item_set"][1]
        """      
        print MyPrettyPrinter().pformat(doc2["item_set"])
        """
        counter1 = 0
        counter2 = doc2['support']

        for doc1_1 in col1.find({"item_set": item1}, {"support": 1, "_id": 0}):
            counter1 += doc1_1['support']
        """
        print("counter1: ", counter1, "counter2:", counter2)
        """
        if counter1 > 0:
            confidence = float(counter2) / float(counter1)
            if confidence >= min_conf:
                print("%s => %s %s" %(item1, item2, confidence))
        
        counter1 = 0
        for doc1_2 in col1.find({"item_set": item2}, {"support": 1, "_id": 0}): 
            counter1 += doc1_2['support']
        
        if counter1 > 0:
            confidence = float(counter2) / float(counter1)
            if confidence >= min_conf:
                print("%s => %s %s" %(item2, item1, confidence))

def strongRuleL3(col1, col2, col3, min_conf):
    for doc3 in col3.find({}, {"item_set": 1, "support": 1, "_id": 0}):
        item1 = doc3['item_set'][0]
        item2 = doc3['item_set'][1]
        item3 = doc3['item_set'][2]

        itemList1 = [item1, item2]
        itemList2 = [item1, item3]
        itemList3 = [item2, item3]
        
        itemList = []
        itemList.append(itemList1)
        itemList.append(itemList2)
        itemList.append(itemList3)

        itemExceptList = []
        itemExceptList.append(item3)
        itemExceptList.append(item2)
        itemExceptList.append(item1)
        
        counter2 = doc3["support"]

        for i in range(0, 3):
            counter1 = 0
            for doc2 in col2.find({}, {"item_set": 1, "support" : 1, "_id": 0}):
                if set(itemList[i]).issubset(set(doc2["item_set"])):
                    """
                    print MyPrettyPrinter().pformat(doc2["item_set"])
                    """
                    counter1 += doc2["support"]
            if counter1 > 0:
                confidence = float(counter2) / float(counter1)
                if confidence >= min_conf:
                    print("%s, %s => %s %s" %(itemList[i][0], itemList[i][1], itemExceptList[i], confidence))
        for i in range(0, 3):
            counter1 = 0
            for doc1 in col1.find({"item_set": itemExceptList[i]}, {"support" : 1, "_id": 0}):
                counter1 += doc1["support"]
            if counter1 > 0:
                confidence = float(counter2) / float(counter1)
                if confidence >= min_conf:
                    print("%s => %s, %s %s" %(itemExceptList[i], itemList[i][0], itemList[i][1], confidence))

def make_stop_word():
    f = open("wordList.txt", 'r')
    while True:
        line = f.readline()
        if not line: break
        stop_word[line.strip('\n')] = line.strip('\n')
    f.close()

def morphing(content):
    t = MeCab.Tagger('-d/usr/local/lib/mecab/dic/mecab-ko-dic')
    nodes = t.parseToNode(content.encode('utf-8'))
    MorpList = []
    while nodes:
        if nodes.feature[0] == 'N' and nodes.feature[1] == 'N':
            w = nodes.surface
            if not w in stop_word:
                try:
                    w = w.encode('utf-8')
                    MorpList.append(w)
                except:
                    pass
        nodes = nodes.next
    return MorpList

def printMenu():
    print "0. CopyData"
    print "1. Morph"
    print "2. print morphs"
    print "3. print wordset"
    print "4. frequent item set"
    print "5. association rule"

def p0():
    col1 = db['news']
    col2 = db['news_freq']

    col2.drop()

    for doc in col1.find():
        contentDic = {}
        for key in doc.keys():
            if key != "_id":
                contentDic[key] = doc[key]
        col2.insert(contentDic)

def p1():
    for doc in db['news_freq'].find():
        doc['morph'] = morphing(doc['content'])
        db['news_freq'].update({"_id":doc['_id']}, doc)


def p2(url):
    findNews = db['news_freq'].find_one({"url": url})
    if findNews is not None:
        morphData = findNews['morph']
        
        for morph in morphData:
            print(morph)
    else:
        print("Cannot Find News Corresponding to the Given URL in 'news_freq'.")
    
def p3():
    col1 = db['news_freq']
    col2 = db['news_wordset']
    col2.drop()
    for doc in col1.find():
        new_doc = {}
        new_set = set()
        for w in doc['morph']:
            new_set.add(w.encode('utf-8'))
        new_doc['word_set'] = list(new_set)
        new_doc['url'] = doc['url']
        col2.insert(new_doc)

def p4(url):
    findNews = db['news_wordset'].find_one({"url": url})
    if findNews is not None:
        wordData = findNews['word_set']

        for word in wordData:
            print(word)
    else:
        print("Cannot Find News Corresponding to the Given URL in 'news_wordset'.")
    
def p5(length):
    col1 = db['news']
    col2 = db['news_wordset']

    min_sup = col1.count() * 0.1
    
    allWordSet = {}
    for doc in col2.find():
        for word in doc['word_set']:
            if word not in allWordSet.keys():
                allWordSet[word] = 0

    if length > 3:
        length = 3

    if length is 1:
        makeCandidateL1(allWordSet, col2)
        
        setName = itemSetName(length)
        db[setName].drop()
        makeNewWordList(allWordSet, min_sup, db[setName])
    elif length is 2:
        newWordList = []
        makeCandidateL1(allWordSet, col2)

        setName = itemSetName(length - 1)
        db[setName].drop()
        newWordList = makeNewWordList(allWordSet, min_sup, db[setName])

        candidateL2 = {}
        candidateL2 = makeCandidateL2(newWordList, col2)
       
        setName = itemSetName(length)
        db[setName].drop()
        makeNewWordList(candidateL2, min_sup, db[setName])
    elif length is 3:
        newWordList = []
        makeCandidateL1(allWordSet, col2)
        
        setName = itemSetName(length - 2)
        db[setName].drop()
        newWordList = makeNewWordList(allWordSet, min_sup, db[setName])
        
        candidateL2 = {}
        candidateL2 = makeCandidateL2(newWordList, col2)

        setName = itemSetName(length - 1)
        db[setName].drop()
        newWordList = makeNewWordList(candidateL2, min_sup, db[setName])

        candidateL3 = {}
        candidateL3 = makeCandidateL3(newWordList, col2)

        setName = itemSetName(length)
        db[setName].drop()
        makeNewWordList(candidateL3, min_sup, db[setName])

def p6(length):
    min_conf = 0.5
    if length is 2:
        col1 = db[itemSetName(length - 1)]
        col2 = db[itemSetName(length)]
        
        strongRuleL2(col1, col2, min_conf)
    if length is 3:
        col1 = db[itemSetName(length - 2)]
        col2 = db[itemSetName(length - 1)]
        col3 = db[itemSetName(length)]

        strongRuleL3(col1, col2, col3, min_conf)

stop_word = {}
DBname = "db20171665"
conn = MongoClient('dbpurple.sogang.ac.kr')
db = conn[DBname]
db.authenticate(DBname, DBname)

if __name__ == "__main__":
    make_stop_word()
    printMenu()
    selector = input()
    if selector == 0:
        p0()
    elif selector == 1:
        p1()
        p3()
    elif selector == 2:
        url = str(raw_input("input news url:"))
        p2(url)
    elif selector == 3:
        url = str(raw_input("input news url:"))
        p4(url)
    elif selector == 4:
        length = int(raw_input("input length of the frequent item:"))
        p5(length)
    elif selector == 5:
        length = int(raw_input("input length of the frequent item:"))
        p6(length)

