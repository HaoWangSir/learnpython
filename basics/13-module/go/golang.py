
def go_fn():
    print("I am go function")

class Gopher:
    def __init__(self):
        self.founder = ["Rob Pike", "Ken Thompson", "Robert Griesemer"]

    def tostring(self):
        print("Golang creaters: {}".format(" ".join(self.founder)))

