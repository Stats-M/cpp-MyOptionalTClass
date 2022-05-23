#include <stdexcept>
#include <utility>
#include <cstring>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception
{
public:
    using exception::exception;

    virtual const char* what() const noexcept override
    {
        return "Bad optional access";
    }
};


template <typename T>
class Optional
{
public:
    Optional() = default;

    Optional(const T& value) : is_initialized_(true)
    {
        // Версия с храненем указателя на объект в буфере
        //value_ = new (&data_[0]) T(value);
        // Версия БЕЗ хранения указателя на объект в буфере
        new (&data_[0]) T(value);
    }
    Optional(T&& value) : is_initialized_(true)
    {
        // Версия с храненем указателя на объект в буфере
        //value_ = new (&data_[0]) T(std::move(value));
        // Версия БЕЗ хранения указателя на объект в буфере
        new (&data_[0]) T(std::move(value));
    }
    Optional(const Optional& other)
    {
        if (other.is_initialized_)
        {
            new (&data_[0]) T(other.Value());
            is_initialized_ = true;
        }
    }
    Optional(Optional&& other)
    {
        if (other.is_initialized_)
        {
            new (&data_[0]) T(std::move(other.Value()));
            is_initialized_ = true;
        }
    }


    Optional& operator=(const T& value)
    {
        // Версия с храненем указателя на объект в буфере
        //value_ = new (&data_[0]) T(value);
        // Версия БЕЗ хранения указателя на объект в буфере
        new (&data_[0]) T(value);
        is_initialized_ = true;
        return *this;
    }


    Optional& operator=(T&& rhs)
    {
        // Версия с храненем указателя на объект в буфере
        //value_ = new (&data_[0]) T(std::move(value));
        // Версия БЕЗ хранения указателя на объект в буфере
        new (&data_[0]) T(std::move(rhs));
        is_initialized_ = true;
        return *this;
    }


    Optional& operator=(const Optional& rhs)
    {
        if (rhs.is_initialized_)
        {
            new (&data_[0]) T(rhs.Value());
            is_initialized_ = true;
        }
        return *this;
    }


    Optional& operator=(Optional&& rhs)
    {
        if (rhs.is_initialized_)
        {
            new (&data_[0]) T(std::move(rhs.Value()));
            is_initialized_ = true;
        }
        return *this;
    }

    ~Optional()
    {
        // Удаляем объект, если есть
        if (is_initialized_)
        {
            // Деструктор
            reinterpret_cast<T&>(this->data_[0]).~T();
            // Освобождаем память в буфере
            delete (&data_[0]);
            // Не требуется уже, объект удаляется
            //is_initialized_ = false;
        }
        // Буфер на стеке. Удалять его не требуется
    }

    bool HasValue() const
    {
        return is_initialized_;
    }

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*()
    {
        return reinterpret_cast<T&>(this->data_[0]);
    }

    const T& operator*() const
    {
        return reinterpret_cast<const T&>(this->data_[0]);
    }

    T* operator->()
    {
        return reinterpret_cast<T*>(data_);
    }

    const T* operator->() const
    {
        return reinterpret_cast<const T*>(data_);
    }

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value()
    {
        if (!this->HasValue())
        {
            throw BadOptionalAccess();
        }

        return reinterpret_cast<T&>(this->data_[0]);
    }

    const T& Value() const
    {
        if (!this->HasValue())
        {
            throw BadOptionalAccess();
        }
        
        return reinterpret_cast<const T&>(this->data_);
    }

    void Reset()
    {
        // Вызываем деструктор размещенного в выделенной памяти объекта
        reinterpret_cast<T*>(data_[0])->~T();
        // Парный operator delete для буфера
        delete (&data_[0]);
        is_initialized_ = false;

        // Буфер на стеке, удалять не требуется.
    }

private:
    // Буфер для хранения 1 объекта типа T.
    // alignas нужен для правильного выравнивания блока памяти
    // Буфер на стеке, operator new/delete не требуется
    alignas(T) char data_[sizeof(T)];
    // Указатель на размещенный при помощи operator new в буфере объект типа Т
    //T* value_ = nullptr;

    bool is_initialized_ = false;
};
