while True:
    print('I am in an endless loop')
    answer = input('Do you want to exit? (y / n) ')
    if answer is 'y':
        break
    elif answer is not 'n':
        print('Please write the valid answer.')
