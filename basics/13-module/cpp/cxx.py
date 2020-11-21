
def cxx_fn():
    print("I am cxx function")

class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age

    def print_info(self):
        print("name: {}, age: {}".format(self.name, self.age))
