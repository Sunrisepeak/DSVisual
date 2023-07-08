#ifndef __DS_CORE_H__DSVISUAL
#define __DS_CORE_H__DSVISUAL

namespace dsvisual {

template <typename T, typename DSType>
class _PrimitiveIterator : public dstruct::PrimitiveIterator<T> {
private:
    using __Self = _PrimitiveIterator;
    using DStruct = dstruct::PrimitiveIterator<T>;

public: // bigfive
    _PrimitiveIterator(T *ptr, DSType *dsPtr) : DStruct(ptr), _mADSPtr { dsPtr } { }

public: // base
    typename DStruct::ReferenceType operator*() const {
        _mADSPtr->_updateIterator(*this, true);
        return DStruct::operator*();
    };

public: // ForwardIterator
    __Self& operator++() { DStruct::_mPointer++; _mADSPtr->_updateIterator(*this); return *this; }
    __Self operator++(int) {
        auto old = *this;
        DStruct::_mPointer++;
        _mADSPtr->_updateIterator(*this);
        return old;
    }
public: // BidirectionalIterator
    __Self& operator--() { DStruct::_mPointer--; _mADSPtr->_updateIterator(*this); return *this; }
    __Self operator--(int) {
        auto old = *this;
        DStruct::_mPointer--;
        _mADSPtr->_updateIterator(*this);
        return old;
    }
public: // RandomIterator
    __Self operator+(const int &n) const { return DStruct::_mPointer + n; };
    __Self operator-(const int &n) const { return DStruct::_mPointer -n; };

protected:
    DSType *_mADSPtr;
};

}

#endif