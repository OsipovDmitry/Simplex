#include <QApplication>
#include "gleswidget.h"


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    GLESWidget w1;
    w1.setWindowTitle("Window 1");
    w1.show();

    return app.exec();
}


//class MyClass;
//using MyClassPtr = std::shared_ptr<MyClass>;

//class MyClass {
//public:
//    ~MyClass() {}
//    // ...

//    static void del(MyClass *p) {
//        delete p;
//    }
//    static MyClassPtr generate() {
//        MyClassPtr obj(new MyClass, del);
//        return obj;
//    }

//protected:
//    MyClass() {}

//};

//int main(int argc, char **argv)
//{
//    auto p1 = MyClass::generate();
//    int i = sizeof(p1);
//    auto p2 = MyClass::generate();
//}
