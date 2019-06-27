/***********************************************************************
#   > File Name   : util.hpp
#   > Author      : ronaldo
#   > Description : 
#   > Create Time : 2019-06-21 15:57:34
***********************************************************************/

namespace co {

class RefObject;  // 前置声明，否则Deleter编译不过

class Deleter {
 public:
  typedef void (*func_t)(RefObject *ptr, void *arg);
  Deleter() : func_(nullptr), arg_(nullptr) {}
  Deleter(func_t func, void *arg) : func_(func), arg_(arg) {}

  // 私有成员变量func_和arg_都是指针类型，都是可拷贝的
  // 编译器会自动合成拷贝构造函数，因此Deleter的实例也是可拷贝的

  // 析构函数也由编译器合成，func_和arg_两个指针被自动销毁（不影响指针所指的实例）

  void operator()(RefObject *ptr) {
    if (func_) {
      func_(ptr, arg_);
    } else {
      delete ptr;
    }
  }

  private:
  // 存储函数指针（如Scheduler的static成员函数delete_task的函数名）或一个可调用对象的副本
  func_t func_;
  // 例如：设置为Deleter实例所属的Task所属的Scheduler实例的地址
  void * arg_;
};

class RefObject {
 public:
  RefObject() {

  }

  virtual ~RefObject() {}

  // 必须赋值一份，因为Scheduler中生成的Deleter临时变量马上会被销毁
  void set_deleter(Deleter /*&*/deleter) {  // 用ref会编译不过
    deleter_ = deleter;
  }

 private:
  Deleter deleter_;   
};

class SharedRefObject : public RefObject {

};

}  // namespace co
