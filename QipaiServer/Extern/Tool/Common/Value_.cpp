#include "Value_.h"
#include <iomanip>

const ValueVector ValueVectorNull;
const ValueMap ValueMapNull;
const ValueMapIntKey ValueMapIntKeyNull;
const Value_ Value_::Null;

#define MAX_ITOA_BUFFER_SIZE 256

#define MING_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define MINGASSERT(cond, msg) do { \
if (!(cond)) { \
printf(msg); \
assert(cond); \
} \
} while (0)

double Value_::AtoF(const char* str) const
{
	if (str == nullptr)
	{
		return 0.0;
	}

	char buf[MAX_ITOA_BUFFER_SIZE];
	strncpy(buf, str, MAX_ITOA_BUFFER_SIZE);

	// strip string, only remain 7 numbers after '.'
	char* dot = strchr(buf, '.');
	if (dot != nullptr && dot - buf + 8 < MAX_ITOA_BUFFER_SIZE)
	{
		dot[8] = '\0';
	}

	return ::atof(buf);
}

Value_::Value_()
: _type(Type::NONE)
{
    memset(&_field, 0, sizeof(_field));
}

Value_::Value_(unsigned char v)
: _type(Type::BYTE)
{
    _field.byteVal = v;
}

Value_::Value_(int v)
: _type(Type::INTEGER)
{
    _field.intVal = v;
}

Value_::Value_(unsigned int v)
: _type(Type::UNSIGNED)
{
    _field.unsignedVal = v;
}

Value_::Value_(float v)
: _type(Type::FLOAT)
{
    _field.floatVal = v;
}

Value_::Value_(double v)
: _type(Type::DOUBLE)
{
    _field.doubleVal = v;
}

Value_::Value_(bool v)
: _type(Type::BOOLEAN)
{
    _field.boolVal = v;
}

Value_::Value_(const char* v)
: _type(Type::STRING)
{
    _field.strVal = new (std::nothrow) std::string();
    if (v)
    {
        *_field.strVal = v;
    }
}

Value_::Value_(const std::string& v)
: _type(Type::STRING)
{
    _field.strVal = new (std::nothrow) std::string();
    *_field.strVal = v;
}

Value_::Value_(const ValueVector& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) ValueVector();
    *_field.vectorVal = v;
}

Value_::Value_(ValueVector&& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) ValueVector();
    *_field.vectorVal = std::move(v);
}

Value_::Value_(const ValueMap& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) ValueMap();
    *_field.mapVal = v;
}

Value_::Value_(ValueMap&& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) ValueMap();
    *_field.mapVal = std::move(v);
}

Value_::Value_(const ValueMapIntKey& v)
: _type(Type::INT_KEY_MAP)
{
    _field.intKeyMapVal = new (std::nothrow) ValueMapIntKey();
    *_field.intKeyMapVal = v;
}

Value_::Value_(ValueMapIntKey&& v)
: _type(Type::INT_KEY_MAP)
{
    _field.intKeyMapVal = new (std::nothrow) ValueMapIntKey();
    *_field.intKeyMapVal = std::move(v);
}

Value_::Value_(const Value_& other)
: _type(Type::NONE)
{
    *this = other;
}

Value_::Value_(Value_&& other)
: _type(Type::NONE)
{
    *this = std::move(other);
}

Value_::~Value_()
{
    clear();
}

Value_& Value_::operator= (const Value_& other)
{
    if (this != &other) {
        reset(other._type);

        switch (other._type) {
            case Type::BYTE:
                _field.byteVal = other._field.byteVal;
                break;
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::UNSIGNED:
                _field.unsignedVal = other._field.unsignedVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                if (_field.strVal == nullptr)
                {
                    _field.strVal = new std::string();
                }
                *_field.strVal = *other._field.strVal;
                break;
            case Type::VECTOR:
                if (_field.vectorVal == nullptr)
                {
                    _field.vectorVal = new (std::nothrow) ValueVector();
                }
                *_field.vectorVal = *other._field.vectorVal;
                break;
            case Type::MAP:
                if (_field.mapVal == nullptr)
                {
                    _field.mapVal = new (std::nothrow) ValueMap();
                }
                *_field.mapVal = *other._field.mapVal;
                break;
            case Type::INT_KEY_MAP:
                if (_field.intKeyMapVal == nullptr)
                {
                    _field.intKeyMapVal = new (std::nothrow) ValueMapIntKey();
                }
                *_field.intKeyMapVal = *other._field.intKeyMapVal;
                break;
            default:
                break;
        }
    }
    return *this;
}

Value_& Value_::operator= (Value_&& other)
{
    if (this != &other)
    {
        clear();
        switch (other._type)
        {
            case Type::BYTE:
                _field.byteVal = other._field.byteVal;
                break;
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::UNSIGNED:
                _field.unsignedVal = other._field.unsignedVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                _field.strVal = other._field.strVal;
                break;
            case Type::VECTOR:
                _field.vectorVal = other._field.vectorVal;
                break;
            case Type::MAP:
                _field.mapVal = other._field.mapVal;
                break;
            case Type::INT_KEY_MAP:
                _field.intKeyMapVal = other._field.intKeyMapVal;
                break;
            default:
                break;
        }
        _type = other._type;

        memset(&other._field, 0, sizeof(other._field));
        other._type = Type::NONE;
    }

    return *this;
}

Value_& Value_::operator= (unsigned char v)
{
    reset(Type::BYTE);
    _field.byteVal = v;
    return *this;
}

Value_& Value_::operator= (int v)
{
    reset(Type::INTEGER);
    _field.intVal = v;
    return *this;
}

Value_& Value_::operator= (unsigned int v)
{
    reset(Type::UNSIGNED);
    _field.unsignedVal = v;
    return *this;
}

Value_& Value_::operator= (float v)
{
    reset(Type::FLOAT);
    _field.floatVal = v;
    return *this;
}

Value_& Value_::operator= (double v)
{
    reset(Type::DOUBLE);
    _field.doubleVal = v;
    return *this;
}

Value_& Value_::operator= (bool v)
{
    reset(Type::BOOLEAN);
    _field.boolVal = v;
    return *this;
}

Value_& Value_::operator= (const char* v)
{
    reset(Type::STRING);
    *_field.strVal = v ? v : "";
    return *this;
}

Value_& Value_::operator= (const std::string& v)
{
    reset(Type::STRING);
    *_field.strVal = v;
    return *this;
}

Value_& Value_::operator= (const ValueVector& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = v;
    return *this;
}

Value_& Value_::operator= (ValueVector&& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = std::move(v);
    return *this;
}

Value_& Value_::operator= (const ValueMap& v)
{
    reset(Type::MAP);
    *_field.mapVal = v;
    return *this;
}

Value_& Value_::operator= (ValueMap&& v)
{
    reset(Type::MAP);
    *_field.mapVal = std::move(v);
    return *this;
}

Value_& Value_::operator= (const ValueMapIntKey& v)
{
    reset(Type::INT_KEY_MAP);
    *_field.intKeyMapVal = v;
    return *this;
}

Value_& Value_::operator= (ValueMapIntKey&& v)
{
    reset(Type::INT_KEY_MAP);
    *_field.intKeyMapVal = std::move(v);
    return *this;
}

bool Value_::operator!= (const Value_& v)
{
    return !(*this == v);
}
bool Value_::operator!= (const Value_& v) const
{
    return !(*this == v);
}

bool Value_::operator== (const Value_& v)
{
    const auto &t = *this;
    return t == v;
}
bool Value_::operator== (const Value_& v) const
{
    if (this == &v) return true;
    if (v._type != this->_type) return false;
    if (this->isNull()) return true;
    switch (_type)
    {
        case Type::BYTE:    return v._field.byteVal     == this->_field.byteVal;
        case Type::INTEGER: return v._field.intVal      == this->_field.intVal;
        case Type::UNSIGNED:return v._field.unsignedVal == this->_field.unsignedVal;
        case Type::BOOLEAN: return v._field.boolVal     == this->_field.boolVal;
        case Type::STRING:  return *v._field.strVal     == *this->_field.strVal;
        case Type::FLOAT:   return std::abs(v._field.floatVal  - this->_field.floatVal)  <= FLT_EPSILON;
        case Type::DOUBLE:  return std::abs(v._field.doubleVal - this->_field.doubleVal) <= DBL_EPSILON;
        case Type::VECTOR:
        {
            const auto &v1 = *(this->_field.vectorVal);
            const auto &v2 = *(v._field.vectorVal);
            const auto size = v1.size();
            if (size == v2.size())
            {
                for (size_t i = 0; i < size; i++)
                {
                    if (v1[i] != v2[i]) return false;
                }
                return true;
            }
            return false;
        }
        case Type::MAP:
        {
            const auto &map1 = *(this->_field.mapVal);
            const auto &map2 = *(v._field.mapVal);
            for (const auto &kvp : map1)
            {
                auto it = map2.find(kvp.first);
                if (it == map2.end() || it->second != kvp.second)
                {
                    return false;
                }
            }
            return true;
        }
        case Type::INT_KEY_MAP:
        {
            const auto &map1 = *(this->_field.intKeyMapVal);
            const auto &map2 = *(v._field.intKeyMapVal);
            for (const auto &kvp : map1)
            {
                auto it = map2.find(kvp.first);
                if (it == map2.end() || it->second != kvp.second)
                {
                    return false;
                }
            }
            return true;
        }
        default:
            break;
    };

    return false;
}

/// Convert value to a specified type
unsigned char Value_::asByte() const
{
	MINGASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");

    if (_type == Type::BYTE)
    {
        return _field.byteVal;
    }

    if (_type == Type::INTEGER)
    {
        return static_cast<unsigned char>(_field.intVal);
    }

    if (_type == Type::UNSIGNED)
    {
        return static_cast<unsigned char>(_field.unsignedVal);
    }

    if (_type == Type::STRING)
    {
        return static_cast<unsigned char>(atoi(_field.strVal->c_str()));
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<unsigned char>(_field.floatVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<unsigned char>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }

    return 0;
}

int Value_::asInt() const
{
	MINGASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::INTEGER)
    {
        return _field.intVal;
    }

    if (_type == Type::UNSIGNED)
    {
		MINGASSERT(_field.unsignedVal < INT_MAX, "Can only convert values < INT_MAX");
        return (int)_field.unsignedVal;
    }

    if (_type == Type::BYTE)
    {
        return _field.byteVal;
    }

    if (_type == Type::STRING)
    {
        return atoi(_field.strVal->c_str());
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<int>(_field.floatVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<int>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }

    return 0;
}


unsigned int Value_::asUnsignedInt() const
{
	MINGASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::UNSIGNED)
    {
        return _field.unsignedVal;
    }

    if (_type == Type::INTEGER)
    {
		MINGASSERT(_field.intVal >= 0, "Only values >= 0 can be converted to unsigned");
        return static_cast<unsigned int>(_field.intVal);
    }

    if (_type == Type::BYTE)
    {
        return static_cast<unsigned int>(_field.byteVal);
    }

    if (_type == Type::STRING)
    {
        // NOTE: strtoul is required (need to augment on unsupported platforms)
        return static_cast<unsigned int>(strtoul(_field.strVal->c_str(), nullptr, 10));
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<unsigned int>(_field.floatVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<unsigned int>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1u : 0u;
    }

    return 0u;
}

float Value_::asFloat() const
{
	MINGASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::FLOAT)
    {
        return _field.floatVal;
    }

    if (_type == Type::BYTE)
    {
        return static_cast<float>(_field.byteVal);
    }

    if (_type == Type::STRING)
    {
        return (float)AtoF(_field.strVal->c_str());
    }

    if (_type == Type::INTEGER)
    {
        return static_cast<float>(_field.intVal);
    }

    if (_type == Type::UNSIGNED)
    {
        return static_cast<float>(_field.unsignedVal);
    }

    if (_type == Type::DOUBLE)
    {
        return static_cast<float>(_field.doubleVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0f : 0.0f;
    }

    return 0.0f;
}

double Value_::asDouble() const
{
	MINGASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal;
    }

    if (_type == Type::BYTE)
    {
        return static_cast<double>(_field.byteVal);
    }

    if (_type == Type::STRING)
    {
        return static_cast<double>(AtoF(_field.strVal->c_str()));
    }

    if (_type == Type::INTEGER)
    {
        return static_cast<double>(_field.intVal);
    }

    if (_type == Type::UNSIGNED)
    {
        return static_cast<double>(_field.unsignedVal);
    }

    if (_type == Type::FLOAT)
    {
        return static_cast<double>(_field.floatVal);
    }

    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0 : 0.0;
    }

    return 0.0;
}

bool Value_::asBool() const
{
	MINGASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal;
    }

    if (_type == Type::BYTE)
    {
        return _field.byteVal == 0 ? false : true;
    }

    if (_type == Type::STRING)
    {
        return (*_field.strVal == "0" || *_field.strVal == "false") ? false : true;
    }

    if (_type == Type::INTEGER)
    {
        return _field.intVal == 0 ? false : true;
    }

    if (_type == Type::UNSIGNED)
    {
        return _field.unsignedVal == 0 ? false : true;
    }

    if (_type == Type::FLOAT)
    {
        return _field.floatVal == 0.0f ? false : true;
    }

    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal == 0.0 ? false : true;
    }

    return false;
}

std::string Value_::asString() const
{
	MINGASSERT(_type != Type::VECTOR && _type != Type::MAP && _type != Type::INT_KEY_MAP, "Only base type (bool, string, float, double, int) could be converted");

    if (_type == Type::STRING)
    {
        return *_field.strVal;
    }

    std::stringstream ret;

    switch (_type)
    {
        case Type::BYTE:
            ret << _field.byteVal;
            break;
        case Type::INTEGER:
            ret << _field.intVal;
            break;
        case Type::UNSIGNED:
            ret << _field.unsignedVal;
            break;
        case Type::FLOAT:
            ret << std::fixed << std::setprecision( 7 )<< _field.floatVal;
            break;
        case Type::DOUBLE:
            ret << std::fixed << std::setprecision( 16 ) << _field.doubleVal;
            break;
        case Type::BOOLEAN:
            ret << (_field.boolVal ? "true" : "false");
            break;
        default:
            break;
    }
    return ret.str();
}

ValueVector& Value_::asValueVector()
{
	MINGASSERT(_type == Type::VECTOR, "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

const ValueVector& Value_::asValueVector() const
{
	MINGASSERT(_type == Type::VECTOR, "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

ValueMap& Value_::asValueMap()
{
	MINGASSERT(_type == Type::MAP, "The value type isn't Type::MAP");
    return *_field.mapVal;
}

const ValueMap& Value_::asValueMap() const
{
	MINGASSERT(_type == Type::MAP, "The value type isn't Type::MAP");
    return *_field.mapVal;
}

ValueMapIntKey& Value_::asIntKeyMap()
{
	MINGASSERT(_type == Type::INT_KEY_MAP, "The value type isn't Type::INT_KEY_MAP");
    return *_field.intKeyMapVal;
}

const ValueMapIntKey& Value_::asIntKeyMap() const
{
	MINGASSERT(_type == Type::INT_KEY_MAP, "The value type isn't Type::INT_KEY_MAP");
    return *_field.intKeyMapVal;
}

static std::string getTabs(int depth)
{
    std::string tabWidth;

    for (int i = 0; i < depth; ++i)
    {
        tabWidth += "\t";
    }

    return tabWidth;
}

static std::string visit(const Value_& v, int depth);

static std::string visitVector(const ValueVector& v, int depth)
{
    std::stringstream ret;

    if (depth > 0)
        ret << "\n";

    ret << getTabs(depth) << "[\n";

    int i = 0;
    for (const auto& child : v)
    {
        ret << getTabs(depth+1) << i << ": " << visit(child, depth + 1);
        ++i;
    }

    ret << getTabs(depth) << "]\n";

    return ret.str();
}

template <class T>
static std::string visitMap(const T& v, int depth)
{
    std::stringstream ret;

    if (depth > 0)
        ret << "\n";

    ret << getTabs(depth) << "{\n";

    for (auto iter = v.begin(); iter != v.end(); ++iter)
    {
        ret << getTabs(depth + 1) << iter->first << ": ";
        ret << visit(iter->second, depth + 1);
    }

    ret << getTabs(depth) << "}\n";

    return ret.str();
}

static std::string visit(const Value_& v, int depth)
{
    std::stringstream ret;

    switch (v.getType())
    {
        case Value_::Type::NONE:
        case Value_::Type::BYTE:
        case Value_::Type::INTEGER:
        case Value_::Type::UNSIGNED:
        case Value_::Type::FLOAT:
        case Value_::Type::DOUBLE:
        case Value_::Type::BOOLEAN:
        case Value_::Type::STRING:
            ret << v.asString() << "\n";
            break;
        case Value_::Type::VECTOR:
            ret << visitVector(v.asValueVector(), depth);
            break;
        case Value_::Type::MAP:
            ret << visitMap(v.asValueMap(), depth);
            break;
        case Value_::Type::INT_KEY_MAP:
            ret << visitMap(v.asIntKeyMap(), depth);
            break;
        default:
			MINGASSERT(false, "Invalid type!");
            break;
    }

    return ret.str();
}

std::string Value_::getDescription() const
{
    std::string ret("\n");
    ret += visit(*this, 0);
    return ret;
}

void Value_::clear()
{
    // Free memory the old value allocated
    switch (_type)
    {
        case Type::BYTE:
            _field.byteVal = 0;
            break;
        case Type::INTEGER:
            _field.intVal = 0;
            break;
        case Type::UNSIGNED:
            _field.unsignedVal = 0u;
            break;
        case Type::FLOAT:
            _field.floatVal = 0.0f;
            break;
        case Type::DOUBLE:
            _field.doubleVal = 0.0;
            break;
        case Type::BOOLEAN:
            _field.boolVal = false;
            break;
        case Type::STRING:
			MING_SAFE_DELETE(_field.strVal);
            break;
        case Type::VECTOR:
			MING_SAFE_DELETE(_field.vectorVal);
            break;
        case Type::MAP:
			MING_SAFE_DELETE(_field.mapVal);
            break;
        case Type::INT_KEY_MAP:
			MING_SAFE_DELETE(_field.intKeyMapVal);
            break;
        default:
            break;
    }

    _type = Type::NONE;
}

void Value_::reset(Type type)
{
    if (_type == type)
        return;

    clear();

    // Allocate memory for the new value
    switch (type)
    {
        case Type::STRING:
            _field.strVal = new (std::nothrow) std::string();
            break;
        case Type::VECTOR:
            _field.vectorVal = new (std::nothrow) ValueVector();
            break;
        case Type::MAP:
            _field.mapVal = new (std::nothrow) ValueMap();
            break;
        case Type::INT_KEY_MAP:
            _field.intKeyMapVal = new (std::nothrow) ValueMapIntKey();
            break;
        default:
            break;
    }

    _type = type;
}
