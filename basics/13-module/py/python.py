
# 跨包导入
import cpp.cxx as cxx

def python_fn():
    print("I am python function")

def call_cxx_from_python():
    p = cxx.Person("张三", 18)
    p.print_info()
