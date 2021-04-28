
#ifndef __Value__
#define __Value__

#include "CommonHead.h"

#include <unordered_map>


class Value_;

typedef std::vector<Value_> ValueVector;
typedef std::unordered_map<std::string, Value_> ValueMap;
typedef std::unordered_map<int, Value_> ValueMapIntKey;

extern const ValueVector ValueVectorNull;
extern const ValueMap ValueMapNull;
extern const ValueMapIntKey ValueMapIntKeyNull;

class  Value_
{
public:
    /** A predefined Value that has not value. */
    static const Value_ Null;

    /** Default constructor. */
    Value_();
    
    /** Create a Value by an unsigned char value. */
    explicit Value_(unsigned char v);
    
    /** Create a Value by an integer value. */
    explicit Value_(int v);

    /** Create a Value by an unsigned value. */
    explicit Value_(unsigned int v);

    /** Create a Value by a float value. */
    explicit Value_(float v);
    
    /** Create a Value by a double value. */
    explicit Value_(double v);
    
    /** Create a Value by a bool value. */
    explicit Value_(bool v);
    
    /** Create a Value by a char pointer. It will copy the chars internally. */
    explicit Value_(const char* v);
    
    /** Create a Value by a string. */
    explicit Value_(const std::string& v);
    
    /** Create a Value by a ValueVector object. */
    explicit Value_(const ValueVector& v);
    /** Create a Value by a ValueVector object. It will use std::move internally. */
    explicit Value_(ValueVector&& v);
    
    /** Create a Value by a ValueMap object. */
    explicit Value_(const ValueMap& v);
    /** Create a Value by a ValueMap object. It will use std::move internally. */
    explicit Value_(ValueMap&& v);
    
    /** Create a Value by a ValueMapIntKey object. */
    explicit Value_(const ValueMapIntKey& v);
    /** Create a Value by a ValueMapIntKey object. It will use std::move internally. */
    explicit Value_(ValueMapIntKey&& v);

    /** Create a Value by another Value object. */
    Value_(const Value_& other);
    /** Create a Value by a Value object. It will use std::move internally. */
    Value_(Value_&& other);
    
    /** Destructor. */
    ~Value_();

    /** Assignment operator, assign from Value to Value. */
    Value_& operator= (const Value_& other);
    /** Assignment operator, assign from Value to Value. It will use std::move internally. */
    Value_& operator= (Value_&& other);

    /** Assignment operator, assign from unsigned char to Value. */
    Value_& operator= (unsigned char v);
    /** Assignment operator, assign from integer to Value. */
    Value_& operator= (int v);
    /** Assignment operator, assign from integer to Value. */
    Value_& operator= (unsigned int v);
    /** Assignment operator, assign from float to Value. */
    Value_& operator= (float v);
    /** Assignment operator, assign from double to Value. */
    Value_& operator= (double v);
    /** Assignment operator, assign from bool to Value. */
    Value_& operator= (bool v);
    /** Assignment operator, assign from char* to Value. */
    Value_& operator= (const char* v);
    /** Assignment operator, assign from string to Value. */
    Value_& operator= (const std::string& v);

    /** Assignment operator, assign from ValueVector to Value. */
    Value_& operator= (const ValueVector& v);
    /** Assignment operator, assign from ValueVector to Value. */
    Value_& operator= (ValueVector&& v);

    /** Assignment operator, assign from ValueMap to Value. */
    Value_& operator= (const ValueMap& v);
    /** Assignment operator, assign from ValueMap to Value. It will use std::move internally. */
    Value_& operator= (ValueMap&& v);

    /** Assignment operator, assign from ValueMapIntKey to Value. */
    Value_& operator= (const ValueMapIntKey& v);
    /** Assignment operator, assign from ValueMapIntKey to Value. It will use std::move internally. */
    Value_& operator= (ValueMapIntKey&& v);

    /** != operator overloading */
    bool operator!= (const Value_& v);
    /** != operator overloading */
    bool operator!= (const Value_& v) const;
    /** == operator overloading */
    bool operator== (const Value_& v);
    /** == operator overloading */
    bool operator== (const Value_& v) const;

    /** Gets as a byte value. Will convert to unsigned char if possible, or will trigger assert error. */
    unsigned char asByte() const;
    /** Gets as an integer value. Will convert to integer if possible, or will trigger assert error. */
    int asInt() const;
    /** Gets as an unsigned value. Will convert to unsigned if possible, or will trigger assert error. */
    unsigned int asUnsignedInt() const;
    /** Gets as a float value. Will convert to float if possible, or will trigger assert error. */
    float asFloat() const;
    /** Gets as a double value. Will convert to double if possible, or will trigger assert error. */
    double asDouble() const;
    /** Gets as a bool value. Will convert to bool if possible, or will trigger assert error. */
    bool asBool() const;
    /** Gets as a string value. Will convert to string if possible, or will trigger assert error. */
    std::string asString() const;

    /** Gets as a ValueVector reference. Will convert to ValueVector if possible, or will trigger assert error. */
    ValueVector& asValueVector();
    /** Gets as a const ValueVector reference. Will convert to ValueVector if possible, or will trigger assert error. */
    const ValueVector& asValueVector() const;

    /** Gets as a ValueMap reference. Will convert to ValueMap if possible, or will trigger assert error. */
    ValueMap& asValueMap();
    /** Gets as a const ValueMap reference. Will convert to ValueMap if possible, or will trigger assert error. */
    const ValueMap& asValueMap() const;

    /** Gets as a ValueMapIntKey reference. Will convert to ValueMapIntKey if possible, or will trigger assert error. */
    ValueMapIntKey& asIntKeyMap();
    /** Gets as a const ValueMapIntKey reference. Will convert to ValueMapIntKey if possible, or will trigger assert error. */
    const ValueMapIntKey& asIntKeyMap() const;

    /**
     * Checks if the Value is null.
     * @return True if the Value is null, false if not.
     */
    inline bool isNull() const { return _type == Type::NONE; }

    /** Value type wrapped by Value. */
    enum class Type
    {
        /// no value is wrapped, an empty Value
        NONE = 0,
        /// wrap byte
        BYTE,
        /// wrap integer
        INTEGER,
        /// wrap unsigned
        UNSIGNED,
        /// wrap float
        FLOAT,
        /// wrap double
        DOUBLE,
        /// wrap bool
        BOOLEAN,
        /// wrap string
        STRING,
        /// wrap vector
        VECTOR,
        /// wrap ValueMap
        MAP,
        /// wrap ValueMapIntKey
        INT_KEY_MAP
    };

    /** Gets the value type. */
    inline Type getType() const { return _type; }

    /** Gets the description of the class. */
    std::string getDescription() const;

private:
    void clear();
    void reset(Type type);
	double AtoF(const char* str) const;

    union
    {
        unsigned char byteVal;
        int intVal;
        unsigned int unsignedVal;
        float floatVal;
        double doubleVal;
        bool boolVal;

        std::string* strVal;
        ValueVector* vectorVal;
        ValueMap* mapVal;
        ValueMapIntKey* intKeyMapVal;
    }_field;

    Type _type;
};

#endif /* defined(__Value__) */
