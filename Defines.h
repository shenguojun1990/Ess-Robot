#ifndef DEFINES_H
#define DEFINES_H

#define Property_Declaration(name,type)\
    type name() const; \
    type& name();

#define Property_Implement(name,cn,mem,type)\
    type cn::name() const{return mem;}\
    type& cn::name() {return mem;}

#endif // DEFINES_H
