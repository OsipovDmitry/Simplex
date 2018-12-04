#ifndef CUSTOMDELETER_H
#define CUSTOMDELETER_H

#define CUSTOMDELETER(Class) \
    private: \
        class Class##Deleter { \
        public: \
            void operator ()(Class *ptr) { delete ptr; } \
        };

#endif // CUSTOMDELETER_H
