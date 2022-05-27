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

    Optional(const T& value)
    {
        // Версия с храненем указателя на объект в буфере
        //value_ = new (&data_[0]) T(value);  // при T* value_
        // Версия БЕЗ хранения указателя на объект в буфере
        if (is_initialized_)
        {
            new(data_) T(value);
        }
        else
        {
            new (&data_[0]) T(value);
            is_initialized_ = true;
        }
    }
    Optional(T&& value)
    {
        // Версия с храненем указателя на объект в буфере
        //value_ = new (&data_[0]) T(std::move(value));  // при T* value_
        // Версия БЕЗ хранения указателя на объект в буфере
        if (is_initialized_)
        {
            new(data_) T(std::move(value));
        }
        else
        {
            new (&data_[0]) T(std::move(value));
            is_initialized_ = true;
        }
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
        if (is_initialized_)
        {
            *(reinterpret_cast<T*>(data_)) = value;
        }
        else
        {
            new (&data_[0]) T(value);
            is_initialized_ = true;
        }
        return *this;
    }


    Optional& operator=(T&& rhs)
    {
        if (is_initialized_)
        {
            *(reinterpret_cast<T*>(data_)) = std::move(rhs);
        }
        else
        {
            new (&data_[0]) T(std::move(rhs));
        }
        is_initialized_ = true;
        return *this;
    }


    Optional& operator=(const Optional& rhs)
    {
        if (rhs.is_initialized_)
        {
            if (is_initialized_)
            {
                *(reinterpret_cast<T*>(data_)) = rhs.Value();
            }
            else
            {
                new (&data_[0]) T(rhs.Value());
                is_initialized_ = true;
            }
        }
        else
        {
            if (is_initialized_)
            {
                this->Reset();
            }
        }
        return *this;
    }


    Optional& operator=(Optional&& rhs)
    {
        if (rhs.is_initialized_)
        {
            if (is_initialized_)
            {
                *(reinterpret_cast<T*>(data_)) = std::move(rhs.Value());
            }
            else
            {
                new (&data_[0]) T(std::move(rhs.Value()));
                is_initialized_ = true;
            }
        }
        else
        {
            if (is_initialized_)
            {
                this->Reset();
            }
        }
        return *this;
    }

    ~Optional()
    {
        // Удаляем объект, если есть
        if (is_initialized_)
        {
            // Деструктор
            reinterpret_cast<T*>(data_)->~T();
            // Освобождаем память в буфере
            // Буфер на стеке, operator new/delete для буфера не требуется
            //delete (&data_);
            is_initialized_ = false;
        }
        // Буфер на стеке. Удалять его не требуется
    }

    /* Реализация Emplace в std::optional
    template <class... _Types>
    _Ty& emplace(_Types&&... _Args) {
        reset();
        return this->_Construct(_STD forward<_Types>(_Args)...);
    }

    template <class _Elem, class... _Types, enable_if_t<is_constructible_v<_Ty, initializer_list<_Elem>&, _Types...>, int> = 0>
    _Ty& emplace(initializer_list<_Elem> _Ilist, _Types&&... _Args) {
        reset();
        return this->_Construct(_Ilist, _STD forward<_Types>(_Args)...);
    }
    */

    template <class... U>
    Optional& Emplace(U&&... args)
    {
        // Если в Optional уже есть значение - стираем его 
        if (is_initialized_)
        {
            Reset();
        }
        // Вызываем конструктор типа Т через свертку аргументов Variadic Template
        new (&data_[0]) T(std::forward<U>(args)...);
        is_initialized_ = true;
        return *this;
    }

    bool HasValue() const
    {
        return is_initialized_;
    }

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*()
    {
        return reinterpret_cast<T&>(*data_);
    }

    const T& operator*() const
    {
        return reinterpret_cast<const T&>(*data_);
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

        return reinterpret_cast<T&>(*data_);
    }

    const T& Value() const
    {
        if (!this->HasValue())
        {
            throw BadOptionalAccess();
        }
        
        return reinterpret_cast<const T&>(*data_);
    }

    void Reset()
    {
        // Вызываем деструктор размещенного в выделенной памяти объекта
        reinterpret_cast<T*>(data_)->~T();
        // Парный operator delete для буфера
        // Буфер на стеке, operator new/delete для буфера не требуется
        //delete (&data_);
        is_initialized_ = false;

        // Буфер на стеке, удалять не требуется.
    }

private:
    // Буфер для хранения 1 объекта типа T.
    // alignas нужен для правильного выравнивания блока памяти
    // Буфер на стеке, operator new/delete для буфера не требуется
    alignas(T) char data_[sizeof(T)];
    // Указатель на размещенный при помощи operator new в буфере объект типа Т
    //T* value_ = nullptr;

    bool is_initialized_ = false;
};
