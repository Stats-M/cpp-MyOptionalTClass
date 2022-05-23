#include <stdexcept>
#include <utility>

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
    Optional(const T& value);
    Optional(T&& value);
    Optional(const Optional& other);
    Optional(Optional&& other);

    Optional& operator=(const T& value);
    Optional& operator=(T&& rhs);
    Optional& operator=(const Optional& rhs);
    Optional& operator=(Optional&& rhs);

    ~Optional();

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
        return *value_;
    }

    void Reset()
    {
        // Сначала вызываем деструктор размещенного в выделенной
        // памяти объекта
        value_->~T();
        // Затем удаляем буфер
        operator delete (data_);
    }

private:
    // Буфер для хранения 1 объекта типа T
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    // Указатель на размещенный при помощи operator new в буфере объект типа Т
    T* value_ = nullptr;

    bool is_initialized_ = false;
};
