#pragma once

#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

// 必要的头文件
#include <pybind11/pybind11.h>
// 容器相关头文件
#include <pybind11/stl.h>

namespace py = pybind11;

// 名字空间
namespace xxx {

// 全局函数
float Add(float a, float b) {
  return a + b;
}

// 在Python中以关键字参数调用
float Subtract(float a, float b) {
  return a - b;
}

// 在Python中设置函数参数默认值
float Multiply(float a, float b) {
  return a * b;
}

// 可变参数函数，在Python可以使用*args和**kwargs传递参数
void VarArgs(py::args args, py::kwargs kwargs) {
  for (const auto& e : args) {
    std::cout << e << ", ";
  }
  std::cout << "\n-----------------------\n";
  for (const auto& e : kwargs) {
    std::cout << e.first << "->" << e.second << "\n";
  }
}

// 枚举类型
enum class Color : uint8_t {
  kRed,
  kGreen,
  kBlue
};

// 输出颜色字符串表示
void PrintColor(Color color) {
  if (color == Color::kRed) {
    std::cout << "红色\n";
  } else if (color == Color::kGreen) {
    std::cout << "绿色\n";
  } else if (color == Color::kBlue) {
    std::cout << "蓝色\n";
  } else {
    std::cout << "未知\n";
  }
}

// 结构体类型
struct Bar {
  Bar() = default;

  // 带参数构造函数
  Bar(const std::string& grade, float score)
    : grade{grade}, score{score} {}

  std::string grade;
  float score;
};

// 基类
class Base {
public:
  Base() = default;

  // 基类成员函数
  void BasePrint() const {
    std::cout << "我是基类成员函数BasePrint" << std::endl;
  }

  virtual void Print() const {
    std::cout << "我是基类虚函数Print" << std::endl;
  }

  virtual ~Base() = default;
};

// 派生类
class Foo : public Base {
public:
  Foo() = default;

  // 带参数构造函数
  Foo(const std::string& name, int age)
    : name_{name}, age_{age} {}

  // setter
  void set_name(const std::string& name) {
    name_ = name;
  }

  // getter
  const std::string& get_name() const { return name_; }

  // 普通成员函数
  void ToString() const {
    std::cout << "大扎好，我叫" << name_ << "，今年" << age_ << "岁!" << std::endl;
  }

  // C++中std::vector/std::deque/std::list/std::array转换为Python中的list
  void AddToVector(const std::vector<float>& vec) {
    std::copy(vec.begin(), vec.end(), std::back_inserter(vec_));
  }

  // C++中std::set/std::unordered_set转换为Python中的set
  void AddToSet(const std::unordered_set<int>& s) {
    std::copy(s.begin(), s.end(), std::inserter(set_, set_.end()));
  }

  // C++中std::map/std::unordered_map转换为Python中的dict
  void AddToMap(const std::unordered_map<std::string, std::string>& m) {
    for (const auto& e : m) {
      map_.emplace(e.first, e.second);
    }
  }

  // 输出各成员容器的值
  void PrintContainer() const {
    std::cout << "std::vector --> python list:\n";
    for (const auto& e : vec_) {
      std::cout << e << ", ";
    }
    std::cout << "\n";

    std::cout << "std::set --> python set:\n";
    for (const auto& e : set_) {
      std::cout << e << ", ";
    }
    std::cout << "\n";

    std::cout << "std::map --> python dict:\n";
    for (const auto& e : map_) {
      std::cout << e.first << "->" << e.second << "\n";
    }
  }

  void Print() const override {
    std::cout << "我是派生类虚函数Print" << std::endl;
  }

  int Add(int a, int b) {
    return a + b;
  }

  // const重载
  int Add(int a, int b) const {
    return a + b;
  }

  // 参数不同的重载
  float Add(float a,  float b) {
    return a + b;
  }

  // C++派生类中定义带参数且有返回值虚函数
  virtual int GetElem(int i) const {
    assert((i >= 0 && i < vec_.size()));
    return vec_[i];
  }

  virtual ~Foo() = default;

private:
  std::string name_;
  int age_;
  std::vector<float> vec_;
  std::unordered_set<int> set_;
  std::unordered_map<std::string, std::string> map_;
};

// 为了在Python中继承C++中定义的类，须定义帮助类，并在帮助类中定义重载的虚函数
class PyBase : public Base {
public:
  using Base::Base;

  void Print() const override {
    // 使用PYBIND11_OVERRIDE宏定义重载，参数顺序依次是
    // 返回值类型，基类名称，重载函数名，函数参数
    // 注意参数和返回值为void时格式
    PYBIND11_OVERRIDE(void, Base, Print, );
  }
};

// 为了在Python中继承C++中定义的类，须定义帮助类，并在帮助类中定义重载的虚函数
class PyFoo : public Foo {
public:
  using Foo::Foo;

  void Print() const override {
    // 使用PYBIND11_OVERRIDE宏定义重载，参数顺序依次是
    // 返回值类型，基类名称，重载函数名，函数参数
    // 注意参数和返回值为void时格式
    PYBIND11_OVERRIDE(void, Foo, Print,);
  }

  int GetElem(int i) const override {
    // 使用PYBIND11_OVERRIDE宏定义重载，参数顺序依次是
    // 返回值类型，基类名称，重载函数名，函数参数
    // 注意参数和返回值格式
    PYBIND11_OVERRIDE(int, Foo, GetElem, i);
  }
};

}

// 以下代码如果定义在xxx名字空间中，则相应的绑定代码不需要加xxx名字空间

// 使用PYBIND11_MODULE这个宏定义模块，pycxx是模块名，m是模块实例
PYBIND11_MODULE(pycxx, m) {
  m.doc() = "在这里写下模块文档，这个模块是干什么的，有什么用等等";

  // 定义全局变量
  m.attr("pi") = 3.14;

  // 第一个参数为在Python中的函数名，第二个参数为在C++中的函数名，第三个参数为函数说明
  m.def("add", &xxx::Add, "简单的加法运算");

  // 最后两个参数分别为在Python中的关键字参数名，即在Python中可以这样调用subtract(a=3, b=5)
  m.def("subtract", &xxx::Subtract, "在Python中可以以关键字参数调用该函数", py::arg("a"), py::arg("b"));

  // 在Python中设置函数参数默认值，即在Python中调用multiply()等价于调用multiply(3, 5)
  m.def("multiply", &xxx::Multiply, "在Python中设置函数参数默认值", py::arg("a")=3, py::arg("b")=5);

  // 可变参数函数，在Python可以使用*args和**kwargs传递参数
  m.def("var_args", &xxx::VarArgs, "在Python可以使用*args和**kwargs传递参数");

  // 还可以直接定义lambda函数
  m.def("square_add", [](float a, float b) {
    return a*a + b*b;
  }, "平方和");

  // 使用pybind11::enum_绑定枚举，第一个参数为模块实例，第二个参数为枚举名
  py::enum_<xxx::Color>(m, "Color")
    .value("kRed", xxx::Color::kRed)
    .value("kGreen", xxx::Color::kGreen)
    .value("kBlue", xxx::Color::kBlue)
    .export_values();                                 // 导出枚举值

  m.def("print_color", &xxx::PrintColor, "输出颜色");

  // 使用pybind11::class_绑定类和结构体，模板参数为类类型，第一个参数为模块实例，第二个参数为类名或结构体名
  py::class_<xxx::Bar>(m, "Bar")
    .def(py::init<const std::string&, float>())       // 定义带参数的构造函数
    .def_readonly("grade", &xxx::Bar::grade)          // 定义只读成员，在Python中只能读该成员，不能修改该成员
    .def_readwrite("score", &xxx::Bar::score);        // 定义可读写成员，在Python中可以读写该成员

  // 第一个模板参数为类类型，后续模板参数为基类类型
  py::class_<xxx::Base, xxx::PyBase>(m, "Base")
    .def(py::init())                                  // 定义空的默认构造函数
    .def("base_print", &xxx::Base::BasePrint)         // base_print为在Python中的函数名，xxx::Base::BasePrint对应的C++函数名
    .def("print", &xxx::Base::Print);                 // 虚函数只需要在基类中定义一次

  // 第一个模板参数为类类型，后续模板参数为基类类型
  py::class_<xxx::Foo, xxx::Base, xxx::PyFoo>(m, "Foo")
    .def(py::init<const std::string&, int>())                               // 定义带参数的构造函数
    .def_property("name", &xxx::Foo::get_name, &xxx::Foo::set_name)         // 为private成员变量绑定getter/setter
    .def("to_string", &xxx::Foo::ToString)                                  // 绑定成员函数
    .def("append_list", &xxx::Foo::AddToVector)
    .def("append_set", &xxx::Foo::AddToSet)
    .def("append_dict", &xxx::Foo::AddToMap)
    .def("print_container", &xxx::Foo::PrintContainer)
    .def("add", py::overload_cast<int, int>(&xxx::Foo::Add))
    .def("add", py::overload_cast<int, int>(&xxx::Foo::Add, py::const_))    // const重载
    .def("add", py::overload_cast<float, float>(&xxx::Foo::Add))            // 参数不同重载
    .def("get_elem", &xxx::Foo::GetElem);
}
