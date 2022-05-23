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

    Optional& operator=(const T& value);
    Optional& operator=(T&& rhs)
    {
        data_
    }
    Optional& operator=(const Optional& rhs)
    {
        // Копируем буфер
        std::strcpy(data_, rhs.data_);
        // Если rhs имеет флаг инициализации, создаем объект
        if (rhs.is_initialized_)
        {
            value_ = &(data_);
        }

        return *this;
    }
    Optional& operator=(Optional&& rhs);

    ~Optional()
    {
        // Удаляем объект, если есть
        if (is_initialized_)
        {
            value_->~T();
            // Не требуется уже, объект удаляется
            //is_initialized_ = false;
        }
        // Буфер на стеке. Удалять не требуется
        //operator delete (data_);
    }

    bool HasValue() const
    {
        return is_initialized_;
    }

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*()
    {
        return *this->value_;
    }

    const T& operator*() const
    {
        return *this->value_;
    }

    T* operator->()
    {
        return this->value_;
    }

    const T* operator->() const
    {
        return this->value_;
    }

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value()
    {
        if (!this->HasValue())
        {
            throw BadOptionalAccess();
        }

        return *value_;
    }

    const T& Value() const
    {
        if (!this->HasValue())
        {
            throw BadOptionalAccess();
        }

        return *value_;
    }

    void Reset()
    {
        // Вызываем деструктор размещенного в выделенной памяти объекта
        value_->~T();
        is_initialized_ = false;

        // Буфер не трогаем, Reset == удалению объекта и готовность
        // буфера к новому размещению
        // Буфер на стеке, удалять не требуется.
        //operator delete (data_);
    }

private:
    // Буфер для хранения 1 объекта типа T.
    // alignas нужен для правильного выравнивания блока памяти
    // Буфер на стеке, operator new/delete не требуется
    alignas(T) char data_[sizeof(T)];
    // Указатель на размещенный при помощи operator new в буфере объект типа Т
    T* value_ = nullptr;

    bool is_initialized_ = false;
};