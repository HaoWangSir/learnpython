
# 同级包模块导入
from .cxx import cxx_fn

def cc_fn():
    print("I am cc function")
    
def call_cxx_from_cc():
    cxx_fn()
