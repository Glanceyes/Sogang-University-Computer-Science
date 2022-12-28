class ReadEliminateSignFromPaper:
    def __init__(self, file):
        # chnaging the way of file I/O is possible
        with open(file, 'r', encoding='utf8') as f:
            paper = f.readlines() # a list that contains paragraph-wise elements. note: each paragraph has different number of sentences
            
        self.sign_dict = dict() # a dictionary whose key and value are the eliminated sign and the number of that sign, respectively
        self.sentences = list() # a list that contains sentence-wise elements
        # changing data-type or name of variables above is possible
        
        #########################################################
        # complete the code below, following assignment guideline
       
        # eliminate signs from each word
        for paragraph in paper:
            # if endline character is found at the end of paragraph, remove it
            if paragraph[-1] == "\n":
                paragraph = paragraph[:-1]

            # split paragraph into words by space
            words = paragraph.split(" ")
            sentence = ""

            for word in words:
                no_sign_word = self.find_eliminate_sign(word)

                # if no_sign_word is not empty, add it to sentence
                if no_sign_word:
                    sentence += no_sign_word
                    # if an original word ends with period, add sentence to self.sentences
                    if word[-1] == '.':
                        self.sentences.append(sentence)
                        sentence = ""
                    else:
                        sentence += " "

            # if sentence is not empty
            if sentence:
                # if sentence ends with period, add it to self.sentences
                if sentence[-1] == '.':
                    self.sentences.append(sentence)
                    sentence = ""
        #########################################################
    
    """it is impossible to change the name of methods (functions) below"""
    def find_eliminate_sign(self, word):
        '''find all signs and eliminate them from given "word"
           (except the period(.) that makes the End Of Sentence)
           and return it
        '''
        
        no_sign_word = '' # changing this variable also okay
        #########################################################
        # complete the code below, following assignment guideline
        
        for index, char in enumerate(word):
            # if character is not a sign or a period ending the sentence, add it to no_sign_word
            if char.isalnum() or (char == '.' and index == len(word) - 1):
                no_sign_word += char
            else:
                # if character is a sign, add it to self.sign_dict
                if char in self.sign_dict:
                    self.sign_dict[char] += 1
                else:
                    self.sign_dict[char] = 1
        #########################################################
        return no_sign_word
    
    def get_sorted_sign(self):
        '''return a list 
           that contains (eliminated sign, the number of that sign) tuples 
           and is sorted by the number in descending
        '''
        #########################################################
        # complete the code below, following assignment guideline
        
        # sort dictionary of signs by the number in descending order
        sorted_sign_dict = sorted(self.sign_dict.items(), key=lambda x: x[1], reverse=True)

        return list(sorted_sign_dict)
        #########################################################
    
    def __len__(self):
        '''return the number of sentences'''
        #########################################################
        # complete the code below, following assignment guideline
        return len(self.sentences)
        #########################################################
        
    def __getitem__(self, idx):
        """return a sentence that corresponds to the given index "idx" """
        #########################################################
        # complete the code below, following assignment guideline
        return self.sentences[idx]
        #########################################################