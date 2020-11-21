
import cpp.cc
import cpp.cxx as cxx

from go.golang import Gopher, go_fn
from py import python

def main():
    cpp.cc.cc_fn()
    cpp.cc.call_cxx_from_cc()

    cxx.cxx_fn()

    python.python_fn()
    python.call_cxx_from_python()

    go_fn()
    obj = Gopher()
    obj.tostring()

if __name__ == "__main__":
    main()
